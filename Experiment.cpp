//
// Created by luopw on 25-5-7.
//

#include "Experiment.h"
#include "DataProc.h"
#include "Schedule.h"

#include <fstream>
#include <iostream>
#include <queue>
#include <climits>

Schedule FIFO_EET(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList){
    Schedule schedule;
    const int machineNum = machines.size();
    schedule.set_machine_count(machineNum);
    auto temp_schedule_items = std::vector<Schedule_item>();

    for (int i = 0; i < machineNum; i++)
    {
        Schedule_item scheduleItem;
        scheduleItem.machine_id = i;
        scheduleItem.process_count = 0;
        scheduleItem.schedule_process.clear();
        temp_schedule_items.push_back(scheduleItem);
    }
    schedule.set_schedule_items(temp_schedule_items);

    auto mchine_time = std::vector<int>(machineNum, 0);
    auto job_time = std::vector<int>(jobList.size(), 0);
    auto job_now_process = std::vector<int>(jobList.size(), 0);

    int process_count = 0;
    for(auto &jobName: jobList){
        auto job = SelectJobByJobName(jobs, jobName);
        process_count += job.get_process_count();
    }
    while(process_count > 0){
        // 获取最早完成工序的工件
        int min_time_job_index = -1;
        int min_time = INT_MAX;
        for(int i = 0; i < jobList.size(); i++){
            if(job_time[i] < min_time){
                min_time = job_time[i];
                min_time_job_index = i;
            }
        }
        auto job = SelectJobByJobName(jobs, jobList[min_time_job_index]);
        int process_id = job_now_process[min_time_job_index];
        job_now_process[min_time_job_index]++;

        // 获取process_id工序可选机器中最早完成的机器
        int min_machine_time = INT_MAX;
        int min_process_time = INT_MAX;
        Process_item min_machine_process;
        for(int i =0;i<job.get_job_process()[process_id].process_item.size(); i++){
            int machine_id = job.get_job_process()[process_id].process_item[i].machine_id;
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            if(mchine_time[machine_id] < min_machine_time)
            {
                min_machine_time = mchine_time[machine_id];
                min_machine_process = job.get_job_process()[process_id].process_item[i];
            }
            else if (mchine_time[machine_id] == min_machine_time && process_time < min_process_time)
            {
                min_process_time = process_time;
                min_machine_process = job.get_job_process()[process_id].process_item[i];
            }
        }
        schedule.AddProcess(min_machine_process.machine_id, job.get_job_id(), process_id);
        for(auto &machine: machines){
            if(machine.get_machine_id() == min_machine_process.machine_id){
                machine.set_process_count(machine.get_process_count() + 1);
                mchine_time[machine.get_machine_id()] += min_machine_process.process_time;
                break;
            }
        }
        if (job_now_process[min_time_job_index] == job.get_process_count()){
            job_time[min_time_job_index] = INT_MAX;
        } else {
            job_time[min_time_job_index] += min_machine_process.process_time;
        } 
        process_count--;
    }

    std::vector<Schedule_item> temp_items = schedule.get_schedule_items();
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(schedule, temp_items, jobs, jobList, true);
    schedule.set_schedule_id(0);
    return schedule;
}

