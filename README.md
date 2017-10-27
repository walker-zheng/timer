# 简单的 timer
1. 功能
  - priority_queue实现简单定时器
  - 支持 std::function 添加，根据id删除
  - 支持 时间点、循环、指定次数等，可嵌套组合使用
2. 使用示例：
```cpp
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
```

3. timer嵌套：
```cpp
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
```
