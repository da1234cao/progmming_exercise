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