Schedule FIFO_SPT(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList)
{
    Schedule schedule;
    const int machineNum = machines.size();
    schedule.set_machine_count(machineNum);
    auto temp_schedule_items = std::vector<Schedule_item>();

    for (int i = 0; i < machineNum; i++)
    {
        Schedule_item scheduleItem;
        scheduleItem.machine_id = i;
        scheduleItem.process_count = 0;
        scheduleItem.schedule_process.clear();
        temp_schedule_items.push_back(scheduleItem);
    }
    schedule.set_schedule_items(temp_schedule_items);

    auto mchine_time = std::vector<int>(machineNum, 0);
    auto job_time = std::vector<int>(jobList.size(), 0);
    auto job_now_process = std::vector<int>(jobList.size(), 0);

    int process_count = 0;
    for (auto &jobName : jobList)
    {
        auto job = SelectJobByJobName(jobs, jobName);
        process_count += job.get_process_count();
    }
    while (process_count > 0)
    {
        // 获取最早完成工序的工件
        int min_time_job_index = -1;
        int min_time = INT_MAX;
        for (int i = 0; i < jobList.size(); i++)
        {
            if (job_time[i] < min_time)
            {
                min_time = job_time[i];
                min_time_job_index = i;
            }
        }
        auto job = SelectJobByJobName(jobs, jobList[min_time_job_index]);
        int process_id = job_now_process[min_time_job_index];
        job_now_process[min_time_job_index]++;

        // 获取process_id工序可选机器中加工时间最短的工序
        int min_machine_time = INT_MAX;
        int min_process_time = INT_MAX;
        Process_item min_time_process;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size(); i++)
        {
            int machine_id = job.get_job_process()[process_id].process_item[i].machine_id;
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            if (process_time < min_process_time)
            {
                min_process_time = process_time;
                min_time_process = job.get_job_process()[process_id].process_item[i];
            }
            else if (process_time == min_process_time && mchine_time[machine_id] < min_machine_time)
            {
                min_machine_time = mchine_time[machine_id];
                min_time_process = job.get_job_process()[process_id].process_item[i];
            }
        }
        schedule.AddProcess(min_time_process.machine_id, job.get_job_id(), process_id);
        for (auto &machine : machines)
        {
            if (machine.get_machine_id() == min_time_process.machine_id)
            {
                machine.set_process_count(machine.get_process_count() + 1);
                mchine_time[machine.get_machine_id()] += min_time_process.process_time;
                break;
            }
        }
        if (job_now_process[min_time_job_index] == job.get_process_count())
        {
            job_time[min_time_job_index] = INT_MAX;
        }
        else
        {
            job_time[min_time_job_index] += min_time_process.process_time;
        }
        process_count--;
    }

    std::vector<Schedule_item> temp_items = schedule.get_schedule_items();
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(schedule, temp_items, jobs, jobList, true);
    schedule.set_schedule_id(0);
    return schedule;
}

Schedule MOPNR_EET(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList){
    Schedule schedule;
    const int machineNum = machines.size();
    schedule.set_machine_count(machineNum);
    auto temp_schedule_items = std::vector<Schedule_item>();

    for (int i = 0; i < machineNum; i++)
    {
        Schedule_item scheduleItem;
        scheduleItem.machine_id = i;
        scheduleItem.process_count = 0;
        scheduleItem.schedule_process.clear();
        temp_schedule_items.push_back(scheduleItem);
    }
    schedule.set_schedule_items(temp_schedule_items);

    auto mchine_time = std::vector<int>(machineNum, 0);
    auto job_time = std::vector<int>(jobList.size(), 0);
    auto job_now_process = std::vector<int>(jobList.size(), 0);
    auto job_process_count = std::vector<int>(jobList.size(), 0);

    int process_count = 0;
    for (auto &jobName : jobList)
    {
        auto job = SelectJobByJobName(jobs, jobName);
        process_count += job.get_process_count();
        job_process_count[job.get_job_id()] = job.get_process_count();
    }
    while (process_count > 0)
    {
        // 获取最早完成工序的工件
        int max_process_count_job_index = -1;
        int max_process_count = INT_MIN;
        for (int i = 0; i < jobList.size(); i++)
        {
            if (job_process_count[i] > max_process_count)
            {
                max_process_count = job_process_count[i];
                max_process_count_job_index = i;
            }
        }
        auto job = SelectJobByJobName(jobs, jobList[max_process_count_job_index]);
        int process_id = job_now_process[max_process_count_job_index];
        job_now_process[max_process_count_job_index]++;
        job_process_count[max_process_count_job_index]--;

        // 获取process_id工序可选机器中最早完成的机器
        int min_machine_time = INT_MAX;
        int min_process_time = INT_MAX;
        Process_item min_machine_process;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size(); i++)
        {
            int machine_id = job.get_job_process()[process_id].process_item[i].machine_id;
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            if (mchine_time[machine_id] < min_machine_time)
            {
                min_machine_time = mchine_time[machine_id];
                min_machine_process = job.get_job_process()[process_id].process_item[i];
            }
            else if (mchine_time[machine_id] == min_machine_time && process_time < min_process_time)
            {
                min_process_time = process_time;
                min_machine_process = job.get_job_process()[process_id].process_item[i];
            }
        }
        schedule.AddProcess(min_machine_process.machine_id, job.get_job_id(), process_id);
        for (auto &machine : machines)
        {
            if (machine.get_machine_id() == min_machine_process.machine_id)
            {
                machine.set_process_count(machine.get_process_count() + 1);
                mchine_time[machine.get_machine_id()] += min_machine_process.process_time;
                break;
            }
        }
        if (job_now_process[max_process_count_job_index] == job.get_process_count())
        {
            job_time[max_process_count_job_index] = INT_MAX;
        }
        else
        {
            job_time[max_process_count_job_index] += min_machine_process.process_time;
        }
        process_count--;
    }

    std::vector<Schedule_item> temp_items = schedule.get_schedule_items();
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(schedule, temp_items, jobs, jobList, true);
    schedule.set_schedule_id(0);
    return schedule;
}

