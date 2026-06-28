// TODO：算法类、算法原型、装饰器

#pragma once

#include <string>

namespace algo {
/**
 * @brief 算法基类
 */
class Algorithm {

protected:
    // 操作计数（mutable 以允许在 const 方法中递增）
    mutable int count = 0;
    // 算法调用次数
    mutable int callCount = 0;

public:
    // 操作计数
    int getCount() const {return count;}
    // 调用次数
    int getCallCount() const {return callCount;}
    // 算法名称（用于统计输出）
    virtual std::string algorithmName() const { return "Unknown"; }
};


}