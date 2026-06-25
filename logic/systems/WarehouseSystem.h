#pragma once
/**
 * @file WarehouseSystem.h
 * @brief 仓库系统 - 库存与吞吐管理
 */

namespace delivery {

    class WarehouseSystem {
    public:
        WarehouseSystem() = default;
        ~WarehouseSystem() = default;

        /**
         * @brief 每帧更新
         * @param deltaSeconds 时间增量
         */
        void update(double deltaSeconds);

        /**
         * @brief 收货入库
         */
        bool receiveGoods(int warehouseId, int count);

        /**
         * @brief 发货出库
         */
        int dispatchGoods(int warehouseId, int count);

        /**
         * @brief 检查所有仓库负载率
         */
        void checkWarehouseStatus();

    private:
        double m_timer = 0.0;
        static constexpr double UPDATE_INTERVAL = 10.0;  ///< 每10秒检查一次
    };

} // namespace delivery