Schedule MOPNR_SPT(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList)
{
    Schedule schedule;
    const int machineNum = machines.size();
    schedule.set_machine_count(machineNum);
    auto temp_schedule_items = std::vector<Schedule_item>();

    for (int i = 0; i < machineNum; i++)
    {
        Schedule_item scheduleItem;
        scheduleItem.machine_id = i;
        scheduleItem.process_count = 0;
        scheduleItem.schedule_process.clear();
        temp_schedule_items.push_back(scheduleItem);
    }
    schedule.set_schedule_items(temp_schedule_items);

    auto mchine_time = std::vector<int>(machineNum, 0);
    auto job_time = std::vector<int>(jobList.size(), 0);
    auto job_now_process = std::vector<int>(jobList.size(), 0);
    auto job_process_count = std::vector<int>(jobList.size(), 0);

    int process_count = 0;
    for (auto &jobName : jobList)
    {
        auto job = SelectJobByJobName(jobs, jobName);
        process_count += job.get_process_count();
        job_process_count[job.get_job_id()] = job.get_process_count();
    }
    while (process_count > 0)
    {
        // 获取最早完成工序的工件
        int max_process_count_job_index = -1;
        int max_process_count = INT_MIN;
        for (int i = 0; i < jobList.size(); i++)
        {
            if (job_process_count[i] > max_process_count)
            {
                max_process_count = job_process_count[i];
                max_process_count_job_index = i;
            }
        }
        auto job = SelectJobByJobName(jobs, jobList[max_process_count_job_index]);
        int process_id = job_now_process[max_process_count_job_index];
        job_now_process[max_process_count_job_index]++;
        job_process_count[max_process_count_job_index]--;

        // 获取process_id工序可选机器中加工时间最短的工序
        int min_machine_time = INT_MAX;
        int min_process_time = INT_MAX;
        Process_item min_time_process;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size(); i++)
        {
            int machine_id = job.get_job_process()[process_id].process_item[i].machine_id;
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            if (process_time < min_process_time)
            {
                min_process_time = process_time;
                min_time_process = job.get_job_process()[process_id].process_item[i];
            }
            else if (process_time == min_process_time && mchine_time[machine_id] < min_machine_time)
            {
                min_machine_time = mchine_time[machine_id];
                min_time_process = job.get_job_process()[process_id].process_item[i];
            }
        }
        schedule.AddProcess(min_time_process.machine_id, job.get_job_id(), process_id);
        for (auto &machine : machines)
        {
            if (machine.get_machine_id() == min_time_process.machine_id)
            {
                machine.set_process_count(machine.get_process_count() + 1);
                mchine_time[machine.get_machine_id()] += min_time_process.process_time;
                break;
            }
        }
        if (job_now_process[max_process_count_job_index] == job.get_process_count())
        {
            job_time[max_process_count_job_index] = INT_MAX;
        }
        else
        {
            job_time[max_process_count_job_index] += min_time_process.process_time;
        }
        process_count--;
    }

    std::vector<Schedule_item> temp_items = schedule.get_schedule_items();
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(schedule, temp_items, jobs, jobList, true);
    schedule.set_schedule_id(0);
    return schedule;
}

