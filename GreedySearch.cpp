//
// Created by luopw on 24-12-1.
//

#include "GreedySearch.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <ostream>
#include <queue>

#include "DataProc.h"
#include <climits>


Schedule GreedySearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, const int iter_count) {
    std::vector<Schedule> scheduleList;
    Schedule temp_schedule = schedule;
    std::vector<std::string> processlist1 = ScoreProcessForExchange(schedule, jobs, jobList);
    std::vector<std::string> processlist2 = ScoreProcessForMove(schedule, jobs, jobList);
    bool flag1 = false, flag2 = false;
    while (scheduleList.size() < iter_count && (processlist1.size()>0 || processlist2.size()> 0)) {
        flag1 = false, flag2 = false;
        if (processlist1.size() > 0) {
            temp_schedule = ExchangeNeighborSearch(schedule, jobs, jobList, processlist1, flag1);
            if (temp_schedule.get_schedule_id()!=-1) scheduleList.push_back(temp_schedule);
        }
        if (processlist2.size() > 0) {
            temp_schedule = MoveNeighborSearch(schedule, jobs, jobList, processlist2, flag2);
            if (temp_schedule.get_schedule_id()!=-1) scheduleList.push_back(temp_schedule);
        }
        if (!flag1 && !flag2) break;
    }
    if (scheduleList.size() == 0) {
        return schedule;
    }
    Schedule best_schedule = scheduleList[0];
    for (Schedule temp_schedule1: scheduleList) {
        if (temp_schedule1.get_TotalTime() < best_schedule.get_TotalTime()) {
            best_schedule = temp_schedule1;
        }
    }
    return best_schedule;
}

// 已弃用
Schedule GreedyAdjust(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList) {
    Schedule schedule1;

    const std::vector<std::vector<int>> graph = schedule.get_graph();
    std::vector<std::string> processList = schedule.get_processList();
    std::vector<Schedule_item> schedule_items = schedule.get_schedule_items();
    std::vector<std::string> key_process = schedule.GetKeyProcess();

    std::cout << std::endl;
    for (const auto &process: key_process) {
        std::cout << process << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // 遍历每个机器上的工序，如果该工序为关键工序，则将其移动到前一个工序，重新计算此时的时间开销，如果时间开销减小，则保留，否则还原
    bool flag = false;
    for (int i = 0; i<schedule_items.size(); i++) {
        for (int j = 1; j<schedule_items[i].process_count;j++) {
            int job_id = schedule_items[i].schedule_process[j].job_id;
            int process_id = schedule_items[i].schedule_process[j].process_id;
            std::string temp_process = std::to_string(job_id) + "-" + std::to_string(process_id);
            // std::cout << "now process: " << temp_process << std::endl;
            // for (int k = 0;k<key_process.size(); k++) {
                // if (temp_process == key_process[k]) {
                    // std::cout << schedule_items[i].schedule_process[j-1].job_id << "-" << schedule_items[i].schedule_process[j-1].process_id << std::endl;
                    schedule_items[i].schedule_process[j].job_id = schedule_items[i].schedule_process[j-1].job_id;
                    schedule_items[i].schedule_process[j].process_id = schedule_items[i].schedule_process[j-1].process_id;
                    schedule_items[i].schedule_process[j-1].job_id = job_id;
                    schedule_items[i].schedule_process[j-1].process_id = process_id;
                    std::vector<std::vector<int>> graph1 = ScheduleItemsToGraph(schedule1, schedule_items, jobs, jobList, false);
                    schedule1.set_graph(graph1);
                    // std::cout << hasCycle(graph1) << std::endl;
                    if (!hasCycle(graph1)) {
                        std::cout << "old time: " << schedule.get_TotalTime() << ", new time: " << CalculateTotalTime(schedule1) << std::endl;
                    }
                    if (!hasCycle(graph1) && CalculateTotalTime(schedule1) < schedule.get_TotalTime()) {
                        std::cout << "old time: " << schedule.get_TotalTime() << ", new time: " << CalculateTotalTime(schedule1) << std::endl;
                        flag = true;
                        break;
                    } else {
                        schedule_items[i].schedule_process[j-1].job_id = schedule_items[i].schedule_process[j].job_id;
                        schedule_items[i].schedule_process[j-1].process_id = schedule_items[i].schedule_process[j].process_id;
                        schedule_items[i].schedule_process[j].job_id = job_id;
                        schedule_items[i].schedule_process[j].process_id = process_id;
                    }
                // }
            // }
            // if (flag) break;
        }
        if (flag) break;
    }
    schedule1.set_schedule_id(schedule.get_schedule_id()+1);
    schedule1.set_machine_count(schedule.get_machine_count());
    schedule1.set_schedule_items(schedule_items);
    std::vector<std::vector<int>> graph1 = ScheduleItemsToGraph(schedule1, schedule_items, jobs, jobList, true);
    return schedule1;
}

bool hasCycle(const std::vector<std::vector<int>> &adjMatrix) {  // 判断该邻接链表是否有环，如果有环返回true，否则返回false
    const int n = adjMatrix.size();
    std::vector<int> inDegree(n, 0);

    // 计算每个节点的入度
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (adjMatrix[i][j] != -1) { // 存在一条从 i 到 j 的边
                inDegree[j]++;
            }
        }
    }

    // 将所有入度为 0 的节点加入队列
    std::queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    int visitedCount = 0;

    // 拓扑排序
    while (!q.empty()) {
        const int node = q.front();
        q.pop();
        visitedCount++;

        for (int j = 0; j < n; ++j) {
            if (adjMatrix[node][j] != -1) { // 存在一条从 node 到 j 的边
                inDegree[j]--;
                if (inDegree[j] == 0) {
                    q.push(j);
                }
            }
        }
    }
    // 如果未能访问所有节点，说明存在环
    return visitedCount != n;
}


