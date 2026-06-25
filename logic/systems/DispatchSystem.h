#pragma once
/**
 * @file DispatchSystem.h
 * @brief 调度系统 - 订单分配核心
 */

namespace delivery {

    /**
     * @brief 调度系统
     * @details 负责将待分配订单分配给合适的骑手
     *          内部调用 OrderMerger、DeliverSorter、PathFinder
     */
    class DispatchSystem {
    public:
        DispatchSystem() = default;
        ~DispatchSystem() = default;

        /**
         * @brief 处理所有待分配订单
         */
        void processOrders();

        /**
         * @brief 处理单个订单
         * @param orderId 订单ID
         * @return true 分配成功
         */
        bool processOrder(int orderId);

    private:
        /**
         * @brief 分配骑手给订单
         * @return 骑手ID，-1表示分配失败
         */
        int assignDeliver(int orderId);

        /**
         * @brief 计算路径
         */
        bool calculatePath(int orderId);
    };

} // namespace delivery