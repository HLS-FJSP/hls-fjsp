//
// Created by luopw on 24-11-30.
//

#ifndef ORDER_H
#define ORDER_H
#include <string>
#include <vector>

struct Order_item {
    std::string  job_name;  // job name,工件名称
    int job_count;  // job count,工件数量
};

class Order {
    std::string order_id;  // order id,订单号
    std::vector<Order_item> order_items;  // order items,订单项

public:
    Order();
    std::string get_order_id() const;
    std::vector<Order_item> get_order_items()const;
    void set_order_id(const std::string &order_id);
    void set_order_items(const std::vector<Order_item> &order_items);
    void to_string();
};

#endif //ORDER_H