Schedule ExchangeNeighborSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<std::string> &processlist, bool &flag) {
    Schedule res_schedule = schedule;
    res_schedule.set_schedule_id(-1);

    std::vector<Schedule_item> schedule_items = schedule.get_schedule_items();
    // TODO: 贪心邻域搜索，基于相同机器上相邻的工序进行最大程度的交换，返回调整后的调度方案
    // for (int i =0;i< processlist.size();i++) {
    //     std::cout << processlist[i] << " ";
    // }
    // std::cout << std::endl;
    int machine_id = -1, item_id = -1, job_id = 0, process_id = 0;
    std::string temp_process = "";
    for (auto process: processlist) {
        GetMachineIdAndItemIdByProcess(schedule_items, process, 1, machine_id, item_id);
        if (item_id>0) {
            job_id = schedule_items[machine_id].schedule_process[item_id].job_id;
            process_id = schedule_items[machine_id].schedule_process[item_id].process_id;
            schedule_items[machine_id].schedule_process[item_id].job_id = schedule_items[machine_id].schedule_process[item_id-1].job_id;
            schedule_items[machine_id].schedule_process[item_id].process_id = schedule_items[machine_id].schedule_process[item_id-1].process_id;
            schedule_items[machine_id].schedule_process[item_id-1].job_id = job_id;
            schedule_items[machine_id].schedule_process[item_id-1].process_id = process_id;
            std::vector<std::vector<int>> graph1 = ScheduleItemsToGraph(res_schedule, schedule_items, jobs, jobList, false);
            res_schedule.set_graph(graph1);
            // std::cout << job_id << "-" << process_id << std::endl;
            // std::cout << "now graph status: " << hasCycle(graph1) << std::endl;
            // if (!hasCycle(graph1)) std::cout << "old time: " << schedule.get_TotalTime() << ", new time: " << CalculateTotalTime(res_schedule) << std::endl;
            if (!hasCycle(graph1) && CalculateTotalTime(res_schedule) < schedule.get_TotalTime()) {
                res_schedule.set_schedule_id(schedule.get_schedule_id()+1);
                res_schedule.set_machine_count(schedule.get_machine_count());
                res_schedule.set_schedule_items(schedule_items);
                std::vector<std::vector<int>> graph2 = ScheduleItemsToGraph(res_schedule, schedule_items, jobs, jobList, true);
                // std::cout <<"\n====exchange " << job_id << "-" << process_id<<" with previous process" << std::endl;
                temp_process = process;
                flag = true;
                break;
            }
            schedule_items = schedule.get_schedule_items();
        }
    }
    if (temp_process != "")
        processlist.erase(std::remove(processlist.begin(), processlist.end(), temp_process), processlist.end());
    return res_schedule;
}

