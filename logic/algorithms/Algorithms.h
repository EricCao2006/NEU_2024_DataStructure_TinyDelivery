// TODO：算法类、算法原型、装饰器

#pragma once

namespace algo {
/**
 * @brief 算法类
 * @details 抽象类，不实例化
 */
class Algorithm {

protected:
    //算法计数
    int count = 0;

public:
    //算法计数
    int getCount() {return count;}
};


}