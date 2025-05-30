//
// Created by 28898 on 25-1-3.
//

#include "RandomSearch.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <set>

#include "DataProc.h"
#include "TabuSearch.h"
#include <climits>


Schedule RandomSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, const double strategy_param, const int repeat_count, const int max_repeat_count, const int max_iter_count, const int tabu_list_length, const int population_size) {
    // ReSharper disable once CppTooWideScope
    std::vector<Schedule> scheduleList;
    Schedule best_schedule = schedule;
    const double random_num = static_cast<double>(std::rand()) / RAND_MAX;
    // if (repeat_count<max_repeat_count/2 || random_num < strategy_param) {
    //     best_schedule = AggressiveSearch(schedule, jobs, machines, jobList, population_size, max_iter_count);
    // } else {
    //     best_schedule = ConservativeSearch(best_schedule, jobs, jobList, tabu_list_length, max_iter_count);
    // }
    // best_schedule = AggressiveSearch(schedule, jobs, machines, jobList, population_size, max_iter_count);
    best_schedule = ConservativeSearch(best_schedule, jobs, jobList, tabu_list_length, max_iter_count);
    return best_schedule;
}

Schedule ConservativeSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, int tabu_list_length, int max_iter_count) {
    Schedule res_schedule = schedule;
    // TODO: ConservativeSearch, 保守搜索，使用禁忌搜索算法，对调度方案进行调整，返回调整后的调度方案列表
    res_schedule = TabuSearch(schedule, jobs, jobList, tabu_list_length, max_iter_count);
    return res_schedule;
}

