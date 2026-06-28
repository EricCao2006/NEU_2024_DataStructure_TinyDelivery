#pragma once

#include "../algorithms/PathFinder.h"
#include "../algorithms/DeliverSorter.h"
#include "../algorithms/OrderMerger.h"
#include "../algorithms/AddressMatcher.h"
#include "../algorithms/InstructionCompressor.h"

#include <vector>
#include <functional>

namespace systems {

    /**
     * @brief 调度系统 —— 订单全生命周期管理
     */
    class DispatchSystem {
    public:
        DispatchSystem() = default;

        void init(const models::CityGrid& grid);

        /// 创建新订单
        int createOrder(int merchantId, int customerId, int vipLevel);

        /// 为待接单的订单分配骑手
        void dispatchPendingOrders();

        /// 每帧更新：骑手移动、到达判定
        void tick(double deltaSeconds);

        /// 获取所有订单
        const std::vector<models::Order>& getOrders() const { return m_orders; }

        /// 获取所有骑手
        const std::vector<models::Deliver>& getDelivers() const { return m_delivers; }
        std::vector<models::Deliver>& getDeliversMut() { return m_delivers; }

        /// 添加骑手
        void addDeliver(const models::Deliver& d) { m_delivers.push_back(d); }

        /// 获取待接单订单数
        int getPendingCount() const;

        /// 获取配送中订单数
        int getDeliveringCount() const;

        /// 获取已完成订单数
        int getCompletedCount() const;

        /// 获取总收入
        double getTotalRevenue() const { return m_totalRevenue; }

        // 信号回调：订单完成时触发
        std::function<void(const models::Order&)> onOrderCompleted;
        // 信号回调：骑手到达商家
        std::function<void(int deliverId, int orderId)> onDeliverArriveMerchant;

    private:
        std::vector<models::Order> m_orders;
        std::vector<models::Deliver> m_delivers;
        const models::CityGrid* m_grid = nullptr;

        algo::PathFinder m_pathFinder;
        algo::DeliverSorter m_deliverSorter;
        algo::OrderMerger m_orderMerger;
        algo::AddressMatcher m_addressMatcher;
        algo::InstructionCompressor m_compressor;

        int m_nextOrderId = 1;
        double m_totalRevenue = 0.0;

        /// 骑手移动一步
        void moveDeliver(int deliverIdx, double deltaSeconds);

    public:
        /// 获取算法引用（用于统计）
        const algo::PathFinder& pathFinder() const { return m_pathFinder; }
        const algo::DeliverSorter& deliverSorter() const { return m_deliverSorter; }
        const algo::OrderMerger& orderMerger() const { return m_orderMerger; }
        const algo::AddressMatcher& addressMatcher() const { return m_addressMatcher; }
        const algo::InstructionCompressor& compressor() const { return m_compressor; }
    };

}
