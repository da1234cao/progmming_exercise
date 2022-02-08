// code come frome:https://www.coder.work/article/1238404
#include <vector>
#include <algorithm>
#include <future>
#include <iostream>
#include <thread>
#include <functional>
#include <random>

template <typename Iterator, typename T>
struct accumulate_block
{
  T operator()(Iterator first, Iterator last)
  {
    return std::accumulate(first, last, T());
  }
};

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
  const unsigned long length = std::distance(first, last);

  if (!length) return init;

  const unsigned long min_per_thread = 25;
  const unsigned long max_threads    = (length) / min_per_thread;
  const unsigned long hardware_conc  = std::thread::hardware_concurrency();
  const unsigned long num_threads    = std::min(hardware_conc != 0 ? hardware_conc : 2, max_threads);
  const unsigned long block_size     = length / num_threads;

  std::vector<std::future<T>> futures(num_threads - 1);
  std::vector<std::thread> threads(num_threads - 1);

  Iterator block_start = first;
  for (unsigned long i = 0; i < (num_threads - 1); ++i)
  {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);

    std::packaged_task<T(Iterator, Iterator)> task{accumulate_block<Iterator, T>()};
    futures[i] = task.get_future();
    threads[i] = std::thread(std::move(task), block_start, block_end);
    block_start = block_end;
  }

  T last_result = accumulate_block<Iterator, T>()(block_start, last);

  for (auto& t : threads) t.join();

  T result = init;
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    result += futures[i].get();
  }
  result += last_result;
  return result;
}

template <typename TimeT = std::chrono::microseconds>
struct measure
{
  template <typename F, typename... Args>
  static typename TimeT::rep execution(F func, Args&&... args)
  {
    using namespace std::chrono;
    auto start = system_clock::now();
    func(std::forward<Args>(args)...);
    auto duration = duration_cast<TimeT>(system_clock::now() - start);
    return duration.count();
  }
};
 
template <typename T>
T parallel(const std::vector<T>& v)
{
  return parallel_accumulate(v.begin(), v.end(), 0);
}

template <typename T>
T stdaccumulate(const std::vector<T>& v)
{
  return std::accumulate(v.begin(), v.end(), 0);
}

int main()
{
  constexpr unsigned int COUNT = 200000000;
  std::vector<int> v(COUNT);

  // optional randomising vector contents - std::accumulate also gives 0us
  // but custom parallel accumulate gives longer times with randomised input
  /**
   * mt19937:32 位梅森缠绕器，由松本与西村设计于 1998
   * uniform_int_distribution:产生在一个范围上均匀分布的整数值
  */
  std::mt19937 mersenne_engine;
  std::uniform_int_distribution<int> dist(1, 100);
  auto gen = std::bind(dist, mersenne_engine);
  std::generate(v.begin(), v.end(), gen);
//   std::fill(v.begin(), v.end(), 1);

  auto v2 = v; // copy to work on the same data

  std::cout << "starting ... " << '\n';
  std::cout << "std::accumulate : \t" << measure<>::execution(stdaccumulate<int>, v) << "us" << '\n';
  std::cout << "parallel: \t" << measure<>::execution(parallel<int>, v2) << "us" << '\n';
}