Schedule AggressiveSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, int population_size, const int max_iter_count) {
    // TODO: AggressiveSearch, 激进搜索，使用遗传算法对调度方案进行调整，返回调整后的调度方案
    double cross_rate = 0.8;
    double variation_rate = 0.3;
    double cross_machine_rate = 0.5;
    double variation_machine_rate = 0.95;

    int optimal_fitness = INT_MAX;
    std::vector<int> optimal_machine_selection_code;
    std::vector<int> optimal_operation_sequencing_code;

    std::vector<std::vector<int>> population_machine_selection_code;  // 种群的机器选择编码
    std::vector<std::vector<int>> population_operation_sequencing_code;  // 种群的工序排序编码
    std::vector<std::vector<int>> temp_population_machine_selection_code;  // 临时种群的机器选择编码
    std::vector<std::vector<int>> temp_population_operation_sequencing_code;  // 临时种群的工序排序编码
    std::vector<int> temp_machine1, temp_machine2;
    std::vector<int> temp_operation1, temp_operation2;
    std::vector<int> fitness_list;

    EncodeSchedule(schedule, jobs, jobList, optimal_machine_selection_code, optimal_operation_sequencing_code);
    optimal_fitness = schedule.get_TotalTime();

    // 种群初始化
    GlobalInitializePopulation(jobs, machines, jobList, population_size*0.6, optimal_operation_sequencing_code, temp_population_machine_selection_code, temp_population_operation_sequencing_code);
    population_machine_selection_code.insert(population_machine_selection_code.end(), temp_population_machine_selection_code.begin(), temp_population_machine_selection_code.end());
    population_operation_sequencing_code.insert(population_operation_sequencing_code.end(), temp_population_operation_sequencing_code.begin(), temp_population_operation_sequencing_code.end());
    LocalInitializePopulation(jobs, machines, jobList, population_size*0.2, optimal_operation_sequencing_code, temp_population_machine_selection_code, temp_population_operation_sequencing_code);
    population_machine_selection_code.insert(population_machine_selection_code.end(), temp_population_machine_selection_code.begin(), temp_population_machine_selection_code.end());
    population_operation_sequencing_code.insert(population_operation_sequencing_code.end(), temp_population_operation_sequencing_code.begin(), temp_population_operation_sequencing_code.end());
    RandomInitializePopulation(jobs, machines, jobList, population_size*0.2, optimal_operation_sequencing_code, temp_population_machine_selection_code, temp_population_operation_sequencing_code);
    population_machine_selection_code.insert(population_machine_selection_code.end(), temp_population_machine_selection_code.begin(), temp_population_machine_selection_code.end());
    population_operation_sequencing_code.insert(population_operation_sequencing_code.end(), temp_population_operation_sequencing_code.begin(), temp_population_operation_sequencing_code.end());

    double random_num = 0;
    int population_count = population_machine_selection_code.size();
    std::vector<std::vector<int>> after_machine_selection_code;  // 种群的机器选择编码
    std::vector<std::vector<int>> after_operation_sequencing_code;  // 种群的工序排序编码
    for (int k = 0; k < max_iter_count;k++) {
        for (int i =0 ;i<population_count;i++) {
            after_machine_selection_code.clear();
            after_operation_sequencing_code.clear();
            fitness_list.clear();
            random_num = static_cast<double>(std::rand()) / RAND_MAX;
            if (random_num < cross_rate) {
                int parent1 = std::rand() % population_machine_selection_code.size();
                random_num = static_cast<double>(std::rand()) / RAND_MAX;
                if (random_num < cross_machine_rate) {
                    MachineCross(population_machine_selection_code[i], population_machine_selection_code[parent1], temp_machine1, temp_machine2);
                    after_machine_selection_code.push_back(temp_machine1);
                    after_machine_selection_code.push_back(temp_machine2);
                    after_operation_sequencing_code.push_back(population_operation_sequencing_code[i]);
                    after_operation_sequencing_code.push_back(population_operation_sequencing_code[i]);
                    // std::cout << "MachineCross" << std::endl;
                } else {
                    OperationCross(population_operation_sequencing_code[i], population_operation_sequencing_code[parent1], temp_operation1, temp_operation2);
                    after_machine_selection_code.push_back(population_machine_selection_code[i]);
                    after_machine_selection_code.push_back(population_machine_selection_code[i]);
                    after_operation_sequencing_code.push_back(temp_operation1);
                    after_operation_sequencing_code.push_back(temp_operation2);
                    // std::cout << "OperationCross" << std::endl;
                }
                after_machine_selection_code.push_back(population_machine_selection_code[i]);
                after_operation_sequencing_code.push_back(population_operation_sequencing_code[i]);
            }
            random_num = static_cast<double>(std::rand()) / RAND_MAX;
            if (random_num < variation_rate) {
                random_num = static_cast<double>(std::rand()) / RAND_MAX;
                if (random_num < variation_machine_rate) {
                    MachineVariation(population_machine_selection_code[i], jobs, jobList, temp_machine1);
                    after_machine_selection_code.push_back(temp_machine1);
                    after_operation_sequencing_code.push_back(population_operation_sequencing_code[i]);
                    // std::cout << "MachineVariation" << std::endl;
                } else {
                    OperationVariation(population_operation_sequencing_code[i], temp_operation1);
                    after_machine_selection_code.push_back(population_machine_selection_code[i]);
                    after_operation_sequencing_code.push_back(temp_operation1);
                    // std::cout << "OperationVariation" << std::endl;
                }
                after_machine_selection_code.push_back(population_machine_selection_code[i]);
                after_operation_sequencing_code.push_back(population_operation_sequencing_code[i]);
            }
            if (after_machine_selection_code.size()!=0) {
                for (int j =0;j<after_machine_selection_code.size();j++) {
                    fitness_list.push_back(CalculateFitness(after_machine_selection_code[j], after_operation_sequencing_code[j], jobs, jobList, schedule));
                }
                int min_fitness_index = 0;
                int min_fitness = fitness_list[0];
                for (int j = 1; j<fitness_list.size();j++) {
                    if (fitness_list[j] < min_fitness) {
                        min_fitness = fitness_list[j];
                        min_fitness_index = j;
                    }
                }
                population_machine_selection_code[i] = after_machine_selection_code[min_fitness_index];
                population_operation_sequencing_code[i] = after_operation_sequencing_code[min_fitness_index];
            }
        }
        // std::cout << "finished " << k << "th iteration" << std::endl;
    }

    for (int i =0 ;i<population_machine_selection_code.size();i++) {
        if (fitness_list[i] < optimal_fitness) {
            optimal_fitness = fitness_list[i];
            optimal_machine_selection_code = population_machine_selection_code[i];
            optimal_operation_sequencing_code = population_operation_sequencing_code[i];
        }
    }
    return DecodeSchedule(optimal_machine_selection_code, optimal_operation_sequencing_code, jobs, jobList, schedule);
}

