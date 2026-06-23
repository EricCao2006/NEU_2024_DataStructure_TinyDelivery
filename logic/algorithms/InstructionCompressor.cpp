#include "InstructionCompressor.h"
#include <queue>
#include <algorithm>
#include <bitset>

namespace delivery::algo {

// ========== 构建频率表 ==========
std::unordered_map<char, int> InstructionCompressor::buildFrequency(const std::string& text) {
    std::unordered_map<char, int> freq;
    for (char c : text) {
        ++freq[c];
    }
    return freq;
}

// ========== 构建哈夫曼树 ==========
std::unique_ptr<HuffmanNode> InstructionCompressor::buildTree(const std::unordered_map<char, int>& freq) {
    // 最小堆：按频率排序
    auto cmp = [](const HuffmanNode* a, const HuffmanNode* b) {
        return a->frequency > b->frequency;
    };
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(cmp)> pq(cmp);

    // 创建叶节点
    for (auto [ch, f] : freq) {
        auto node = std::make_unique<HuffmanNode>();
        node->symbol = ch;
        node->frequency = f;
        pq.push(node.release());
    }

    // 特殊情况：只有一个符号
    if (pq.size() == 1) {
        auto node = pq.top();
        pq.pop();
        auto root = std::make_unique<HuffmanNode>();
        root->frequency = node->frequency;
        root->left.reset(node);
        return root;
    }

    // 反复合并频率最小的两个节点
    while (pq.size() > 1) {
        auto left = std::unique_ptr<HuffmanNode>(pq.top());
        pq.pop();
        auto right = std::unique_ptr<HuffmanNode>(pq.top());
        pq.pop();

        auto parent = std::make_unique<HuffmanNode>();
        parent->symbol = 0;
        parent->frequency = left->frequency + right->frequency;
        parent->left = std::move(left);
        parent->right = std::move(right);

        pq.push(parent.release());
    }

    return std::unique_ptr<HuffmanNode>(pq.top());
}

// ========== 生成编码表 ==========
void InstructionCompressor::generateCodes(const HuffmanNode* node, const std::string& prefix, CodeTable& out) {
    if (!node) return;

    if (node->isLeaf()) {
        out.codes[node->symbol] = prefix;
        out.totalBits += prefix.length() * node->frequency;
        out.originalBits += 8 * node->frequency;
        return;
    }

    generateCodes(node->left.get(), prefix + "0", out);
    generateCodes(node->right.get(), prefix + "1", out);
}

// ========== 字节转换 ==========
std::vector<uint8_t> InstructionCompressor::encodeToBytes(const std::string& bitString) {
    std::vector<uint8_t> result;
    result.reserve((bitString.length() + 7) / 8);

    for (size_t i = 0; i < bitString.length(); i += 8) {
        uint8_t byte = 0;
        size_t bits = std::min(size_t(8), bitString.length() - i);
        for (size_t j = 0; j < bits; ++j) {
            if (bitString[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        result.push_back(byte);
    }

    return result;
}

std::string InstructionCompressor::decodeFromBytes(const std::vector<uint8_t>& data, size_t totalBits) {
    std::string result;
    result.reserve(totalBits);

    for (size_t i = 0; i < totalBits; ++i) {
        size_t byteIdx = i / 8;
        size_t bitIdx = 7 - (i % 8);
        result += (data[byteIdx] & (1 << bitIdx)) ? '1' : '0';
    }

    return result;
}

// ========== 压缩 ==========
CompressResult InstructionCompressor::compress(const std::string& text) {
    CompressResult result;
    result.originalSize = text.size();

    if (text.empty()) {
        return result;
    }

    // 1. 统计频率
    auto freq = buildFrequency(text);

    // 2. 构建哈夫曼树
    auto root = buildTree(freq);

    // 3. 生成编码表
    CodeTable codeTable;
    generateCodes(root.get(), "", codeTable);

    // 4. 编码文本
    std::string bitString;
    bitString.reserve(codeTable.totalBits);
    for (char c : text) {
        bitString += codeTable.codes[c];
    }

    // 5. 转为字节流
    result.data = encodeToBytes(bitString);
    result.compressedSize = result.data.size();
    result.codeTable = codeTable.codes;
    result.ratio = static_cast<double>(result.compressedSize) / result.originalSize;

    return result;
}

// ========== 解压 ==========
DecompressResult InstructionCompressor::decompress(
    const std::vector<uint8_t>& data,
    const std::unordered_map<char, std::string>& codeTable,
    size_t originalSize
) {
    DecompressResult result;

    if (data.empty() || codeTable.empty()) {
        return result;
    }

    // 1. 构建反向编码表（编码 -> 符号）
    std::unordered_map<std::string, char> reverseTable;
    for (auto [ch, code] : codeTable) {
        reverseTable[code] = ch;
    }

    // 2. 计算总比特数（从原始大小和编码表推断）
    // 实际项目中会在头部存储总比特数，这里简化
    size_t totalBits = data.size() * 8;
    // 去掉末尾的填充位（如果有）
    while (totalBits > 0 && data[totalBits / 8] & (1 << (7 - totalBits % 8))) {
        // 这里不能简单判断，实际应存储填充位数
        // 简化：使用原始大小来限制
        totalBits = originalSize * 8;  // 近似
    }

    std::string bitString = decodeFromBytes(data, totalBits);

    // 3. 解码
    std::string currentCode;
    for (char bit : bitString) {
        currentCode += bit;
        auto it = reverseTable.find(currentCode);
        if (it != reverseTable.end()) {
            result.text += it->second;
            currentCode.clear();
            if (result.text.size() >= originalSize) break;
        }
    }

    result.success = (result.text.size() == originalSize);
    return result;
}

// ========== 文件格式说明 ==========
std::string InstructionCompressor::fileFormatDescription() {
    return R"(
哈夫曼压缩文件格式（自包含）：
+--------+----------+------------+----------+------------+
| 魔数   | 符号数   | 编码表     | 填充位数 | 压缩数据   |
| 4B     | 2B       | 变长       | 1B       | 变长       |
+--------+----------+------------+----------+------------+

魔数: 0x4846464D ("HFFM")
符号数: 编码表中符号的个数
编码表: 每个符号 [符号1B][编码长度1B][编码数据...]
填充位数: 最后一个字节中无效的比特数
压缩数据: 编码后的字节流
)";
}

} // namespace delivery::algo