// 交换评分函数，根据当前调度方案中工序编号和位置编号的关系，计算优先级，返回工序列表
std::vector<std::string> ScoreProcessForExchange(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList) {
    std::vector<std::string> res_processes;
    // 工序编号越小，但调度位置较为靠后的工序，影响程度越大
    // 优先选择工序编号小的工序，工序编号相同时，选择调度位置较为靠后的工序，返回工序名称，使用algorithm库中的sort函数进行排序
    std::map<std::string, int> temp_list;
    for (const auto &job: jobList) {
        for (int i = 0; i<jobs.size(); i++) {
            if (job == jobs[i].get_job_name()) {
                for (int j = 0; j<jobs[i].get_process_count(); j++) {
                    const int item_index = GetItemIndex(schedule, i, j);
                     if (item_index != -1) {
                        temp_list[jobs[i].get_job_name() + "-" + std::to_string(j)] = item_index;
                    }
                }
            }
        }
    }
    std::vector<std::pair<std::string, int>> temp_vec(temp_list.begin(), temp_list.end());
    // 按照工序编号从小到大排序，工序编号相同时，按照调度位置从大到小排序，工序编号从key中截取-后的数字进行比较
    std::sort(temp_vec.begin(), temp_vec.end(), [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b) {
        return std::stoi(a.first.substr(a.first.find('-')+1)) < std::stoi(b.first.substr(b.first.find('-')+1)) ||
               (std::stoi(a.first.substr(a.first.find('-')+1)) == std::stoi(b.first.substr(b.first.find('-')+1)) && a.second > b.second);
    });

    // 将排序后的工序名称存入res_processes中
    for (auto it = temp_list.begin(); it != temp_list.end(); ++it) {
        res_processes.push_back(it->first);
    }
    return res_processes;
}

// 获取调度位置编号
int GetItemIndex(const Schedule &schedule, const int job_id, const int process_id) {
    for (int i =0 ; i<schedule.get_schedule_items().size(); i++) {
        for (int j = 0; j<schedule.get_schedule_items()[i].process_count; j++) {
            if (schedule.get_schedule_items()[i].schedule_process[j].job_id == job_id && schedule.get_schedule_items()[i].schedule_process[j].process_id == process_id) {
                return j;
            }
        }
    }
    return -1;
}

// 获取工序所在的机器和位置编号
void GetMachineIdAndItemIdByProcess(const std::vector<Schedule_item> &schedule_items, const std::string &process,int type, int &x, int &y) {
    for (int i=0;i<schedule_items.size();i++) {
        for (int j=0;j<schedule_items[i].process_count;j++) {
            if (type==1) {
                if (process == "job"+std::to_string(schedule_items[i].schedule_process[j].job_id)+"-"+std::to_string(schedule_items[i].schedule_process[j].process_id)) {
                    x = i;
                    y = j;
                    return;
                }
            } else if (type==2) {
                if (process == std::to_string(schedule_items[i].schedule_process[j].job_id)+"-"+std::to_string(schedule_items[i].schedule_process[j].process_id)) {
                    x = i;
                    y = j;
                    return;
                }
            }

        }
    }
}

