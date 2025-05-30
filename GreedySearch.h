//
// Created by luopw on 24-12-1.
//

#ifndef GREEDYSEARCH_H
#define GREEDYSEARCH_H
#include <vector>

#include "Job.h"
#include "Schedule.h"

// 贪心搜索
Schedule GreedySearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, int iter_count);

// 贪心调整
Schedule GreedyAdjust(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList);

// 判断是否存在环
bool hasCycle(const std::vector<std::vector<int>> &adjMatrix);

// 贪心邻域搜索，基于相同机器上相邻的工序进行最大程度的交换
Schedule ExchangeNeighborSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<std::string> &processlist, bool &flag);

// 交换评分函数，根据当前调度方案中工序编号和位置编号的关系，计算优先级，返回工序列表
std::vector<std::string> ScoreProcessForExchange(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList);

// 获取工序在调度方案中的索引
int GetItemIndex(const Schedule &schedule, const int job_id, const int process_id);

// 根据工序，在调度方案中获取到工序所在的机器编号
void GetMachineIdAndItemIdByProcess(const std::vector<Schedule_item> &schedule_items, const std::string &process, int type, int &x, int &y);

// 迁移邻域搜索，基于工序的类型和所在机器的工序数量的差值进行工序迁移
Schedule MoveNeighborSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<std::string> &processlist, bool &flag);

// 根据工序的编号，获取工序的开始时间
int GetStartTimeByProcess(const Schedule &schedule, const int job_id, const int process_id);

// 迁移评分函数，根据当前调度方案中工序的类型和所在机器的工序数量，计算优先级，返回工序列表
std::vector<std::string> ScoreProcessForMove(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList);

// 计算可调度工序的各个机器的工序数量，返回最大差值
int GetDifferFromIdealMachine(const Schedule &schedule, const std::vector<Job> &jobs, const int job_id, const int process_id);

// 获取机器上的工序数量
int GetProcessCountByMachineId(const Schedule &schedule, const int machine_id);

// 根据工件名称查找工件
Job SelectJobByJobId(const std::vector<Job> &jobs, const int job_id);
#endif //GREEDYSEARCH_H
