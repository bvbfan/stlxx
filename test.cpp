
#include "stlxx.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

struct Employee {
    std::string id;
    Employee(Employee &&) = default;
    Employee(std::string id) : id(id) {}
    std::vector<std::string> lunch_partners;
};

int main(int argc, char *argv[])
{
    Employee mat("mat");
    std::thread *workers[16];
    stlxx::atomic<std::vector<std::int64_t>> vec;
    stlxx::atomic<Employee> mel = Employee{"mel"}, bob = Employee{"bob"};
    for (auto &worker : workers) {
        worker = new std::thread([=, &mat]() {
            std::stringstream s;
            s << std::this_thread::get_id();
            std::int64_t i64; s >> i64;
            vec->push_back(i64);
            using mutex = std::recursive_mutex&;
            stlxx::synchronized([=, &mat]() {
                mel->lunch_partners.push_back(mat.id);
                mat.lunch_partners.push_back(bob->id);
                bob->lunch_partners.push_back(mel->id);
            }, mutex(mel), mutex(bob));
        });
    }
    for (auto &worker : workers) {
        worker->join();
        delete worker;
    }
    auto &v = vec;
    auto &v1 = mel->lunch_partners;
    auto &v2 = mat.lunch_partners;
    auto &v3 = bob->lunch_partners;
    std::cout << v->size() << ": "
              << v1.size() << ": "
              << v2.size() << ": "
              << v3.size() << '\n';
    return 0;
}
