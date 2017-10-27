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
    {}
    timer_event(const timer_event & other)
        : callback(other.callback), when(other.when), duration(other.duration), loop(other.loop), id(other.id)
    {}
    timer_event(timer_event && other)
        : callback(other.callback), when(other.when), duration(other.duration), loop(other.loop), id(other.id)
    {}
    timer_event & operator=(timer_event && other)
    {
        swap(other);
        return *this;
    }
    ~timer_event() {}

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
        callback();
    }
    timer_event & next()
    {
        when += duration;
        return *this;
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
    typedef std::chrono::duration<int>      sec_type;
    std::priority_queue<timer_event, std::vector<timer_event>, event_less> event_queue;
    std::unique_ptr<std::thread>            handler;
    std::atomic<bool>                       _stop;
    std::atomic<int>                        taskid{ 0 };
    std::map<int, bool>                     invalid;
public:
    Timer()
    {
        handler = std::make_unique<std::thread>([this]()
        {
            auto start = std::chrono::system_clock::now();

            while (!_stop)
            {
                timer_event::time_type now = std::chrono::system_clock::now();

                while (!event_queue.empty() &&
                    (event_queue.top().when < now))
                {
                    auto evt = event_queue.top();

                    if (!invalid[evt.id])
                    {
                        // async/thread pool
                        if (evt.loop)
                        {
                            event_queue.emplace(evt.next());
                        }

                        std::async(std::launch::async, [evt]() { evt(); });
                    }

                    event_queue.pop();
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    ~Timer() { stop(); if (handler) { handler->join(); } }

    int add(const timer_event::callback_type & cb,
        const std::chrono::time_point<std::chrono::system_clock> & when,
        const sec_type & duration = sec_type(0), int times = 0)
    {
        auto id = ++taskid;

        if (times > 0)
        {
            timer_event task{ id, cb, when, duration };
            event_queue.emplace(task);

            while (--times)
            {
                event_queue.emplace(task.next());
            }
        }
        else if (duration > sec_type(0))
        {
            event_queue.emplace(id, cb, when, duration, true);
        }
        else
        {
            event_queue.emplace(id, cb, when);
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