//
// Created by luopw on 24-12-1.
//

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <string>
#include <vector>

struct Schedule_process {
    int job_id;  // job id,工件编号
    int process_id; // process id,工序编号
};

struct Schedule_item {
    int machine_id;  // machine id,机器编号
    int process_count;  // process count,工序数量
    std::vector<Schedule_process> schedule_process;  // schedule process,调度工序
};

class Schedule {
    int schedule_id;  // schedule id,调度编号
    int machine_count;  // machine count,机器数量
    std::vector<Schedule_item> schedule_items;  // schedule items,调度项
    std::vector<std::string> processList;  // process list,工序列表
    std::vector<std::vector<int>> graph; // graph,邻接矩阵
    int TotalTime; // process time,加工时间
    std::vector<int> start_time;  // start time,开始时间

public:
    Schedule();
    int get_schedule_id() const;
    int get_machine_count() const;
    std::vector<Schedule_item> get_schedule_items() const;
    std::vector<std::string> get_processList() const;
    std::vector<std::vector<int>> get_graph() const;
    int get_TotalTime() const;
    std::vector<int> get_start_time() const;
    void set_schedule_id(int schedule_id);
    void set_machine_count(int machine_count);
    void set_schedule_items(const std::vector<Schedule_item> &schedule_items);
    void set_processList(const std::vector<std::string> &processList);
    void set_graph(const std::vector<std::vector<int>> &graph);
    void set_TotalTime(int ProcessTime);
    void set_start_time(const std::vector<int> &start_time);
    void to_string() const;

    void AddProcess(int machine_id, int job_id, int process_id);
    void ToMapCode() const;
    std::vector<std::string> GetKeyProcess() const;
};



#endif //SCHEDULE_H
