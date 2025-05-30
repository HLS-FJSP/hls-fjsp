//
// Created by luopw on 24-12-1.
//

#ifndef MACHINE_H
#define MACHINE_H
#include <string>

enum Machine_type { //单机，平行机，流水作业多工序机，自由作业多工序机，异序作业多工序机
    Single_machine,  //单机
    Parallel_machine,  //平行机
    Flow_shop_multi_process_machine,  //流水作业多工序机
    Job_shop_multi_process_machine,  //异序作业多工序机
    Open_shop_multi_process_machine  //自由作业多工序机
};

class Machine {
    int machine_id;  // machine id,机器编号
    std::string machine_name;  // machine name,机器名称
    Machine_type machine_type;  // machine type,机器类型
    int count = 0;  // machine count,机器数量
    int idle_count = 0;  // idle machine count,空闲机器数量
    int process_count = 0;  // process count,工序数量

public:
    Machine();
    int get_machine_id() const;
    std::string get_machine_name();
    Machine_type get_machine_type() const;
    int get_count() const;
    int get_idle_count() const;
    int get_process_count() const;
    void set_machine_id(int machine_id);
    void set_machine_name(const std::string &machine_name);
    void set_machine_type(Machine_type machine_type);
    void set_count(int count);
    void set_idle_count(int idle_count);
    void set_process_count(int process_count);
    void to_string() const;
};

#endif //MACHINE_H
