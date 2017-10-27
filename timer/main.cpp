#include "main.h"
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
    {
        easy::Timer timer;
        bar b;
        std::vector<int> taskids;
        std::cout << (duration<double>(system_clock::now() - now)).count() << "s => Start!" << std::endl;
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
    }
    done();
    return 0;
}
int main()
{
    test_timer();
    return 0;
}
