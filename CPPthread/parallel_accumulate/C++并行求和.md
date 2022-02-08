[toc]

## 前言

最近想了解下C++的并行编程。之前，我翻看过半本的《C++ primer》和《unix环境高级编程》，但这似乎没法让我轻松的的使用C++进行并行编程。《C++ primer》侧重于教我们如何使用C++进行编程；《unix环境高级编程》是一本unix API介绍手册，侧重于C编程。如果想将它俩结合起来，我需要自己使用C++的代码，将unix API封装成类，进行使用。这似乎并不是个好注意：线程的基础工具，应当由高级程序员进行开发，低级程序员调用API，这样高效又安全；这样封装的代码只能在类unix环境下运行，无法在window下运行，不具有跨平台性。所以，C++应当具有自己的线程库，即[线程支持库](https://zh.cppreference.com/w/cpp/thread)。

（1）直接通过阅读[线程支持库-官方手册](https://zh.cppreference.com/w/cpp/thread)，了解C++并行编程，在这里似乎并不是一个好主意。难度在于，不同的API如何正确的组合，以保证安全的并行编程。“安全”需要遵守某些默认的规则，需要“经验”，这是手册无法给予的内容。
（2）接着，我翻看《Linux多线程服务器编程》，但这似乎并不是一本入门的书籍，难度有些大。可能由于书本出来的时间较早，书中代码没有使用C++标准库中提供的线程接口，而是自行实现，比如[Thread.cc](https://github.com/chenshuo/recipes/blob/b9b67c9b05857c2f5a47d91bbc8be93d123e1cbc/thread/Thread.cc)。所以这本书也不适合作为入门了解C++并行编程的书籍。
（3）在接着，我翻看《C++并发编程实战》，它翻译自《C++ Concurrency in action》。英文版本的阅读速度会慢于中文版本的阅读，而中文版本的翻译可能存在一些通顺问题。且这本书长达500页，不适合短期速看。
（4）最后，可算找见一本合适的书籍 ——《C++标准库》。它使用一章来写C++的并发，长度合适；概念介绍和代码演示交错，容易理解；介绍使用API的注意事项，填补官方文档的不足。

---

## 代码展示：C++并行累加

基础的概念介绍，自行翻看上面书籍。这里使用简单的示例，演示C++线程库的简单使用。

**完整代码见仓库或者本篇附录**。

```c++
    // 随机生成一条向量
    std::vector<int> v(1000);
    std::random_device rd;
    std::mt19937_64 gen(rd());     
    std::uniform_int_distribution<int> dis(1,100); //(2.1)
    auto gen_num = std::bind(dis,gen);
    std::generate(v.begin(),v.end(),gen_num);
    // for(int num : v)
    //     std::cout<<num<<" ";

    int result = parallel_accumulate(v); // (2.2)
    int check_result = std::accumulate(v.begin(),v.end(),0);
    
    assert(result == check_result); // (2.3)
```

(2.1) 简单起见，这里累加一个vector中的元素。我们需要先随机填充vector。[uniform_int_distribution](https://zh.cppreference.com/w/cpp/numeric/random/uniform_int_distribution)用于生成范围内的随机值，参照官方的示例，照葫芦画瓢写一个就好。我暂时还没有弄清楚这接口之间的联系。大概意思可能是[random_device](https://zh.cppreference.com/w/cpp/numeric/random/random_device)用于获取种子，[mt19937_64](https://zh.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine)是随机数的生成策略，[uniform_int_distribution](https://zh.cppreference.com/w/cpp/numeric/random/uniform_int_distribution)将数字映射到指定区间。

(2.2) 指定并行累加函数的接口。

(2.3) 将顺序累计结果和并行累加结果，进行比较，判断并行累加的正确性。

```c++
    if(v.empty()) // (2.4)
        return 0;
    if(v.size() == 1)
        return v[0];
    
    // 计算使用的线程数
    unsigned int hc = std::thread::hardware_concurrency(); // (2.5)
    const int thread_num = (hc!=0 ? hc: std::floor(std::log10(v.size())) ); // (2.6)
    int step_size = (v.size()+thread_num-1)/thread_num;
```

（2.4）传入参数的判断。当为空或者只有一个元素时，直接返回就好，不需要并行。

（2.5）[hardware_concurrency](https://zh.cppreference.com/w/cpp/thread/thread/hardware_concurrency)用于返回当前系统支持的并发数。我当前的CPU是四核，所以返回4。需要注意，如果不可计算会返回0。

（2.6）使用thread_num个线程，没有线程累计step_size个元素。

并行求和的总体思路是：
* 传递某些可并行执行的函数,交给std: async()作为调用对象( callable ob-ject)
* 将执行结果赋值给一个 future< Return Type> object
* 当你需要那个被启动函数的执行结果,或当你想确保该函数结束,就对 future<> object调用get()

```c++
    // 并行求和
    std::vector<std::future<int>> tasks(thread_num); //(2.7)
    for(int i=0; i<v.size();){
        int start = i;
        int end = (i+step_size>v.size()) ? v.size():start+step_size;

        auto sum_x = [](const std::vector<int> &v, int s, int e){ //(2.8)
            int result=0; 
            for(int j=s; j<e; j++)
                result += v[j];
            return result;
        };
        std::future<int> task = std::async(std::launch::async,sum_x,v,start,end); //(2.9)
        tasks[i/step_size] = std::move(task);//(2.10)
        i += step_size;
        // (2.11)
    }
```

（2.7）[future](https://zh.cppreference.com/w/cpp/thread/future) 提供访问异步操作结果的机制。这个结果可能是一个返回值，也可能是一个异常，但不会两者都是。这份成果被管理于一个shared state内,后者可被sd:: async()或一个std::packaged_task或一个promise创建出来。这份成果也许尚未存在,因此 future持有的也可能是“生成该成果”的每一件必要东西。如果 future是被 async()返回且其相关的task受到推迟,对它调用get()或wait()会同步启动该task。

（2.8）不想在外层单独写一个求和函数，所以使用了lambda表达式，累加vector中[s,e)范围内的元素。

（2.9）[async](https://zh.cppreference.com/w/cpp/thread/async)异步地运行函数,并返回最终将保有该函数调用结果的 std::future。它的标准调用形式为`async( std::launch policy, Function&& f, Args&&... args );`。

（2.10）[std::future<T>::operator=](https://zh.cppreference.com/w/cpp/thread/future/operator%3D)不允许复制拷贝，需要使用[move](https://zh.cppreference.com/w/cpp/utility/move)进行资源传递。

（2.11）总的来看，因为设计的时候，需要保存线程的执行结果，所以选择future+async的组合，而非使用[thread](https://zh.cppreference.com/w/cpp/thread/thread)。

```c++
    int result = 0;
    for(int i=0; i<tasks.size(); i++){
        result += tasks[i].get(); // (2.12)
    }
```

(2.12) [get 方法](https://zh.cppreference.com/w/cpp/thread/future/get)获取线程的执行结果。get()的返回值取决于 future<>的特化类型。如果它是void,get()获得的就是void。如果future的Template参数是个reference类型,getO便返回一个reference指向返回值。否则get()返回返回值的一份copy,或是对返回值进行move assign动作——取决于返回类型是否支持 move assignment语义。注意,只可调用get一次,因为get()会令future处于无效状态。

---
## C++并行累加-功能优化

（1）上面的代码仅能处理，int型的累加。如果需要扩展到其他数值类型，需要泛型编程。
（2）上面代码使用vector进行存储，利用下标进行累加。如果需要扩展到使用其他存储类型，比如list，需要使用迭代器而非下标。
（3）上面代码，并行和串行执行，没有记录时间以比较效率。

上面三点的功能优化，可参考：[累加的并行](https://www.coder.work/article/1238404) | [parallel version of accumulate](https://stackoverflow.com/questions/36271005/why-would-a-parallel-version-of-accumulate-be-so-much-slower)

---

## 附录

完整代码。

```c++
#include <iostream>
#include <vector>
#include <future>
#include <random>
#include <functional>
#include <algorithm>
#include <cassert>

int parallel_accumulate(const std::vector<int> &v){
    if(v.empty())
        return 0;
    if(v.size() == 1)
        return v[0];
    
    // 计算使用的线程数
    unsigned int hc = std::thread::hardware_concurrency();
    const int thread_num = (hc!=0 ? hc: std::floor(std::log10(v.size())) );
    int step_size = (v.size()+thread_num-1)/thread_num;

    // 并行求和
    std::vector<std::future<int>> tasks(thread_num);
    for(int i=0; i<v.size();){
        int start = i;
        int end = (i+step_size>v.size()) ? v.size():start+step_size;

        auto sum_x = [](const std::vector<int> &v, int s, int e){
            int result=0; 
            for(int j=s; j<e; j++)
                result += v[j];
            return result;
        };
        std::future<int> task = std::async(std::launch::async,sum_x,v,start,end);
        tasks[i/step_size] = std::move(task);
        i += step_size;
    }

    // 结果累加
    int result = 0;
    for(int i=0; i<tasks.size(); i++){
        result += tasks[i].get();
    }

    return result;
}

int main(void){

    // 随机生成一条向量
    std::vector<int> v(1000);
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> dis(1,100);
    auto gen_num = std::bind(dis,gen);
    std::generate(v.begin(),v.end(),gen_num);
    // for(int num : v)
    //     std::cout<<num<<" ";

    int result = parallel_accumulate(v);
    int check_result = std::accumulate(v.begin(),v.end(),0);
    
    assert(result == check_result);
    return 0;
}
```