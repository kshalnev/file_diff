#include "processor.h"

#include <algorithm>
#include <thread>

void Processor::Add(std::function<void()> fn)
{
    std::lock_guard<std::mutex> lg(m_guard);
    m_queue.emplace(std::move(fn));
}

void Processor::Process()
{
  static const size_t hc = std::max(1U, std::thread::hardware_concurrency());
    
  std::vector<std::thread> threads(hc - 1);
  for (auto & t : threads) {
    t = std::thread([this] { ProcessImpl(); });
  }

  ProcessImpl();
  
  for (auto & t : threads) {
    t.join();
  }
}

void Processor::ProcessImpl()
{
  while (true)
  {
    std::function<void()> fn;
    
    {
      std::lock_guard<std::mutex> lg(m_guard);
      if (m_queue.empty()) break;
      fn = std::move(m_queue.front());
      m_queue.pop();
    }
    
    fn();
  }
}
