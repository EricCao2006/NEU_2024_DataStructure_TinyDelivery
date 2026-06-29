#pragma once

/**
 * @brief 全局枚举定义
 */

enum class OrderStatus {
    Pending,     // 待接单
    Assigned,    // 已派单
    PickingUp,   // 取货中
    Delivering,  // 配送中
    Completed,   // 已完成
    Cancelled    // 已取消
};

enum class MerchantType {
    Food = 0,       // 餐饮
    Clothing = 1,   // 服饰
    Retail = 2      // 零售
};

enum class TileType {
    Road,       // 道路（可通行）
    Obstacle,   // 障碍物（不可通行）
    Merchant,   // 商家
    Customer,   // 客户
    Warehouse   // 仓库
};
