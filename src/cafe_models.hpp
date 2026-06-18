#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Include custom data structures from lib/
#include "../lib/LinkedList.hpp"
#include "../lib/Queue.hpp"
#include "../lib/PriorityQueue.hpp"
#include "../lib/Stack.hpp"
#include "../lib/HashTable.hpp"
#include "../lib/AVL.hpp"
#include "../lib/Algorithms.hpp"
#include "../lib/Vector.hpp"

// ── Models ───────────────────────────────────────────────────────────────

struct MenuItem {
    std::string id;
    std::string name;
    double price = 0.0;
    int sold_count = 0;

    bool operator==(const MenuItem& o) const { return id == o.id; }
    bool operator<(const MenuItem& o) const { return id < o.id; }
};

struct Order {
    int id = 0;
    std::string customer_name;
    Vector<std::pair<MenuItem, int>> items; // Item and quantity
    double total_price = 0.0;
    bool is_vip = false;
    std::string date; // YYYY-MM-DD

    bool operator==(const Order& o) const { return id == o.id; }
};

// Comparator for PriorityQueue (max-heap).
// Since we want FIFO (earlier IDs first) for VIPs, and PriorityQueue is max-heap,
// cmp(a, b) returning true means a has LOWER priority than b.
// So order with smaller ID has HIGHER priority.
struct OrderCmp {
    bool operator()(const Order& a, const Order& b) const {
        return a.id > b.id; // min-heap by ID
    }
};

struct DailyRevenue {
    std::string date;
    double revenue = 0.0;

    bool operator==(const DailyRevenue& o) const { return date == o.date; }
    bool operator<(const DailyRevenue& o) const { return date < o.date; }
};

// Actions for Undo stack
enum class ActionType { ADD_ITEM, EDIT_ITEM, DELETE_ITEM };

struct Action {
    ActionType type;
    MenuItem item;
    MenuItem old_item; // Used for EDIT_ITEM
};

// ── Helpers ──────────────────────────────────────────────────────────────

inline std::string get_system_date() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    std::stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon << "-"
       << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
}

inline std::string format_currency(double amount) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << amount;
    std::string s = ss.str();
    int n = s.length();
    std::string out = "";
    int count = 0;
    for (int i = n - 1; i >= 0; i--) {
        out = s[i] + out;
        count++;
        if (count == 3 && i > 0) {
            out = "." + out;
            count = 0;
        }
    }
    return out + "đ";
}

// ── Manager Classes ──────────────────────────────────────────────────────

class MenuManager {
public:
    LinkedList<MenuItem> menu_list;
    HashTable<std::string, int> sold_counts;
    Stack<Action> undo_stack;

    void add_item(const MenuItem& item, bool record_undo = true) {
        menu_list.insertBack(item);
        if (!sold_counts.contains(item.id)) {
            sold_counts.insert(item.id, item.sold_count);
        }
        if (record_undo) {
            undo_stack.push({ActionType::ADD_ITEM, item, MenuItem{}});
        }
    }

    bool edit_item(const std::string& id, const std::string& name, double price, bool record_undo = true) {
        int idx = -1;
        for (int i = 0; i < menu_list.size(); i++) {
            if (menu_list.at(i).id == id) {
                idx = i;
                break;
            }
        }
        if (idx == -1) return false;

        MenuItem old_item = menu_list.at(idx);
        MenuItem new_item = old_item;
        new_item.name = name;
        new_item.price = price;
        menu_list.at(idx) = new_item;

        if (record_undo) {
            undo_stack.push({ActionType::EDIT_ITEM, new_item, old_item});
        }
        return true;
    }

    bool delete_item(const std::string& id, bool record_undo = true) {
        int idx = -1;
        for (int i = 0; i < menu_list.size(); i++) {
            if (menu_list.at(i).id == id) {
                idx = i;
                break;
            }
        }
        if (idx == -1) return false;

        MenuItem item = menu_list.at(idx);
        menu_list.removeAt(idx);

        if (record_undo) {
            undo_stack.push({ActionType::DELETE_ITEM, item, MenuItem{}});
        }
        return true;
    }

    void sort_by_price(bool ascending) {
        int n = menu_list.size();
        if (n <= 1) return;
        MenuItem* arr = new MenuItem[n];
        for (int i = 0; i < n; i++) {
            arr[i] = menu_list.at(i);
        }
        if (ascending) {
            merge_sort(arr, 0, n - 1, [](const MenuItem& a, const MenuItem& b) {
                return a.price < b.price;
            });
        } else {
            merge_sort(arr, 0, n - 1, [](const MenuItem& a, const MenuItem& b) {
                return a.price > b.price;
            });
        }
        menu_list.clear();
        for (int i = 0; i < n; i++) {
            menu_list.insertBack(arr[i]);
        }
        delete[] arr;
    }

