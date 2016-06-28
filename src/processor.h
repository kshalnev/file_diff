#pragma once

#include <functional>
#include <mutex>
#include <queue>

class Processor
{
public:
  void Add(std::function<void()> fn);
  void Process();
  
private:
  void ProcessImpl();
  
  std::mutex m_guard;
  std::queue<std::function<void()>> m_queue;
};
