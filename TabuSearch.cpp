//
// Created by lpw on 25-4-14.
//

#include "TabuSearch.h"

#include <algorithm>
#include <iostream>

#include "DataProc.h"
#include "GreedySearch.h"
#include "Job.h"
#include "Schedule.h"

Schedule TabuSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, int tabu_list_length, int max_iter_count) {
    std::vector<std::string> tabu_list;
    std::vector<Schedule> scheduleList;
    Schedule best_schedule = schedule;
    int best_total_time = schedule.get_TotalTime();
    Schedule start_schedule = schedule;

    for (int i =0;i<max_iter_count;i++) {
        // std::cout << "start_schedule_total_time: " << start_schedule.get_TotalTime() << std::endl;
        std::vector<Schedule> temp_schedules = MoveProcessSearch(start_schedule, jobs, jobList);
        scheduleList.insert(scheduleList.end(), temp_schedules.begin(), temp_schedules.end());
        temp_schedules = ExchangeProcessSearch(start_schedule, jobs, jobList);
        scheduleList.insert(scheduleList.end(), temp_schedules.begin(), temp_schedules.end());

        Schedule min_schedule = scheduleList[0];
        int min_total_time = scheduleList[0].get_TotalTime();
        for (auto temp_schedule: scheduleList) {
            if (temp_schedule.get_TotalTime() < min_total_time) {
                min_total_time = temp_schedule.get_TotalTime();
                min_schedule = temp_schedule;
            }
        }
        // std::cout << "1------min_total_time: " << min_total_time << "   " << "best_total_time: " << best_total_time << std::endl;
        if (min_total_time < best_total_time) {
            best_total_time = min_total_time;
            best_schedule = min_schedule;
        } else {
            min_schedule = scheduleList[0];
            min_total_time = scheduleList[0].get_TotalTime();
            for (auto temp_schedule: scheduleList) {
                if (!IsInTabuList(temp_schedule, tabu_list) && temp_schedule.get_TotalTime() < min_total_time) {
                    min_total_time = temp_schedule.get_TotalTime();
                    min_schedule = temp_schedule;
                }
            }
            // std::cout << "2------min_total_time: " << min_total_time << "   " << "best_total_time: " << best_total_time << std::endl;
            best_total_time = min_total_time;
            best_schedule = min_schedule;
        }
        AddScheduleToTabuList(start_schedule, tabu_list, tabu_list_length);
        start_schedule = best_schedule;
        // std::cout << "iter_count: " << i << ", best_total_time: " << best_total_time << std::endl;
    }
    return best_schedule;
}

std::vector<Schedule> MoveProcessSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList) {
    std::vector<Schedule> scheduleList;
    Schedule res_schedule = schedule;
    res_schedule.set_schedule_id(-1);
    std::vector<std::string> key_process = schedule.GetKeyProcess();
    const std::vector<Schedule_item> schedule_items = schedule.get_schedule_items();
    std::vector<std::string> processList = schedule.get_processList();
    std::vector<std::vector<int>> graph = schedule.get_graph();

    int machine_id = -1, item_id = -1, temp_item_i = 0;
    for (int k =1; k< key_process.size() -1; k++) {
        // std::cout << "now:"<<key_process[k] << std::endl;
        std::string process = key_process[k];
        int job_id = std::stoi(process.substr(0, process.find('-')));
        int process_id = std::stoi(process.substr(process.find('-')+1));
        GetMachineIdAndItemIdByProcess(schedule_items, process, 2, machine_id, item_id);
        Job_process temp_job_process = SelectJobByJobId(jobs, job_id).get_job_process()[process_id];

        for (auto process_item: temp_job_process.process_item) {
            if (process_item.machine_id != machine_id) {
                int temp_machine_i = process_item.machine_id;
                std::vector<Schedule_item> temp_schedule_items = schedule_items;
                temp_schedule_items[machine_id].schedule_process.erase(temp_schedule_items[machine_id].schedule_process.begin()+item_id);
                temp_schedule_items[machine_id].process_count--;

                temp_item_i = 0;
                for (int i =0; i< temp_schedule_items[temp_machine_i].process_count; i++) {
                    if (temp_schedule_items[temp_machine_i].schedule_process[i].job_id == job_id && temp_schedule_items[temp_machine_i].schedule_process[i].process_id < process_id) {
                        temp_item_i = i+1;
                    }
                }
                temp_schedule_items[temp_machine_i].schedule_process.insert(temp_schedule_items[temp_machine_i].schedule_process.begin()+temp_item_i, Schedule_process{job_id, process_id});
                temp_schedule_items[temp_machine_i].process_count++;
                // std::cout << "move process: " << process << " from machine" << machine_id << " to machine" << temp_machine_i << std::endl;
                std::vector<std::vector<int>> graph1 = ScheduleItemsToGraph(res_schedule, temp_schedule_items, jobs, jobList, false);
                res_schedule.set_graph(graph1);
                if (!hasCycle(graph1)) {
                    res_schedule.set_schedule_id(schedule.get_schedule_id()+1);
                    res_schedule.set_machine_count(schedule.get_machine_count());
                    res_schedule.set_schedule_items(temp_schedule_items);
                    std::vector<std::vector<int>> graph2 = ScheduleItemsToGraph(res_schedule, temp_schedule_items, jobs, jobList, true);
                    scheduleList.push_back(res_schedule);
                }
            }
        }
    }
    return scheduleList;
}