Schedule MWKR_EET(std::vector<Job> & jobs, std::vector<Machine> & machines, std::vector<std::string> & jobList)
{
    Schedule schedule;
    const int machineNum = machines.size();
    schedule.set_machine_count(machineNum);
    auto temp_schedule_items = std::vector<Schedule_item>();

    for (int i = 0; i < machineNum; i++)
    {
        Schedule_item scheduleItem;
        scheduleItem.machine_id = i;
        scheduleItem.process_count = 0;
        scheduleItem.schedule_process.clear();
        temp_schedule_items.push_back(scheduleItem);
    }
    schedule.set_schedule_items(temp_schedule_items);

    auto mchine_time = std::vector<int>(machineNum, 0);
    auto job_time = std::vector<int>(jobList.size(), 0);
    auto job_now_process = std::vector<int>(jobList.size(), 0);
    auto job_time_remain = std::vector<int>(jobList.size(), 0);

    int process_count = 0;
    for (auto &jobName : jobList)
    {
        auto job = SelectJobByJobName(jobs, jobName);
        process_count += job.get_process_count();
        int total_time = job.getTotalAverageProcessTime();
        job_time_remain[job.get_job_id()] = total_time;
    }
    while (process_count > 0)
    {
        // 获取最早完成工序的工件
        int max_time_remain_job_index = -1;
        int max_time_remain = INT_MIN;
        for (int i = 0; i < jobList.size(); i++)
        {
            if (job_time_remain[i] > max_time_remain)
            {
                max_time_remain = job_time_remain[i];
                max_time_remain_job_index = i;
            }
        }
        auto job = SelectJobByJobName(jobs, jobList[max_time_remain_job_index]);
        int process_id = job_now_process[max_time_remain_job_index];
        job_now_process[max_time_remain_job_index]++;
        int process_average_time = 0;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size();i++){
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            process_average_time += process_time;
        }
        process_average_time /= job.get_job_process()[process_id].machine_count;
        job_time_remain[max_time_remain_job_index] -= process_average_time;

        // 获取process_id工序可选机器中最早完成的机器
        int min_machine_time = INT_MAX;
        int min_process_time = INT_MAX;
        Process_item min_machine_process;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size(); i++)
        {
            int machine_id = job.get_job_process()[process_id].process_item[i].machine_id;
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            if (mchine_time[machine_id] < min_machine_time)
            {
                min_machine_time = mchine_time[machine_id];
                min_machine_process = job.get_job_process()[process_id].process_item[i];
            }
            else if (mchine_time[machine_id] == min_machine_time && process_time < min_process_time)
            {
                min_process_time = process_time;
                min_machine_process = job.get_job_process()[process_id].process_item[i];
            }
        }
        schedule.AddProcess(min_machine_process.machine_id, job.get_job_id(), process_id);
        for (auto &machine : machines)
        {
            if (machine.get_machine_id() == min_machine_process.machine_id)
            {
                machine.set_process_count(machine.get_process_count() + 1);
                mchine_time[machine.get_machine_id()] += min_machine_process.process_time;
                break;
            }
        }
        if (job_now_process[max_time_remain_job_index] == job.get_process_count())
        {
            job_time[max_time_remain_job_index] = INT_MAX;
            job_time_remain[max_time_remain_job_index] = 0;
        }
        else
        {
            job_time[max_time_remain_job_index] += min_machine_process.process_time;
        }
        process_count--;
    }

    std::vector<Schedule_item> temp_items = schedule.get_schedule_items();
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(schedule, temp_items, jobs, jobList, true);
    schedule.set_schedule_id(0);
    return schedule;
}

