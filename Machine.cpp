//
// Created by luopw on 24-12-1.
//

#include "Machine.h"

#include <iostream>

Machine::Machine() {
    machine_id = 0;
    machine_name = "";
    machine_type = Job_shop_multi_process_machine;
    count = 0;
    idle_count = 0;
}
int Machine::get_machine_id() const {return machine_id;}
std::string Machine::get_machine_name() {return machine_name;}
Machine_type Machine::get_machine_type() const {return machine_type;}
int Machine::get_count() const {return count;}
int Machine::get_idle_count() const {return idle_count;}
int Machine::get_process_count() const {return process_count;}
void Machine::set_machine_id(const int machine_id) {this->machine_id = machine_id;}
void Machine::set_machine_name(const std::string &machine_name) {this->machine_name = machine_name;}
void Machine::set_machine_type(const Machine_type machine_type) {this->machine_type = machine_type;}
void Machine::set_count(const int count) {this->count = count;}
void Machine::set_idle_count(const int idle_count) {this->idle_count = idle_count;}
void Machine::set_process_count(const int process_count) {this->process_count = process_count;}
void Machine::to_string() const {
    std::cout << "machine_id: " << machine_id << ", machine_name: " << machine_name << ", machine_type: " << machine_type << ", count: " << count << ", idle_count: " << idle_count << std::endl;
}