std::vector<Schedule> ExchangeProcessSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList) {
    std::vector<Schedule> scheduleList;
    Schedule res_schedule = schedule;
    res_schedule.set_schedule_id(-1);
    std::vector<std::string> key_process = schedule.GetKeyProcess();
    std::vector<Schedule_item> schedule_items = schedule.get_schedule_items();
    std::vector<std::string> processList = schedule.get_processList();
    std::vector<std::vector<int>> graph = schedule.get_graph();

    int machine_id = -1, item_id = -1;
    for (int k =1; k< key_process.size() -1; k++) {
        // std::cout << "now:"<<key_process[k] << std::endl;
        std::string process = key_process[k];
        int job_id = std::stoi(process.substr(0, process.find('-')));
        int process_id = std::stoi(process.substr(process.find('-')+1));
        GetMachineIdAndItemIdByProcess(schedule_items, process, 2, machine_id, item_id);
        Job_process temp_job_process = SelectJobByJobId(jobs, job_id).get_job_process()[process_id];

        std::vector<Schedule_item> temp_schedule_items = schedule_items;
        int temp_item = item_id;
        for (int i = item_id-1; i>=0; i--) {
            if (temp_schedule_items[machine_id].schedule_process[i].job_id == job_id) {
                temp_item = i;
                break;

            }
        }
        if (temp_item != item_id) {
            temp_schedule_items[machine_id].schedule_process.erase(temp_schedule_items[machine_id].schedule_process.begin()+item_id);
            temp_schedule_items[machine_id].schedule_process.insert(temp_schedule_items[machine_id].schedule_process.begin()+temp_item, Schedule_process{job_id, process_id});
        }
        // std::cout << "exchange process: " << process << " with previous process" << std::endl;
        std::vector<std::vector<int>> graph1 = ScheduleItemsToGraph(res_schedule, temp_schedule_items, jobs, jobList, false);
        res_schedule.set_graph(graph1);
        if (!hasCycle(graph1)) {
            res_schedule.set_schedule_id(schedule.get_schedule_id()+1);
            res_schedule.set_machine_count(schedule.get_machine_count());
            res_schedule.set_schedule_items(temp_schedule_items);
            std::vector<std::vector<int>> graph2 = ScheduleItemsToGraph(res_schedule, temp_schedule_items, jobs, jobList, true);
            scheduleList.push_back(res_schedule);
        }
    }
    return scheduleList;
}

std::string ScheduleToString(const Schedule &schedule) {
    std::string res;
    for (auto schedule_item: schedule.get_schedule_items()) {
        res += std::to_string(schedule_item.machine_id) + ":";
        for (const auto process: schedule_item.schedule_process) {
            res += std::to_string(process.job_id) + "-" + std::to_string(process.process_id) + ",";
        }
    }
    return res;
}

bool IsInTabuList(const Schedule &schedule, const std::vector<std::string> &tabu_list) {
    const std::string schedule_str = ScheduleToString(schedule);
    return std::find(tabu_list.begin(), tabu_list.end(), schedule_str) != tabu_list.end();
}

void AddScheduleToTabuList(const Schedule &schedule, std::vector<std::string> &tabu_list, const int tabu_list_length) {
    const std::string schedule_str = ScheduleToString(schedule);
    if (tabu_list.size() >= tabu_list_length) {
        tabu_list.erase(tabu_list.begin());
    }
    tabu_list.push_back(schedule_str);
}