Schedule MWKR_SPT(std::vector<Job> &jobs, std::vector<Machine> &machines, std::vector<std::string> &jobList)
{
    Schedule schedule;
    const int machineNum = machines.size();
    schedule.set_machine_count(machineNum);
    auto temp_schedule_items = std::vector<Schedule_item>();

    for (int i = 0; i < machineNum; i++)
    {
        Schedule_item scheduleItem;
        scheduleItem.machine_id = i;
        scheduleItem.process_count = 0;
        scheduleItem.schedule_process.clear();
        temp_schedule_items.push_back(scheduleItem);
    }
    schedule.set_schedule_items(temp_schedule_items);

    auto mchine_time = std::vector<int>(machineNum, 0);
    auto job_time = std::vector<int>(jobList.size(), 0);
    auto job_now_process = std::vector<int>(jobList.size(), 0);
    auto job_time_remain = std::vector<int>(jobList.size(), 0);

    int process_count = 0;
    for (auto &jobName : jobList)
    {
        auto job = SelectJobByJobName(jobs, jobName);
        process_count += job.get_process_count();
        int total_time = job.getTotalAverageProcessTime();
        job_time_remain[job.get_job_id()] = total_time;
    }
    while (process_count > 0)
    {
        // 获取最早完成工序的工件
        int max_time_remain_job_index = -1;
        int max_time_remain = INT_MIN;
        for (int i = 0; i < jobList.size(); i++)
        {
            if (job_time_remain[i] > max_time_remain)
            {
                max_time_remain = job_time_remain[i];
                max_time_remain_job_index = i;
            }
        }
        auto job = SelectJobByJobName(jobs, jobList[max_time_remain_job_index]);
        int process_id = job_now_process[max_time_remain_job_index];
        job_now_process[max_time_remain_job_index]++;
        int process_average_time = 0;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size(); i++)
        {
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            process_average_time += process_time;
        }
        process_average_time /= job.get_job_process()[process_id].machine_count;
        job_time_remain[max_time_remain_job_index] -= process_average_time;

        // 获取process_id工序可选机器中加工时间最短的工序
        int min_machine_time = INT_MAX;
        int min_process_time = INT_MAX;
        Process_item min_time_process;
        for (int i = 0; i < job.get_job_process()[process_id].process_item.size(); i++)
        {
            int machine_id = job.get_job_process()[process_id].process_item[i].machine_id;
            int process_time = job.get_job_process()[process_id].process_item[i].process_time;
            if (process_time < min_process_time)
            {
                min_process_time = process_time;
                min_time_process = job.get_job_process()[process_id].process_item[i];
            }
            else if (process_time == min_process_time && mchine_time[machine_id] < min_machine_time)
            {
                min_machine_time = mchine_time[machine_id];
                min_time_process = job.get_job_process()[process_id].process_item[i];
            }
        }
        schedule.AddProcess(min_time_process.machine_id, job.get_job_id(), process_id);
        for (auto &machine : machines)
        {
            if (machine.get_machine_id() == min_time_process.machine_id)
            {
                machine.set_process_count(machine.get_process_count() + 1);
                mchine_time[machine.get_machine_id()] += min_time_process.process_time;
                break;
            }
        }
        if (job_now_process[max_time_remain_job_index] == job.get_process_count())
        {
            job_time[max_time_remain_job_index] = INT_MAX;
            job_time_remain[max_time_remain_job_index] = 0;
        }
        else
        {
            job_time[max_time_remain_job_index] +=  min_time_process.process_time;
        }
        process_count--;
    }

    std::vector<Schedule_item> temp_items = schedule.get_schedule_items();
    std::vector<std::vector<int>> graph = ScheduleItemsToGraph(schedule, temp_items, jobs, jobList, true);
    schedule.set_schedule_id(0);
    return schedule;
}