    void sort_by_name(bool ascending) {
        int n = menu_list.size();
        if (n <= 1) return;
        MenuItem* arr = new MenuItem[n];
        for (int i = 0; i < n; i++) {
            arr[i] = menu_list.at(i);
        }
        if (ascending) {
            merge_sort(arr, 0, n - 1, [](const MenuItem& a, const MenuItem& b) {
                return a.name < b.name;
            });
        } else {
            merge_sort(arr, 0, n - 1, [](const MenuItem& a, const MenuItem& b) {
                return a.name > b.name;
            });
        }
        menu_list.clear();
        for (int i = 0; i < n; i++) {
            menu_list.insertBack(arr[i]);
        }
        delete[] arr;
    }

    bool undo() {
        if (undo_stack.empty()) return false;
        Action act = undo_stack.pop();
        if (act.type == ActionType::ADD_ITEM) {
            // Undo add -> delete
            delete_item(act.item.id, false);
        } else if (act.type == ActionType::DELETE_ITEM) {
            // Undo delete -> add back
            add_item(act.item, false);
        } else if (act.type == ActionType::EDIT_ITEM) {
            // Undo edit -> restore old
            edit_item(act.item.id, act.old_item.name, act.old_item.price, false);
        }
        return true;
    }

    void load_from_file(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return;
        menu_list.clear();
        std::string line;
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id, name, price_str, sold_str;
            if (std::getline(ss, id, '|') &&
                std::getline(ss, name, '|') &&
                std::getline(ss, price_str, '|') &&
                std::getline(ss, sold_str, '|')) {
                MenuItem item;
                item.id = id;
                item.name = name;
                item.price = std::stod(price_str);
                item.sold_count = std::stoi(sold_str);
                add_item(item, false);
            }
        }
    }

    void save_to_file(const std::string& path) {
        std::ofstream ofs(path);
        if (!ofs.is_open()) return;
        for (int i = 0; i < menu_list.size(); i++) {
            const auto& item = menu_list.at(i);
            int sold = 0;
            if (sold_counts.contains(item.id)) {
                sold = sold_counts.find(item.id);
            }
            ofs << item.id << "|" << item.name << "|" << item.price << "|" << sold << "\n";
        }
    }
};

class OrderManager {
public:
    Queue<Order> normal_queue;
    PriorityQueue<Order, OrderCmp> vip_queue;
    LinkedList<Order> history_list;
    AVL<DailyRevenue> revenue_tree;
    int order_id_counter = 1;

    Order current_order;
    bool has_current_order = false;

    void add_order(Order& order) {
        order.id = order_id_counter++;
        order.date = get_system_date();
        if (!has_current_order) {
            current_order = order;
            has_current_order = true;
        } else {
            if (order.is_vip) {
                vip_queue.insert(order);
            } else {
                normal_queue.push(order);
            }
        }
    }

    bool process_next_order(Order& processed_order, MenuManager& menu) {
        if (!has_current_order) return false;

        processed_order = current_order;

        // Add to history
        history_list.insertBack(processed_order);

        // Update sold counts in menu manager
        for (int i = 0; i < processed_order.items.size(); i++) {
            const auto& pair = processed_order.items[i];
            const std::string& item_id = pair.first.id;
            int qty = pair.second;
            
            // Update HashTable
            int current_sold = 0;
            if (menu.sold_counts.contains(item_id)) {
                current_sold = menu.sold_counts.find(item_id);
            }
            menu.sold_counts.insert(item_id, current_sold + qty);

            // Update in menu list as well
            for (int j = 0; j < menu.menu_list.size(); j++) {
                if (menu.menu_list.at(j).id == item_id) {
                    menu.menu_list.at(j).sold_count += qty;
                    break;
                }
            }
        }

        // Update revenue tree
        std::string date = processed_order.date;
        DailyRevenue target{date, 0.0};
        if (revenue_tree.search(target)) {
            double existing_rev = 0.0;
            revenue_tree.inorder([&](const DailyRevenue& dr) {
                if (dr.date == date) {
                    existing_rev = dr.revenue;
                }
            });
            revenue_tree.remove(target);
            target.revenue = existing_rev + processed_order.total_price;
            revenue_tree.insert(target);
        } else {
            target.revenue = processed_order.total_price;
            revenue_tree.insert(target);
        }

        // Load next order from queue
        bool has_vip = !vip_queue.empty();
        bool has_norm = !normal_queue.empty();

        if (has_vip) {
            current_order = vip_queue.extract();
            has_current_order = true;
        } else if (has_norm) {
            current_order = normal_queue.front();
            normal_queue.pop();
            has_current_order = true;
        } else {
            current_order = Order{};
            has_current_order = false;
        }

        return true;
    }

