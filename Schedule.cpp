//
// Created by luopw on 24-12-1.
//

#include "Schedule.h"
#include <iostream>
#include <queue>
#include <climits>


Schedule::Schedule() {
    schedule_id = 0;
    machine_count = 0;
    schedule_items.clear();
    graph.clear();
    TotalTime = 0;
}

int Schedule::get_schedule_id() const {return schedule_id;}
int Schedule::get_machine_count() const {return machine_count;}
std::vector<Schedule_item> Schedule::get_schedule_items() const {return schedule_items;}
std::vector<std::string> Schedule::get_processList() const {return processList;}
std::vector<std::vector<int>> Schedule::get_graph() const {return graph;}
int Schedule::get_TotalTime() const {return TotalTime;}
std::vector<int> Schedule::get_start_time() const {return start_time;}
void Schedule::set_schedule_id(const int schedule_id) {this->schedule_id = schedule_id;}
void Schedule::set_machine_count(const int machine_count) {this->machine_count = machine_count;}
void Schedule::set_schedule_items(const std::vector<Schedule_item> &schedule_items) {this->schedule_items = schedule_items;}
void Schedule::set_processList(const std::vector<std::string> &processList) {this->processList = processList;}
void Schedule::set_graph(const std::vector<std::vector<int>> &graph) {this->graph = graph;}
void Schedule::set_TotalTime(const int ProcessTime) {this->TotalTime = ProcessTime;}
void Schedule::set_start_time(const std::vector<int> &start_time) {this->start_time = start_time;}
void Schedule::to_string() const {
    std::cout << std::endl;
    for (auto &[machine_id, process_count, schedule_process] : schedule_items) {
        std::cout << "machine_id: " << machine_id << ", process_count: " << process_count << std::endl;
        for (const auto &[job_id, process_id] : schedule_process) {
            std::cout << "[" << job_id << "," << process_id << "] ";
        }
        std::cout << std::endl;
    }
}


void Schedule::AddProcess(const int machine_id, const int job_id, const int process_id) {
    for (auto &[temp_machine_id, process_count, schedule_process] : schedule_items) {
        if (machine_id == temp_machine_id) {
            Schedule_process scheduleProcess;
            scheduleProcess.job_id = job_id;
            scheduleProcess.process_id = process_id;
            schedule_process.push_back(scheduleProcess);
            process_count++;
            return;
        }
    }
}

void Schedule::ToMapCode() const {
    std::cout << "Map code: " << std::endl;
    std::cout << "flowchart LR" << std::endl;
    for (auto &[machine_id, process_count, schedule_process] : schedule_items) {
        std::cout << "start0 --> " << schedule_process[0].job_id<<"_"<<schedule_process[0].process_id << std::endl;
        for ( int i =1 ; i < process_count; i++) {
            std::cout<< schedule_process[i-1].job_id<<"_"<<schedule_process[i-1].process_id << " --> " << schedule_process[i].job_id<<"_"<<schedule_process[i].process_id << std::endl;
        }
        std::cout << schedule_process[process_count-1].job_id<<"_"<<schedule_process[process_count-1].process_id << " --> end0"  << std::endl;
    }
}

std::vector<std::string> Schedule::GetKeyProcess() const {
    std::vector<std::string> key_process;
    const std::vector<std::vector<int>> graph = this->get_graph();
    std::vector<std::string> processList = this->get_processList();
    std::vector<Schedule_item> schedule_items = this->get_schedule_items();

    // 获取关键路径
    const int processCount = graph.size();
    std::vector<int> inDegree(processCount, 0);  // 记录每个节点的入度
    for (int i = 0; i<processCount; i++) {
        for (int j = 0; j<processCount; j++) {
            if (graph[i][j] != -1) {
                inDegree[j]++;
            }
        }
    }
    // 获取拓扑排序的列表
    std::vector<int> toPoSort;
    std::queue<int> zeroInDegreeQueue;
    for (int i = 0; i<processCount; i++) {
        if (inDegree[i] == 0) {
            zeroInDegreeQueue.push(i);
        }
    }
    while (!zeroInDegreeQueue.empty()) {
        int temp = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        toPoSort.push_back(temp);
        for (int i = 0; i<processCount; i++) {
            if (graph[temp][i] != -1) {
                inDegree[i]--;
                if (inDegree[i] == 0) {
                    zeroInDegreeQueue.push(i);
                }
            }
        }
    }
    // 检查是否存在环
    if (toPoSort.size() != processCount) {
        throw std::runtime_error(" get key process: The graph contains a cycle and cannot be topologically sorted.");
    }

    std::vector<int> ve(processCount, 0), vl(processCount, INT_MAX);
    for (int i = 0; i<toPoSort.size(); i++) {
        for (int j = 0; j<processCount; j++) {
            if (graph[j][toPoSort[i]] != -1) {
                ve[toPoSort[i]] = std::max(ve[toPoSort[i]], ve[j] + graph[j][toPoSort[i]]);
            }
        }
    }
    vl[processCount-1] = ve[processCount-1];
    for (int i = toPoSort.size() - 1; i >= 0; i--) {
        for (int j = 0; j < processCount; j++) {
            if (graph[toPoSort[i]][j] != -1) {
                vl[toPoSort[i]] = std::min(vl[toPoSort[i]], vl[j] - graph[toPoSort[i]][j]);
            }
        }
    }
    for (int i =0; i<toPoSort.size(); i++) {
        if (vl[toPoSort[i]] == ve[toPoSort[i]]) {
            key_process.push_back(processList[toPoSort[i]]);
        }
    }
    return key_process;
}