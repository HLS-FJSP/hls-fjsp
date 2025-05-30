//
// Created by luopw on 24-12-1.
//

#ifndef DATAPROC_H
#define DATAPROC_H
#include <vector>

#include "Job.h"
#include "Machine.h"
#include "Order.h"
#include "Schedule.h"


// 从文件中读取数据，初始化机器、工件、订单
void Init(std::vector<Machine> &machines, std::vector<Job> &jobs, std::vector<Order> &orders, std::string file_path);

// 将订单转换为待加工的工件列表
void OrderToJobList(const std::vector<Order> &orders, std::vector<std::string> &jobList);

// 生成初始解，按照fifo策略依次分配工件到机器，暂不考虑工件加工的时间开销
Schedule GenerateInitialSolution(const std::vector<std::string> &jobList, const std::vector<Job> &jobs, std::vector<Machine> &machines);

// 根据工件名称查找工件
Job SelectJobByJobName(const std::vector<Job> &jobs, const std::string &jobName);

// 根据机器id, 工件id, 工序id查询时间开销
int GetProcessTime(const std::vector<Job> &jobs, const int machine_id, const int job_id, const int process_id);

// 将调度方案转换为邻接矩阵
std::vector<std::vector<int>> ScheduleItemsToGraph(Schedule &schedule, std::vector<Schedule_item> &schedule_items, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, bool flag);

// 计算调度方案的总时间开销
int CalculateTotalTime(Schedule &schedule);

// 设置停止标志，当搜索时间超过指定时间或搜索次数超过指定次数时，停止搜索
bool CheckStopFlag(const int &search_time, const int &count, int repeat_count, int max_repeat_count);

// std::vector<std::vector<std::string>> ScheduleToProcessList(const Schedule &schedule);
#endif //DATAPROC_H
