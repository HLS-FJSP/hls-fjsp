//
// Created by 28898 on 25-1-3.
//

#ifndef RANDOMSEARCH_H
#define RANDOMSEARCH_H
#include "Job.h"
#include "Machine.h"
#include "Schedule.h"


// 随机搜索
Schedule RandomSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, double strategy_param, int repeat_count, int max_repeat_count, int max_iter_count, int tabu_list_length, int population_size);

// 保守搜索
Schedule ConservativeSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, int tabu_list_length, int max_iter_count);

// 激进搜索
Schedule AggressiveSearch(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, int population_size, int max_iter_count);

// 将调度方案编码为机器选择编码和工序排序编码
void EncodeSchedule(const Schedule &schedule, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<int> &machine_selection_code, std::vector<int> &operation_sequencing_code);
// 将机器选择编码和工序排序编码解码为调度方案
Schedule DecodeSchedule(const std::vector<int> &machine_selection_code, const std::vector<int> &operation_sequencing_code, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, const Schedule &schedule);

// 根据工序获取机器编号
int GetMachineIdByJobIdAndProcessId(const Schedule &schedule, int job_id, int process_id);

// 全局初始化种群
void GlobalInitializePopulation(const std::vector<Job> &jobs, const std::vector<Machine> &machines,const std::vector<std::string> &jobList, const int population_size, const std::vector<int> &operation_sequencing_code,
    std::vector<std::vector<int>> &population_machine_selection_code, std::vector<std::vector<int>> &population_operation_sequencing_code);

// 本地初始化种群
void LocalInitializePopulation(const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, const int population_size, const std::vector<int> &operation_sequencing_code,
    std::vector<std::vector<int>> &population_machine_selection_code, std::vector<std::vector<int>> &population_operation_sequencing_code);

// 随机初始化种群
void RandomInitializePopulation(const std::vector<Job> &jobs, const std::vector<Machine> &machines, const std::vector<std::string> &jobList, const int population_size, const std::vector<int> &operation_sequencing_code,
    std::vector<std::vector<int>> &population_machine_selection_code, std::vector<std::vector<int>> &population_operation_sequencing_code);

// 机器选择编码交叉
void MachineCross(const std::vector<int> &parent1, const std::vector<int> &parent2, std::vector<int> &child1, std::vector<int> &child2);

// 工序排序编码交叉
void OperationCross(const std::vector<int> &parent1, const std::vector<int> &parent2, std::vector<int> &child1, std::vector<int> &child2);

// 机器选择编码变异
void MachineVariation(const std::vector<int> &parent, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, std::vector<int> &child);

// 工序排序编码变异
void OperationVariation(const std::vector<int> &parent, std::vector<int> &child);

// 计算适应度
int CalculateFitness(const std::vector<int> &machine_selection_code, const std::vector<int> &operation_sequencing_code, const std::vector<Job> &jobs, const std::vector<std::string> &jobList, const Schedule &schedule0);
#endif //RANDOMSEARCH_H
