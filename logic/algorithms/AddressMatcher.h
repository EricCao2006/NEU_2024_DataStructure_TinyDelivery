#pragma once

#include "Algorithms.h"

namespace algo {
    /**
     * @brief 地址匹配（哈希匹配算法）
     * @todo 地址匹配（哈希匹配算法）
     */
    class AddressMatcher : public algo::Algorithm {
    public:
        AddressMatcher() = default;
        ~AddressMatcher() = default;

        void match();
    };
}