// 将调度方案编码为机器选择编码和工序排序编码
void EncodeSchedule(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<int> &machine_selection_code, std::vector<int> &operation_sequencing_code) {
    for (std::string jobName: jobList) {
        // 获取当前工件的信息
        Job job;
        for (Job jobItem: jobs) {
            if (jobItem.get_job_name() == jobName) {
                job = jobItem;
                break;
            }
        }
        // 根据工件的工序信息，生成编码
        for (auto job_process: job.get_job_process()) {
            //获取当前工序所在的机器的编号
            int temp_machine_id = GetMachineIdByJobIdAndProcessId(schedule ,job.get_job_id(), job_process.process_item[0].process_id);
            for (int i =0;i<job_process.machine_count;i++) {
                if (job_process.process_item[i].machine_id == temp_machine_id) {
                    machine_selection_code.push_back(i);
                    break;
                }
            }
        }
    }
    for (int i = 1;i<schedule.get_processList().size()-1;i++) {

        operation_sequencing_code.push_back(std::stoi(schedule.get_processList()[i].substr(0, schedule.get_processList()[i].find('-'))));
    }
}

// 将机器选择编码和工序排序编码解码为调度方案
Schedule DecodeSchedule(const std::vector<int> &machine_selection_code, const std::vector<int> &operation_sequencing_code, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, const Schedule &schedule) {
    std::vector<std::vector<int>> machine_matrix;
    std::vector<int> temp_machine_selection_code = machine_selection_code;
    std::map<int, int> now_process_id;
    for (int i = 0; i<jobs.size();i++) {
        machine_matrix.push_back(std::vector<int>(jobs[i].get_process_count(), -1));
        now_process_id[i] = 0;
    }
    for (std::string jobName: jobList) {
        // 获取当前工件的信息
        Job job;
        for (Job jobItem: jobs) {
            if (jobItem.get_job_name() == jobName) {
                job = jobItem;
                break;
            }
        }
        for (auto job_process: job.get_job_process()) {
            const int temp_process_id = job_process.process_item[0].process_id;
            const int machine_selected = temp_machine_selection_code[0];
            temp_machine_selection_code.erase(temp_machine_selection_code.begin());  // 删除已经使用的机器选择编码，即删除第一个元素
            machine_matrix[job.get_job_id()][temp_process_id] = job_process.process_item[machine_selected].machine_id;
        }
    }
    // for (int i =0;i<machine_selection_code.size(); i++) {
    //     std::cout << machine_selection_code[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i =0;i<operation_sequencing_code.size(); i++) {
    //     std::cout << operation_sequencing_code[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i =0;i< machine_matrix.size();i++) {
    //     for (int j =0;j<machine_matrix[i].size();j++) {
    //         if (machine_matrix[i][j] != -1) {
    //             std::cout << machine_matrix[i][j] <<" ";
    //         }
    //     }
    //     std::cout << std::endl;
    // }
    Schedule res_schedule;
    res_schedule.set_schedule_id(schedule.get_schedule_id()+1);
    res_schedule.set_machine_count(schedule.get_machine_count());
    std::vector<Schedule_item> temp_schedule_items;
    for (int i = 0; i<schedule.get_machine_count(); i++) {
        Schedule_item temp_schedule_item;
        temp_schedule_item.machine_id = i;
        temp_schedule_item.process_count = 0;
        temp_schedule_item.schedule_process.clear();
        temp_schedule_items.push_back(temp_schedule_item);
    }
    for (int i = 0; i<operation_sequencing_code.size(); i++) {
        int temp_job_id = operation_sequencing_code[i];
        int temp_process_id = now_process_id[temp_job_id];
        int temp_machine_id = machine_matrix[temp_job_id][temp_process_id];
        temp_schedule_items[temp_machine_id].schedule_process.push_back({temp_job_id, temp_process_id});
        temp_schedule_items[temp_machine_id].process_count++;
        now_process_id[temp_job_id]++;
    }
    res_schedule.set_schedule_items(temp_schedule_items);
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(res_schedule, temp_schedule_items, jobs, jobList, true);
    return res_schedule;
}