    bool undo_process_order(MenuManager& menu) {
        if (history_list.empty()) return false;
        
        int last_idx = history_list.size() - 1;
        Order undone_order = history_list.at(last_idx);
        history_list.removeAt(last_idx);

        // Push current order back to the front of queue if there is one
        if (has_current_order) {
            if (current_order.is_vip) {
                vip_queue.insert(current_order);
            } else {
                Queue<Order> new_queue;
                new_queue.push(current_order);
                while (!normal_queue.empty()) {
                    new_queue.push(normal_queue.front());
                    normal_queue.pop();
                }
                normal_queue = new_queue;
            }
        }

        // Revert current order to the undone order
        current_order = undone_order;
        has_current_order = true;

        // Revert sold counts
        for (int i = 0; i < undone_order.items.size(); i++) {
            const auto& pair = undone_order.items[i];
            const std::string& item_id = pair.first.id;
            int qty = pair.second;
            
            if (menu.sold_counts.contains(item_id)) {
                int current_sold = menu.sold_counts.find(item_id);
                menu.sold_counts.insert(item_id, std::max(0, current_sold - qty));
            }

            for (int j = 0; j < menu.menu_list.size(); j++) {
                if (menu.menu_list.at(j).id == item_id) {
                    menu.menu_list.at(j).sold_count = std::max(0, menu.menu_list.at(j).sold_count - qty);
                    break;
                }
            }
        }

        // Revert revenue
        std::string date = undone_order.date;
        DailyRevenue target{date, 0.0};
        if (revenue_tree.search(target)) {
            double existing_rev = 0.0;
            revenue_tree.inorder([&](const DailyRevenue& dr) {
                if (dr.date == date) {
                    existing_rev = dr.revenue;
                }
            });
            revenue_tree.remove(target);
            double new_rev = existing_rev - undone_order.total_price;
            if (new_rev > 0) {
                target.revenue = new_rev;
                revenue_tree.insert(target);
            }
        }
        return true;
    }

    void load_history(const std::string& path, MenuManager& menu) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return;
        history_list.clear();
        revenue_tree.clear();
        
        std::string line;
        int max_id = 0;
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id_str, name, price_str, vip_str, date, items_data;
            if (std::getline(ss, id_str, '|') &&
                std::getline(ss, name, '|') &&
                std::getline(ss, price_str, '|') &&
                std::getline(ss, vip_str, '|') &&
                std::getline(ss, date, '|') &&
                std::getline(ss, items_data, '|')) {
                
                Order order;
                order.id = std::stoi(id_str);
                order.customer_name = name;
                order.total_price = std::stod(price_str);
                order.is_vip = (vip_str == "1");
                order.date = date;

                if (order.id > max_id) max_id = order.id;

                std::stringstream iss(items_data);
                std::string item_token;
                while (std::getline(iss, item_token, ';')) {
                    if (item_token.empty()) continue;
                    size_t colon = item_token.find(':');
                    if (colon != std::string::npos) {
                        std::string item_id = item_token.substr(0, colon);
                        int qty = std::stoi(item_token.substr(colon + 1));
                        
                        MenuItem m_item;
                        for (int j = 0; j < menu.menu_list.size(); j++) {
                            if (menu.menu_list.at(j).id == item_id) {
                                m_item = menu.menu_list.at(j);
                                break;
                            }
                        }
                        if (!m_item.id.empty()) {
                            order.items.push_back({m_item, qty});
                        }
                    }
                }

                history_list.insertBack(order);

                // Update revenue tree
                DailyRevenue target{date, 0.0};
                if (revenue_tree.search(target)) {
                    double existing_rev = 0.0;
                    revenue_tree.inorder([&](const DailyRevenue& dr) {
                        if (dr.date == date) {
                            existing_rev = dr.revenue;
                        }
                    });
                    revenue_tree.remove(target);
                    target.revenue = existing_rev + order.total_price;
                    revenue_tree.insert(target);
                } else {
                    target.revenue = order.total_price;
                    revenue_tree.insert(target);
                }
            }
        }
        order_id_counter = max_id + 1;
    }

    void save_history(const std::string& path) {
        std::ofstream ofs(path);
        if (!ofs.is_open()) return;
        for (int i = 0; i < history_list.size(); i++) {
            const auto& order = history_list.at(i);
            ofs << order.id << "|" << order.customer_name << "|" << order.total_price << "|"
                << (order.is_vip ? "1" : "0") << "|" << order.date << "|";
            for (int j = 0; j < order.items.size(); j++) {
                ofs << order.items[j].first.id << ":" << order.items[j].second << ";";
            }
            ofs << "\n";
        }
    }
};
