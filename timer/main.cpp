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
    std::cout << "> " << (duration<double>(system_clock::now() - now)).count() << "s => Done!" << std::endl;
}

struct bar
{
    void hello()
    {
        std::cout << (duration<double>(system_clock::now() - now)).count() << "s => func bar::hello" << std::endl;
    }
};

int test_loop_timer()
{
    easy::Timer g_timer;
    g_timer.add([ = ]()
    {
        easy::Timer timer;
        bar b;
        std::vector<int> taskids;
        auto curr = system_clock::now();
        std::cout << "> timer " << ": " << (duration<double>(curr - now)).count() << "s => Start!" << std::endl;
        taskids.push_back(timer.add(foo, curr + seconds(2)));
        taskids.push_back(timer.add(std::bind(&bar::hello, b), curr + seconds(3), seconds(3), 3));
        taskids.push_back(timer.add(
        [ = ]() { std::cout << (duration<double>(system_clock::now() - now)).count() << "s => func lambda" << std::endl; },
        curr + seconds(4), seconds(2)));
        std::this_thread::sleep_for(seconds(10));

        for (auto id : taskids)
        {
            std::cout << "> timer " << ": del " << id << std::endl;
            timer.del(id);
        }

        timer.stop();
        curr = system_clock::now();
        std::cout << "> timer " << ": " << (duration<double>(curr - now)).count() << "s => Stop!" << std::endl;
    },
    system_clock::now() + seconds(4), seconds(15));
    std::this_thread::sleep_for(seconds(60));
    return 0;
}
int test_timer()
{
    std::vector<std::thread> timers;

    for (auto i = 0; i < 5; i++)
    {
        timers.emplace_back([i]()
        {
            std::this_thread::sleep_for(seconds(12 * i));
            easy::Timer timer;
            bar b;
            std::vector<int> taskids;
            auto curr = system_clock::now();
            std::cout << "> timer " << i << ": " << (duration<double>(curr - now)).count() << "s => Start!" << std::endl;
            taskids.push_back(timer.add(foo, curr + seconds(2)));
            taskids.push_back(timer.add(std::bind(&bar::hello, b), curr + seconds(3), seconds(3), 3));
            taskids.push_back(timer.add(
            [ = ]() { std::cout << (duration<double>(system_clock::now() - now)).count() << "s => func lambda" << std::endl; },
            curr + seconds(4), seconds(2)));
            std::this_thread::sleep_for(seconds(10));

            for (auto id : taskids)
            {
                std::cout << "> timer " << i << ": del " << id << std::endl;
                timer.del(id);
            }

            timer.stop();
            curr = system_clock::now();
            std::cout << "> timer " << i << ": " << (duration<double>(curr - now)).count() << "s => Stop!" << std::endl;
        });
    }

    for (auto & th : timers)
    {
        th.join();
    }

    done();
    return 0;
}
int main()
{
    test_loop_timer();
    //test_timer();
    return 0;
}