// 根据工序获取机器编号
int GetMachineIdByJobIdAndProcessId(const Schedule &schedule, const int job_id, const int process_id) {
    for (int i = 0; i < schedule.get_machine_count(); i++) {
        for (int j = 0; j < schedule.get_schedule_items()[i].process_count; j++) {
            if (schedule.get_schedule_items()[i].schedule_process[j].job_id == job_id &&
                schedule.get_schedule_items()[i].schedule_process[j].process_id == process_id) {
                return i;
            }
        }
    }
    return 0;
}

// 全局初始化种群
void GlobalInitializePopulation(const std::vector<Job> &jobs, const std::vector<Machine> &machines,const std::vector<std::string> &jobList, const int population_size, const std::vector<int> &operation_sequencing_code,
    std::vector<std::vector<int>> &population_machine_selection_code, std::vector<std::vector<int>> &population_operation_sequencing_code) {

    population_machine_selection_code.clear();
    population_operation_sequencing_code.clear();
    std::map<std::string, std::vector<int>> temp_machine_selection_code_map;

    for (int k=0;k<population_size;k++) {
        temp_machine_selection_code_map.clear();
        std::vector<int> temp_machine_selection_code;
        auto machine_time = std::vector<int>(machines.size(), 0);
        std::vector<std::string> temp_jobList = jobList;
        int temp_job_id = 0;
        while (temp_jobList.size()!=0) {
            temp_job_id = std::rand() % temp_jobList.size();  // 随机选择一个工件
            Job temp_job;
            for (Job jobItem: jobs) {
                if (jobItem.get_job_name() == temp_jobList[temp_job_id]) {
                    temp_job = jobItem;
                    break;
                }
            }
            for (const Job_process job_process: temp_job.get_job_process()) {
                auto operation_time = std::vector<int>(job_process.machine_count, 0);
                for (int i = 0; i < job_process.machine_count; i++) {
                    operation_time[i] = job_process.process_item[i].process_time + machine_time[job_process.process_item[i].machine_id];
                }
                int min_time = operation_time[0];
                int min_machine = 0;
                for (int i = 1; i < operation_time.size(); i++) {
                    if (operation_time[i] < min_time) {
                        min_time = operation_time[i];
                        min_machine = i;
                    }
                }
                machine_time[job_process.process_item[min_machine].machine_id] = min_time;
                if (temp_machine_selection_code_map.find(temp_jobList[temp_job_id]) == temp_machine_selection_code_map.end()) {
                    temp_machine_selection_code_map[temp_jobList[temp_job_id]] = std::vector<int>();
                }
                temp_machine_selection_code_map[temp_jobList[temp_job_id]].push_back(min_machine);
            }
            temp_jobList.erase(temp_jobList.begin() + temp_job_id);  // 删除已经使用的工件
        }
        std::vector<int> temp_operation_sequencing_code = operation_sequencing_code;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(temp_operation_sequencing_code.begin(), temp_operation_sequencing_code.end(), gen);

        for (int i =0; i<jobList.size();i++) {
            std::string temp_job_name = jobList[i];
            if (temp_machine_selection_code_map.find(temp_job_name) != temp_machine_selection_code_map.end()) {
                for (int j =0;j<temp_machine_selection_code_map[temp_job_name].size();j++) {
                    temp_machine_selection_code.push_back(temp_machine_selection_code_map[temp_job_name][j]);
                }
            }
        }
        population_machine_selection_code.push_back(temp_machine_selection_code);
        population_operation_sequencing_code.push_back(temp_operation_sequencing_code);
    }
}

