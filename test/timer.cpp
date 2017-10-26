#include "timer.h"
#include <iostream>

void foo()
{
    std::cout << "hello from foo" << std::endl;
}

void done()
{
    std::cout << "Done!" << std::endl;
}

struct bar
{
    void hello()
    {
        std::cout << "Hello from bar::hello" << std::endl;
    }
};

int test_timer()
{
    using namespace std::chrono;
    events::Timer timer;
    auto now = std::chrono::system_clock::now();
    bar b;
    std::vector<int> taskids;
    taskids.push_back(timer.add(foo, now + seconds(2)));
    taskids.push_back(timer.add(std::bind(&bar::hello, b), now + seconds(4), seconds(3), 5));
    taskids.push_back(timer.add([ = ]() { std::cout << "last!" << std::endl; }, now + seconds(6), seconds(3)));
    std::this_thread::sleep_for(seconds(30));

    for (auto i : taskids)
    {
        timer.del(i);
    }

    std::this_thread::sleep_for(seconds(30));
    done();
    return 0;
}