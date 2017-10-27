#include "timer.h"
#include <iostream>

using namespace std::chrono;
static auto now = system_clock::now();
void foo()
{
    std::cout << (duration<double>(system_clock::now() - now)).count() << "s => func foo" << std::endl;
}

void done()
{
    std::cout << (duration<double>(system_clock::now() - now)).count() << "s => Done!" << std::endl;
}

struct bar
{
    void hello()
    {
        std::cout << (duration<double>(system_clock::now() - now)).count() << "s => func bar::hello" << std::endl;
    }
};

int test_timer()
{
    events::Timer timer;
    bar b;
    std::vector<int> taskids;
    taskids.push_back(timer.add(foo, now + seconds(2)));
    taskids.push_back(timer.add(std::bind(&bar::hello, b), now + seconds(3), seconds(3), 3));
    taskids.push_back(timer.add(
    [ = ]() { std::cout << (duration<double>(system_clock::now() - now)).count() << "s => func lambda" << std::endl; },
    now + seconds(3), seconds(1)));
    std::this_thread::sleep_for(seconds(10));

    for (auto i : taskids)
    {
        timer.del(i);
    }

    std::this_thread::sleep_for(seconds(5));
    timer.stop();
    std::this_thread::sleep_for(seconds(5));
    done();
    return 0;
}