// 本地初始化种群
void LocalInitializePopulation(const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, const int population_size, const std::vector<int> &operation_sequencing_code,
    std::vector<std::vector<int>> &population_machine_selection_code, std::vector<std::vector<int>> &population_operation_sequencing_code) {
    population_machine_selection_code.clear();
    population_operation_sequencing_code.clear();

    for (int k = 0; k < population_size; k++) {
        std::vector<int> temp_machine_selection_code;
        auto machine_time = std::vector<int>(machines.size(), 0);
        std::vector<std::string> temp_jobList = jobList;
        int temp_job_id = 0;
        while (temp_jobList.size() != 0) {
            Job temp_job;
            for (Job jobItem: jobs) {
                if (jobItem.get_job_name() == temp_jobList[0]) {
                    temp_job = jobItem;
                    break;
                }
            }
            for (const Job_process job_process: temp_job.get_job_process()) {
                auto operation_time = std::vector<int>(job_process.machine_count, 0);
                for (int i = 0; i < job_process.machine_count; i++) {
                    operation_time[i] = job_process.process_item[i].process_time + machine_time[job_process.process_item[i].machine_id];
                }
                int min_time = operation_time[0];
                int min_machine = 0;
                for (int i = 1; i < operation_time.size(); i++) {
                    if (operation_time[i] < min_time) {
                        min_time = operation_time[i];
                        min_machine = i;
                    }
                }
                machine_time[job_process.process_item[min_machine].machine_id] = min_time;
                temp_machine_selection_code.push_back(min_machine);
            }
            machine_time = std::vector<int>(machines.size(), 0);  // 重置机器时间
            temp_jobList.erase(temp_jobList.begin() + temp_job_id);  // 删除已经使用的工件
        }
        std::vector<int> temp_operation_sequencing_code = operation_sequencing_code;
        std::random_device rd;
        std::default_random_engine rng(rd());
        std::shuffle(temp_operation_sequencing_code.begin(), temp_operation_sequencing_code.end(), rng);

        population_machine_selection_code.push_back(temp_machine_selection_code);
        population_operation_sequencing_code.push_back(temp_operation_sequencing_code);
    }
}

// 随机初始化种群
void RandomInitializePopulation(const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, const int population_size, const std::vector<int> &operation_sequencing_code,
    std::vector<std::vector<int>> &population_machine_selection_code, std::vector<std::vector<int>> &population_operation_sequencing_code) {
    population_machine_selection_code.clear();
    population_operation_sequencing_code.clear();

    for (int k = 0; k < population_size; k++) {
        std::vector<int> temp_machine_selection_code;
        for (std::string jobName: jobList) {
            Job job;
            for (Job jobItem: jobs) {
                if (jobItem.get_job_name() == jobName) {
                    job = jobItem;
                    break;
                }
            }
            for (const auto job_process: job.get_job_process()) {
                const int temp_machine_id = std::rand() % job_process.machine_count;
                temp_machine_selection_code.push_back(temp_machine_id);
            }
        }
        std::vector<int> temp_operation_sequencing_code = operation_sequencing_code;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(temp_operation_sequencing_code.begin(), temp_operation_sequencing_code.end(), gen);

        population_machine_selection_code.push_back(temp_machine_selection_code);
        population_operation_sequencing_code.push_back(temp_operation_sequencing_code);
    }
}

// 机器选择编码交叉
void MachineCross(const std::vector<int> &parent1, const std::vector<int> &parent2, std::vector<int> &child1, std::vector<int> &child2) {

    child1.clear();
    child2.clear();
    const int parent_count = parent1.size();
    const int r = std::rand() % parent_count;
    std::vector<int> cross_point;  // 交叉点,生成r个互不相等的随机数
    for (int i = 0; i < r; i++) {
        int temp = std::rand() % parent_count;
        while (std::find(cross_point.begin(), cross_point.end(), temp) != cross_point.end()) {
            temp = std::rand() % parent_count;
        }
        cross_point.push_back(temp);
    }
    for (int i = 0; i < parent_count; i++) {
        if (std::find(cross_point.begin(), cross_point.end(), i) != cross_point.end()) {
            child1.push_back(parent1[i]);
            child2.push_back(parent2[i]);
        }
        else {
            child1.push_back(parent2[i]);
            child2.push_back(parent1[i]);
        }
    }
}

