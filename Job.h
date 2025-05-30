//
// Created by luopw on 24-12-1.
//

#ifndef JOB_H
#define JOB_H
#include <string>
#include <vector>

struct Process_item {
    int process_id;  // process id,工序编号
    int machine_id;  // machine id,机器编号
    int process_time;  // process time,加工时间
};

struct Job_process {  // 工序项
    int machine_count;  // machine count,机器数量
    std::vector<Process_item> process_item;  // process item,工序可选项
};

class Job {
    int job_id;  // job id,工件编号
    std::string job_name;  // job name,工件名称
    int process_count;  // process count,工序数量
    std::vector<Job_process> job_process;  // job process,工序

public:
    Job();
    int get_job_id() const;
    std::string get_job_name() const;
    int get_process_count() const;
    std::vector<Job_process> get_job_process();
    void set_job_id(int job_id);
    void set_job_name(const std::string &job_name);
    void set_process_count(int process_count);
    void set_job_process(const std::vector<Job_process> &job_process);
    void to_string();

    int getTotalAverageProcessTime() const;
};

#endif //JOB_H
