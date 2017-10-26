#include <functional>
#include <queue>
#include <map>
#include <chrono>
#include <atomic>
#include <memory>
#include <thread>
#include <iostream>
#include <future>
#include <iomanip>
#include <ctime>
#include <time.h>

int test_timer();

namespace events {
struct timer_event
{
    typedef std::function<void()>                               callback_type;
    typedef std::chrono::time_point<std::chrono::system_clock>  time_type;
    typedef std::chrono::duration<int, std::milli>              msec_type;

    timer_event(int id, callback_type const & callback, time_type const & when, msec_type const & duration = msec_type(0),
        bool loop = false)
        : callback(callback), when(when), duration(duration), loop(loop), id(id)
    {
    }
    timer_event(const timer_event & other)
        : callback(other.callback), when(other.when), duration(other.duration), loop(other.loop), id(other.id)
    {
    }
    timer_event & operator=(timer_event other)
    {
        swap(other);
        return *this;
    }
    void swap(timer_event & other)
    {
        std::swap(callback, other.callback);
        std::swap(when, other.when);
        std::swap(duration, other.duration);
        std::swap(loop, other.loop);
        std::swap(id, other.id);
    }
    void operator()() const
    {
        //std::cout << "---operator()---" << std::endl;
        callback();
    }
    timer_event next()
    {
        return timer_event(id, callback, when + duration, duration, loop);
    }

    callback_type   callback;
    time_type       when;
    msec_type       duration;
    bool            loop;
    int             id;
};

struct event_less : public std::less<timer_event>
{
    bool operator()(const timer_event &e1, const timer_event &e2) const
    {
        return (e2.when < e1.when);
    }
};

class Timer
{
    std::priority_queue<timer_event, std::vector<timer_event>, event_less> event_queue;
    std::unique_ptr<std::thread>            handler;
    std::atomic<bool>                       _stop;
    typedef std::chrono::duration<int>      sec_type;
    std::atomic<int>                        taskid{ 0 };
    std::map<int, bool>                     invalid;
public:
    Timer()
    {
        handler = std::make_unique<std::thread>([this]()
        {
            auto start = std::chrono::system_clock::now();
            std::cout << "---start---" << std::endl;

            while (!_stop)
            {
                timer_event::time_type now = std::chrono::system_clock::now();

                while (!event_queue.empty() &&
                    (event_queue.top().when < now))
                {
                    std::cout << "---" << (std::chrono::duration<double>(std::chrono::system_clock::now() - start)).count()
                        << "s" << std::endl;
                    auto & evt = event_queue.top();

                    if (evt.loop)
                    {
                        //std::cout << "---loop next---" << std::endl;
                        event_queue.emplace(evt.id, evt.callback, evt.when + evt.duration, evt.duration, true);
                    }

                    event_queue.pop();
                    std::async(std::launch::async, [ = ]()
                    {
                        if (!invalid[evt.id])
                        {
                            evt();
                        }
                    });
                }

                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    ~Timer() { stop();  if (handler) { handler->join(); } }
    int add(const timer_event::callback_type &cb,
        const std::chrono::time_point<std::chrono::system_clock> &when)
    {
        //std::cout << "---add()---" << std::endl;
        auto id = ++taskid;
        event_queue.emplace(++taskid, cb, when);
        return id;
    }
    int add(const timer_event::callback_type &cb,
        const std::chrono::time_point<std::chrono::system_clock> &when, sec_type duration)
    {
        //std::cout << "---add()---" << std::endl;
        auto id = ++taskid;
        event_queue.emplace(id, cb, when, duration, true);
        return id;
    }
    int add(const timer_event::callback_type &cb,
        const std::chrono::time_point<std::chrono::system_clock> &when, sec_type duration, int times)
    {
        //std::cout << "---add()---" << std::endl;
        auto id = ++taskid;

        if (times > 0)
        {
            int i{ 0 };

            while (times--)
            {
                i++;
                event_queue.emplace(id, cb, when + i * duration, duration);
            }
        }
        else
        {
            event_queue.emplace(id, cb, when, duration, true);
        }

        return id;
    }
    void del(int id)
    {
        invalid[id] = true;
    }
    void stop() { _stop = true; }
};
}