void OperationCross(const std::vector<int> &parent1, const std::vector<int> &parent2, std::vector<int> &child1, std::vector<int> &child2) {
    const int parent_count = parent1.size();
    child1.clear();
    child2.clear();
    std::vector<int> job_id_list;
    for (int i = 0; i < parent_count; i++) {
        if (job_id_list.size() == 0) {
            job_id_list.push_back(parent1[i]);
        }
        else {
            if (std::find(job_id_list.begin(), job_id_list.end(), parent1[i]) == job_id_list.end()) {
                job_id_list.push_back(parent1[i]);
            }
        }
    }
    std::set<int> job_set1;
    int random_num = 0;
    for (int i = 0; i < job_id_list.size(); i++) {
        random_num = std::rand() % 2;
        if (random_num == 0) {
            job_set1.insert(job_id_list[i]);
        }
    }
    int index =0;
    for (int i =0; i< parent1.size();i++) {
        if (job_set1.find(parent1[i]) != job_set1.end()) {
            child1.push_back(parent1[i]);
        }
        else {
            for (int k=index; k<parent2.size(); k++) {
                if (job_set1.find(parent2[k]) == job_set1.end()) {
                    child1.push_back(parent2[k]);
                    index = k+1;
                    break;
                }
            }
        }
    }
    index = 0;
    for (int i = 0; i < parent2.size(); i++) {
        if (job_set1.find(parent2[i]) == job_set1.end()) {
            child2.push_back(parent2[i]);
        }
        else {
            for (int k = index; k < parent1.size(); k++) {
                if (job_set1.find(parent1[k]) != job_set1.end()) {
                    child2.push_back(parent1[k]);
                    index = k + 1;
                    break;
                }
            }
        }
    }
}

void MachineVariation(const std::vector<int> &parent, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<int> &child) {
    child = parent;
    const int parent_count = parent.size();
    const int r = std::rand() % parent_count;
    std::vector<int> variation_point;  // 变异点,生成r个互不相等的随机数
    for (int i = 0; i < r; i++) {
        int temp = std::rand() % parent_count;
        while (std::find(variation_point.begin(), variation_point.end(), temp) != variation_point.end()) {
            temp = std::rand() % parent_count;
        }
        variation_point.push_back(temp);
    }
    int now_index = -1;
    for (std::string jobName: jobList) {
        Job job;
        for (Job jobItem: jobs) {
            if (jobItem.get_job_name() == jobName) {
                job = jobItem;
                break;
            }
        }
        for (const auto job_process: job.get_job_process()) {
            now_index++;
            if (std::find(variation_point.begin(), variation_point.end(), now_index) != variation_point.end()) {
                int min_machine = 0;
                int min_time = job_process.process_item[0].process_time;
                for (int i = 1; i < job_process.machine_count; i++) {
                    if (job_process.process_item[i].process_time < min_time) {
                        min_time = job_process.process_item[i].process_time;
                        min_machine = i;
                    }
                }
                child[now_index] = min_machine;
            }
        }
    }
}

void OperationVariation(const std::vector<int> &parent, std::vector<int> &child) {
    child = parent;
    std::random_device rd;
    std::default_random_engine rng(rd());
    std::shuffle(child.begin(), child.end(), rng);
}

int CalculateFitness(const std::vector<int> &machine_selection_code, const std::vector<int> &operation_sequencing_code, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, const Schedule &schedule0) {
    Schedule schedule = DecodeSchedule(machine_selection_code, operation_sequencing_code, jobs, jobList, schedule0);
    return CalculateTotalTime(schedule);
}

