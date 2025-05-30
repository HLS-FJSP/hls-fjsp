//
// Created by luopw on 24-11-30.
//

#include "Order.h"
#include <iostream>

// Order constructor
Order::Order() {
    order_id = "";
    order_items = std::vector<Order_item>();
}

std::string Order::get_order_id() const {return order_id;}
std::vector<Order_item> Order::get_order_items() const{return order_items;}
void Order::set_order_id(const std::string &order_id) {Order::order_id = order_id;}
void Order::set_order_items(const std::vector<Order_item> &order_items) {Order::order_items = order_items;}
void Order::to_string() {
    std::cout << "order_id: " << order_id << std::endl;
    for (auto &[job_name, job_count] : order_items) {
        std::cout << "job_name: " << job_name << ", job_count: " << job_count << std::endl;
    }
}

