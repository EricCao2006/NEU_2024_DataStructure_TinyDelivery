#pragma once

#include "Algorithms.h"

namespace algo {
    /**
     * @brief 指令压缩（哈夫曼编码）
     * @todo 指令压缩（哈夫曼编码）
     */
    class InstructionCompressor : public algo::Algorithm {
    public:
        InstructionCompressor() = default;
        ~InstructionCompressor() = default;

        void compress();
    };
}