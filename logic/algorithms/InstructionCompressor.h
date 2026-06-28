#pragma once

#include "Algorithms.h"

#include <string>
#include <map>

namespace algo {

    /**
     * @brief 指令压缩（哈夫曼编码与译码）
     * @details 将配送指令序列用哈夫曼编码压缩，减少通信开销
     */
    class InstructionCompressor : public Algorithm {
    public:
        InstructionCompressor() = default;
        std::string algorithmName() const override { return "哈夫曼编码(指令压缩)"; }

        /// 哈夫曼树节点
        struct HuffNode {
            char ch = '\0';
            int freq = 0;
            HuffNode* left = nullptr;
            HuffNode* right = nullptr;
            bool operator>(const HuffNode& o) const { return freq > o.freq; }
        };

        using CodeTable = std::map<char, std::string>;

        /**
         * @brief 根据频率表构建哈夫曼树
         * @return 树根指针（调用者负责释放内存）
         */
        HuffNode* buildTree(const std::map<char, int>& freqMap) const;

        /**
         * @brief 从哈夫曼树生成编码表
         */
        CodeTable generateCodes(HuffNode* root);

        /**
         * @brief 压缩：原始字符串 → 哈夫曼编码比特串
         */
        std::string compress(const std::string& input, const CodeTable& codes) const;

        /**
         * @brief 解压：哈夫曼编码比特串 → 原始字符串
         */
        std::string decompress(const std::string& compressed, HuffNode* root) const;

        /**
         * @brief 统计字符频率
         */
        static std::map<char, int> buildFrequencyMap(const std::string& input);
    };

}