// 迁移邻域搜索，基于工序的类型和所在机器的工序数量的差值进行工序迁移
Schedule MoveNeighborSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<std::string> &processlist, bool &flag) {
    Schedule res_schedule = schedule;
    res_schedule.set_schedule_id(-1);

    std::vector<Schedule_item> schedule_items = schedule.get_schedule_items();
    // TODO: 迁移邻域搜索，基于工序的类型和所在机器的工序数量的差值进行工序迁移，返回调整后的调度方案
    // for (int i =0;i< processlist.size();i++) {
    //     std::cout << processlist[i] << " ";
    // }
    // std::cout << std::endl;
    int machine_id = -1, item_id = -1, job_id = 0, process_id = 0;
    std::string temp_process = "";
    for (auto process: processlist) {
        GetMachineIdAndItemIdByProcess(schedule_items, process, 2, machine_id, item_id);
        // std::cout << process << ":" << machine_id << " " << item_id << std::endl;
        if (item_id>0) {
            job_id = schedule_items[machine_id].schedule_process[item_id].job_id;
            process_id = schedule_items[machine_id].schedule_process[item_id].process_id;

            Job_process temp_job_process = SelectJobByJobId(jobs, job_id).get_job_process()[process_id];
            // for (int i =0;i< temp_job_process.machine_count;i++) {
            //     std::cout << temp_job_process.process_item[i].machine_id << " ";
            // }
            // std::cout << std::endl;

            int min_process_count = schedule_items[machine_id].process_count, temp_machine_id = machine_id;
            int machine_id_1 = 0, process_count = 0;
            for (int i =0; i < temp_job_process.machine_count; i++) {
                machine_id_1 = temp_job_process.process_item[i].machine_id;
                process_count = GetProcessCountByMachineId(schedule, machine_id_1);
                if (process_count < min_process_count) {
                    min_process_count = process_count;
                    temp_machine_id = machine_id_1;
                }
            }
            if (machine_id == machine_id_1) continue;
            // 将该工序从原机器上移除，并添加至目标机器的合适位置
            schedule_items[machine_id].schedule_process.erase(schedule_items[machine_id].schedule_process.begin()+item_id);
            schedule_items[machine_id].process_count--;

            int temp_item_i = 0;
            for (int i =0; i< schedule_items[temp_machine_id].process_count; i++) {
                if (schedule_items[temp_machine_id].schedule_process[i].job_id == job_id && schedule_items[temp_machine_id].schedule_process[i].process_id < process_id) {
                    temp_item_i = i+1;
                }
            }

            schedule_items[temp_machine_id].schedule_process.insert(schedule_items[temp_machine_id].schedule_process.begin()+temp_item_i, Schedule_process{job_id, process_id});
            schedule_items[temp_machine_id].process_count++;
            std::vector<std::vector<int>> graph1 = ScheduleItemsToGraph(res_schedule, schedule_items, jobs, jobList, false);
            res_schedule.set_graph(graph1);
            // std::cout << "now graph status: " << hasCycle(graph1) << std::endl;
            // std::cout << "old time: " << schedule.get_TotalTime() << ", new time: " << CalculateTotalTime(res_schedule) << std::endl;
            if (!hasCycle(graph1) && CalculateTotalTime(res_schedule) <= schedule.get_TotalTime()) {
                res_schedule.set_schedule_id(schedule.get_schedule_id()+1);
                res_schedule.set_machine_count(schedule.get_machine_count());
                res_schedule.set_schedule_items(schedule_items);
                std::vector<std::vector<int>> graph2 = ScheduleItemsToGraph(res_schedule, schedule_items, jobs, jobList, true);
                // std::cout <<"\n=====move "<< job_id << "-" << process_id <<" from machine" << machine_id << " to machine" << temp_machine_id << std::endl;
                temp_process = process;
                flag = true;
                break;
            }
            schedule_items = schedule.get_schedule_items();
        }
    }
    if (temp_process != "")
        processlist.erase(std::remove(processlist.begin(), processlist.end(), temp_process), processlist.end());
    return res_schedule;
}

