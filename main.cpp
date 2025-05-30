#include <iostream>

#include "DataProc.h"
#include "GreedySearch.h"
#include "RandomSearch.h"
#include "Schedule.h"
#include "Job.h"
#include "Machine.h"
#include "Order.h"
#include "Experiment.h"
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
    auto machines = std::vector<Machine>();                   // 机器列表
    auto jobs = std::vector<Job>();                           // 工件列表
    auto orders = std::vector<Order>();                       // 订单列表
    std::string file_path = argc > 1 ? argv[1] : "../input.txt"; // 运行命令示例：./main input.txt
    // cout << "file_path: " << file_path << endl;
    Init(machines, jobs, orders, file_path);   // 初始化机器、工件、订单
    auto jobList = std::vector<std::string>(); // 用于记录每个工件列表，根据订单生成
    OrderToJobList(orders, jobList);           // 将订单转换为待加工的工件列表
    Schedule input_schedule;                   // 输入的调度方案
    Schedule output_schedule;                  // 输出的调度方案

    // ======parameters========
    // double search_mode_param = 0.5;  // 搜索模式参数0~1，配置选择随机或者贪心
    // double random_search_strategy_param = 0.5;  // 搜索模式参数，配置选择遗传算法或者禁忌搜索
    // int max_iter_count = 30;  // 单轮优化最大迭代次数，表示每次搜索的最大迭代次数
    // int population_size = 20;  // 种群大小，表示遗传算法中种群的大小
    // int tabu_list_length = 10;  // 禁忌表长度，表示禁忌搜索算法中禁忌表的长度
    // int max_repeat_count = 20;  // 最优解最大重复次数，达到最大重复次数则退出搜索

    double search_mode_param = 0.5;            // 搜索模式参数0~1，配置选择随机或者贪心
    double random_search_strategy_param = 0.5; // 搜索模式参数，配置选择遗传算法或者禁忌搜索
    int max_iter_count = 30;                   // 单轮优化最大迭代次数，表示每次搜索的最大迭代次数
    int population_size = 20;                  // 种群大小，表示遗传算法中种群的大小
    int tabu_list_length = 10;                 // 禁忌表长度，表示禁忌搜索算法中禁忌表的长度
    int max_repeat_count = 20;                 // 最优解最大重复次数，达到最大重复次数则退出搜索

    // ==========================

    cout << "file_path: " << file_path << endl;

    if (argc > 2)
    {
        Schedule schedule0;
        if (strcmp(argv[2], "FIFO_EET") == 0)
        {
            schedule0 = FIFO_EET(jobs, machines, jobList);
        }
        else if (strcmp(argv[2], "FIFO_SPT") == 0)
        {
            schedule0 = FIFO_SPT(jobs, machines, jobList);
        }
        else if (strcmp(argv[2], "MOPNR_EET") == 0)
        {
            schedule0 = MOPNR_EET(jobs, machines, jobList);
        }
        else if (strcmp(argv[2], "MOPNR_SPT") == 0)
        {
            schedule0 = MOPNR_SPT(jobs, machines, jobList);
        }
        else if (strcmp(argv[2], "MWKR_EET") == 0)
        {
            schedule0 = MWKR_EET(jobs, machines, jobList);
        }
        else if (strcmp(argv[2], "MWKR_SPT") == 0)
        {
            schedule0 = MWKR_SPT(jobs, machines, jobList);
        }
        else
        {
            cout << "Invalid algorithm name. Please use FIFO, MOPNR, MWKR or SPT." << endl;
            return 1;
        }
        // schedule0.to_string();
        // schedule0.ToMapCode();
        const int ans0 = CalculateTotalTime(schedule0);
        cout << "Total time: " << ans0 << endl;

        // schedule0 = FIFO_EET(jobs, machines, jobList);
        // cout << "FIFO_EET Total time: " << CalculateTotalTime(schedule0) << endl;
        // schedule0 = FIFO_SPT(jobs, machines, jobList);
        // cout << "FIFO_SPT Total time: " << CalculateTotalTime(schedule0) << endl;
        // schedule0 = MOPNR_EET(jobs, machines, jobList);
        // cout << "MOPNR_EET Total time: " << CalculateTotalTime(schedule0) << endl;
        // schedule0 = MOPNR_SPT(jobs, machines, jobList);
        // cout << "MOPNR_SPT Total time: " << CalculateTotalTime(schedule0) << endl;
        // schedule0 = MWKR_EET(jobs, machines, jobList);
        // cout << "MWKR_EET Total time: " << CalculateTotalTime(schedule0) << endl;
        // schedule0 = MWKR_SPT(jobs, machines, jobList);
        // cout << "MWKR_SPT Total time: " << CalculateTotalTime(schedule0) << endl;
        // cout << endl;
        return 0;
    }

    // 生成初始解
    Schedule schedule0 = GenerateInitialSolution(jobList, jobs, machines);
    // schedule0.to_string();
    // schedule0.ToMapCode();
    const int ans0 = CalculateTotalTime(schedule0);

    Schedule temp_schedule;
    input_schedule = schedule0;
    output_schedule = schedule0;

    int search_time = time(nullptr), count = 0, repeat_count = 0;
    Schedule start_schedule = input_schedule;
    while (CheckStopFlag(search_time, count, repeat_count, max_repeat_count))
    {

        double random_num = static_cast<double>(std::rand()) / RAND_MAX;
        if (random_num < search_mode_param)
        {
            // 选择随机搜索模式
            temp_schedule = RandomSearch(input_schedule, jobs, machines, jobList, random_search_strategy_param, repeat_count, max_repeat_count, max_iter_count, tabu_list_length, population_size);
            if (temp_schedule.get_TotalTime() < output_schedule.get_TotalTime())
            {
                output_schedule = temp_schedule;
                input_schedule = temp_schedule;
            }
        }
        else
        {
            // 选择贪心搜索模式
            temp_schedule = GreedySearch(input_schedule, jobs, jobList, max_iter_count);
            if (temp_schedule.get_TotalTime() < output_schedule.get_TotalTime())
            {
                output_schedule = temp_schedule;
                input_schedule = temp_schedule;
            }
            else
            {
                // 选择随机搜索模式
                // std::cout << "Random search2" << std::endl;
                temp_schedule = RandomSearch(input_schedule, jobs, machines, jobList, random_search_strategy_param, repeat_count, max_repeat_count, max_iter_count, tabu_list_length, population_size);
                if (temp_schedule.get_TotalTime() < output_schedule.get_TotalTime())
                {
                    output_schedule = temp_schedule;
                    input_schedule = temp_schedule;
                }
            }
        }

        if (output_schedule.get_TotalTime() <= start_schedule.get_TotalTime())
        {
            if (output_schedule.get_TotalTime() < start_schedule.get_TotalTime())
            {
                repeat_count = 1;
            }
            else
            {
                repeat_count++;
            }
            cout << "Total time: " << CalculateTotalTime(output_schedule) << endl;
        }
        count++;
        start_schedule = output_schedule;
    }

    // output_schedule.to_string();
    // output_schedule.ToMapCode();
    const int ans1 = CalculateTotalTime(output_schedule);
    cout << "Total time: " << ans1 << endl;
    cout << "Time cost: " << time(nullptr) - search_time << "s" << endl;
    // 对于file_path为fjsp.hurink.vdata-mt20.m5j20c5.txt 输出格式为：hurink.vdata-mt20.m5j20c5
    // cout << "search_time:" << time(nullptr) - search_time << "s" << endl;
    // cout << "repeat_count:" << repeat_count << endl;
    // cout << "count:" << count << endl;
    cout << endl;
    return 0;
}
