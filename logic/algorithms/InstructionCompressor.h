#pragma once
/**
 * @file InstructionCompressor.h
 * @brief 指令压缩算法
 * @details 基于哈夫曼编码实现的指令集无损压缩
 *          用于压缩下发到配送员的指令数据，节省带宽
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace delivery::algo {

/**
 * @brief 哈夫曼树节点
 */
struct HuffmanNode {
    char symbol = 0;                     ///< 符号（0表示内部节点）
    int frequency = 0;                   ///< 频率
    std::unique_ptr<HuffmanNode> left;   ///< 左子节点（0）
    std::unique_ptr<HuffmanNode> right;  ///< 右子节点（1）

    bool isLeaf() const { return !left && !right; }
};

/**
 * @brief 哈夫曼编码表
 */
struct CodeTable {
    std::unordered_map<char, std::string> codes;  ///< 符号 -> 二进制编码
    int totalBits = 0;                             ///< 编码后总比特数
    int originalBits = 0;                          ///< 原始总比特数（8位/符号）
};

/**
 * @brief 压缩结果
 */
struct CompressResult {
    std::vector<uint8_t> data;           ///< 压缩后的字节流
    std::unordered_map<char, std::string> codeTable;  ///< 编码表（用于解码）
    size_t originalSize = 0;             ///< 原始大小（字节）
    size_t compressedSize = 0;           ///< 压缩后大小（字节）
    double ratio = 0.0;                  ///< 压缩率
};

/**
 * @brief 解压结果
 */
struct DecompressResult {
    std::string text;                    ///< 解压后的原始文本
    bool success = false;
};

/**
 * @brief 哈夫曼压缩器
 */
class InstructionCompressor {
public:
    /**
     * @brief 构建频率表并压缩
     * @param text 原始指令文本
     * @return CompressResult
     */
    CompressResult compress(const std::string& text);

    /**
     * @brief 解压数据
     * @param data 压缩后的字节流
     * @param codeTable 编码表
     * @param originalSize 原始文本大小（用于验证）
     * @return DecompressResult
     */
    DecompressResult decompress(
        const std::vector<uint8_t>& data,
        const std::unordered_map<char, std::string>& codeTable,
        size_t originalSize
    );

    /**
     * @brief 获取压缩后的文件格式说明
     * @details 自包含格式：头部存储编码表，尾部存储压缩数据
     *          [魔数 4B][符号数 2B][编码表...][数据大小 4B][压缩数据...]
     */
    static std::string fileFormatDescription();

private:
    /**
     * @brief 统计字符频率
     */
    std::unordered_map<char, int> buildFrequency(const std::string& text);

    /**
     * @brief 构建哈夫曼树（使用最小堆）
     */
    std::unique_ptr<HuffmanNode> buildTree(const std::unordered_map<char, int>& freq);

    /**
     * @brief 生成编码表（DFS遍历哈夫曼树）
     */
    void generateCodes(const HuffmanNode* node, const std::string& prefix, CodeTable& out);

    /**
     * @brief 将二进制字符串转为字节流
     */
    std::vector<uint8_t> encodeToBytes(const std::string& bitString);

    /**
     * @brief 将字节流转为二进制字符串
     */
    std::string decodeFromBytes(const std::vector<uint8_t>& data, size_t totalBits);

    /**
     * @brief 序列化编码表
     */
    std::vector<uint8_t> serializeTable(const std::unordered_map<char, std::string>& table);

    /**
     * @brief 反序列化编码表
     */
    std::unordered_map<char, std::string> deserializeTable(const std::vector<uint8_t>& data, size_t& offset);
};

} // namespace delivery::algo