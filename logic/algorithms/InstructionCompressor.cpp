#include "InstructionCompressor.h"

#include <functional>
#include <queue>
#include <stack>

namespace algo {

    InstructionCompressor::HuffNode* InstructionCompressor::buildTree(
        const std::map<char, int>& freqMap) const {
        std::priority_queue<HuffNode*, std::vector<HuffNode*>,
            std::function<bool(HuffNode*, HuffNode*)>> pq(
            [](const HuffNode* a, const HuffNode* b) { return a->freq > b->freq; });

        // 为每个字符创建叶子节点
        for (const auto& [ch, freq] : freqMap) {
            pq.push(new HuffNode{ch, freq, nullptr, nullptr});
        }

        // 反复合并最小两个节点，直到只剩一棵树
        while (pq.size() > 1) {
            HuffNode* left = pq.top(); pq.pop();
            HuffNode* right = pq.top(); pq.pop();
            count++;

            auto* parent = new HuffNode{
                '\0', left->freq + right->freq, left, right
            };
            pq.push(parent);
        }

        return pq.empty() ? nullptr : pq.top();
    }

    InstructionCompressor::CodeTable InstructionCompressor::generateCodes(HuffNode* root) {
        CodeTable codes;
        if (!root) return codes;

        // 用栈模拟递归遍历
        std::stack<std::pair<HuffNode*, std::string>> stk;
        stk.emplace(root, "");

        while (!stk.empty()) {
            auto [node, code] = stk.top();
            stk.pop();

            if (!node->left && !node->right) {
                // 叶子节点 → 记录编码
                codes[node->ch] = code.empty() ? "0" : code;
            } else {
                if (node->right) stk.emplace(node->right, code + "1");
                if (node->left)  stk.emplace(node->left,  code + "0");
            }
        }
        return codes;
    }

    std::string InstructionCompressor::compress(const std::string& input,
                                                 const CodeTable& codes) const {
        callCount++;
        std::string result;
        result.reserve(input.size() * 4);
        for (char ch : input) {
            if (auto it = codes.find(ch); it != codes.end()) {
                result += it->second;
            }
            count++;
        }
        return result;
    }

    std::string InstructionCompressor::decompress(const std::string& compressed,
                                                   HuffNode* root) const {
        std::string result;
        if (!root) return result;

        const HuffNode* cur = root;
        for (const char bit : compressed) {
            cur = (bit == '0') ? cur->left : cur->right;
            if (!cur->left && !cur->right) {
                result += cur->ch;
                cur = root;
            }
            count++;
        }
        return result;
    }

    std::map<char, int> InstructionCompressor::buildFrequencyMap(const std::string& input) {
        std::map<char, int> freq;
        for (char ch : input) {
            freq[ch]++;
        }
        return freq;
    }

}