int GetStartTimeByProcess(const Schedule &schedule, const int job_id, const int process_id) {
    for (int i =0; i<schedule.get_processList().size(); i++) {
        if (process_id==0) return 0;
        if (schedule.get_processList()[i] == std::to_string(job_id) + "-" + std::to_string(process_id)) {
            return schedule.get_start_time()[i];
        }
    }
    return 0;
}

// 迁移评分函数，根据当前调度方案中工序的类型和所在机器的工序数量，计算优先级，返回工序列表
std::vector<std::string> ScoreProcessForMove(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList) {
    std::vector<std::string> res_processes;
    std::vector<int> job_id_list;
    std::vector<int> process_id_list;
    for (int  i =0 ;i< jobList.size();i++) {
        auto job = SelectJobByJobName(jobs, jobList[i]);
        for (int j =0; j< job.get_job_process().size();j++) {
            if (job.get_job_process()[j].machine_count>1) {
                job_id_list.push_back(job.get_job_id());
                process_id_list.push_back(job.get_job_process()[j].process_item[0].process_id);
            }
        }
    }
    std::map<std::string, int> temp_list;
    int differ = 0;
    for (int i =0;i< job_id_list.size();i++) {
        differ = GetDifferFromIdealMachine(schedule, jobs, job_id_list[i], process_id_list[i]);
        if (differ != 0) temp_list[std::to_string(job_id_list[i]) + "-" + std::to_string(process_id_list[i])] = differ;
    }
    // 按照工序的deffer从大到小排序
    std::vector<std::pair<std::string, int>> temp_vec(temp_list.begin(), temp_list.end());
    std::sort(temp_vec.begin(), temp_vec.end(), [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b) {
        return a.second > b.second;
    });
    // 将排序后的工序名称存入res_processes中
    for (auto it = temp_list.begin(); it != temp_list.end(); ++it) {
        res_processes.push_back(it->first);
    }
    return res_processes;
}

// 计算可调度工序的各个机器的工序数量，返回最大差值
int GetDifferFromIdealMachine(const Schedule &schedule, const std::vector<Job> &jobs, const int job_id, const int process_id) {
    Job_process temp_job_process = SelectJobByJobId(jobs, job_id).get_job_process()[process_id];
    int min_process_count = INT_MAX;
    int machine_id = 0, process_count = 0, now_process_count = 0;
    for (int i =0; i < temp_job_process.machine_count; i++) {
        machine_id = temp_job_process.process_item[i].machine_id;
        process_count = GetProcessCountByMachineId(schedule, machine_id);
        if (process_count < min_process_count) {
            min_process_count = process_count;
        }
    }
    // 获取当前调度方案下，该工序所在机器的工序数量
    for (int i =0 ; i<schedule.get_machine_count(); i++) {
        for (int j = 0; j<schedule.get_schedule_items()[i].process_count; j++) {
            if (schedule.get_schedule_items()[i].schedule_process[j].job_id == job_id && schedule.get_schedule_items()[i].schedule_process[j].process_id == process_id) {
                now_process_count = schedule.get_schedule_items()[i].process_count;
                break;
            }
        }
    }
    return now_process_count - min_process_count;
}

// 获取机器上的工序数量
int GetProcessCountByMachineId(const Schedule &schedule, const int machine_id) {
    for (int i = 0; i<schedule.get_schedule_items().size(); i++) {
        if (schedule.get_schedule_items()[i].machine_id == machine_id) {
            return schedule.get_schedule_items()[i].process_count;
        }
    }
    return 0;
}

// 根据工件编号，获取工件
Job SelectJobByJobId(const std::vector<Job> &jobs, const int job_id) {
    for (auto job: jobs) {
        if (job.get_job_id() == job_id) {
            return job;
        }
    }
    return Job();
}