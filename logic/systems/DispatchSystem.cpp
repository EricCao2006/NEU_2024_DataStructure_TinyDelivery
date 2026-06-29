#include "DispatchSystem.h"
#include "../utils/math_.h"
#include "../utils/log_.h"

namespace systems {

    void DispatchSystem::init(const models::CityGrid& grid) {
        m_grid = &grid;

        // 用地址匹配构建哈希表
        std::vector<std::tuple<std::string, int, int>> addresses;
        for (const auto& m : grid.getMerchants()) {
            addresses.emplace_back(m.getName().toStdString(), m.getX(), m.getY());
        }
        for (const auto& c : grid.getCustomers()) {
            addresses.emplace_back(c.getName().toStdString(), c.getX(), c.getY());
        }
        m_addressMatcher.buildTable(addresses);

        utils::Logger::log(1, "DispatchSystem initialized with " +
                           std::to_string(m_delivers.size()) + " delivers");
    }

    int DispatchSystem::createOrder(const int merchantId, const int customerId, const int vipLevel) {
        if (!m_grid) return -1;

        const auto* merchant = m_grid->findMerchant(merchantId);
        const auto* customer = m_grid->findCustomer(customerId);
        if (!merchant || !customer) return -1;

        // 用哈希表查找地址坐标（演示地址匹配功能）
        auto mPos = m_addressMatcher.lookup(merchant->getName().toStdString());
        auto cPos = m_addressMatcher.lookup(customer->getName().toStdString());
        if (mPos.first < 0 || cPos.first < 0) {
            // 回退到直接坐标
            mPos = {merchant->getX(), merchant->getY()};
            cPos = {customer->getX(), customer->getY()};
        }

        const int dist = utils::manhattanDistance(mPos.first, mPos.second,
                                            cPos.first, cPos.second);

        models::Order order(m_nextOrderId++, merchantId, customerId, dist);
        order.setVipLevel(vipLevel);
        order.calculateFee();

        m_orders.push_back(order);
        return order.getId();
    }

    void DispatchSystem::dispatchPendingOrders() {
        if (!m_grid) return;

        // 收集所有待派单订单
        std::vector<models::Order> pendingOrders;
        for (const auto& order : m_orders) {
            if (order.getStatus() == OrderStatus::Pending) {
                pendingOrders.push_back(order);
            }
        }
        if (pendingOrders.empty()) return;

        // 用稀疏多项式加法合并同路线订单
        auto merged = m_orderMerger.mergeOrders(pendingOrders);
        utils::Logger::log(0, "[Dispatch] Merged " + std::to_string(pendingOrders.size()) +
                           " orders into " + std::to_string(merged.size()) + " groups");

        // 对每个合并后的订单组分配骑手
        for (const auto& term : merged) {
            const int merchantId = term.exp.first;
            const int customerId = term.exp.second;

            // 找出空闲骑手
            std::vector<models::Deliver> available;
            for (const auto& d : m_delivers) {
                if (d.getOrderIds().empty() &&
                    d.getCurrentLoad() < d.getMaxCapacity()) {
                    available.push_back(d);
                }
            }
            if (available.empty()) continue;

            const auto* merchant = m_grid->findMerchant(merchantId);
            if (!merchant) continue;

            // 用堆排序选出最优骑手
            m_deliverSorter.heapSort(available, merchant->getX(), merchant->getY());

            // 分配最优骑手
            const int bestId = available[0].getId();
            for (auto& d : m_delivers) {
                if (d.getId() == bestId) {
                    // 找到对应的待派订单
                    for (auto& order : m_orders) {
                        if (order.getStatus() == OrderStatus::Pending &&
                            order.getMerchantId() == merchantId &&
                            order.getCustomerId() == customerId) {
                            d.addOrderId(order.getId());
                            order.setStatus(OrderStatus::Assigned);
                            order.setAssignTime(0);

                            // 用 PathFinder 规划路径（Dijkstra/A*）
                            auto path = m_pathFinder.findPathDijkstra(*m_grid, d.getX(), d.getY(),
                                                                      merchant->getX(), merchant->getY());
                            if (!path.empty()) {
                                utils::Logger::log(0, "[Dispatch] Path planned: " + std::to_string(path.size()) + " steps");
                                // 将路径存入 Deliver
                                for (auto& del : m_delivers) {
                                    if (del.getId() == bestId) {
                                        del.setPath(path);
                                        break;
                                    }
                                }
                            }

                            // 用哈夫曼编码压缩配送指令
                            std::string instruction = "Deliver:#" + std::to_string(order.getId()) +
                                                      " From:" + merchant->getName().toStdString() +
                                                      " To:" + std::to_string(customerId);
                            auto freqMap = algo::InstructionCompressor::buildFrequencyMap(instruction);
                            auto* tree = m_compressor.buildTree(freqMap);
                            auto codes = m_compressor.generateCodes(tree);
                            std::string compressed = m_compressor.compress(instruction, codes);
                            utils::Logger::log(0, "[Dispatch] Instruction compressed: " +
                                               std::to_string(instruction.size()) + " -> " +
                                               std::to_string(compressed.size()) + " chars");
                            delete tree;

                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    void DispatchSystem::tick(const double deltaSeconds) {
        if (!m_grid) return;

        for (int i = 0; i < static_cast<int>(m_delivers.size()); i++) {
            moveDeliver(i, deltaSeconds);
        }

        // 尝试派单
        dispatchPendingOrders();
    }

    void DispatchSystem::moveDeliver(const int deliverIdx, double /*deltaSeconds*/) {
        auto& deliver = m_delivers[deliverIdx];
        if (deliver.getOrderIds().empty()) return;

        const int orderId = deliver.getOrderIds().front();

        // 查找订单
        models::Order* order = nullptr;
        for (auto& o : m_orders) {
            if (o.getId() == orderId) { order = &o; break; }
        }
        if (!order) return;

        const auto* merchant = m_grid->findMerchant(order->getMerchantId());
        const auto* customer = m_grid->findCustomer(order->getCustomerId());
        if (!merchant || !customer) return;

        // 状态机
        if (order->getStatus() == OrderStatus::Assigned) {
            order->setStatus(OrderStatus::PickingUp);
        }

        if (order->getStatus() == OrderStatus::PickingUp) {
            // 向商家移动（沿规划路径）
            if (bool arrived = deliver.moveAlongPath()) {
                order->setStatus(OrderStatus::Delivering);
                deliver.addLoad(1);
                // 规划去客户的路径
                auto path = m_pathFinder.findPathDijkstra(*m_grid, deliver.getX(), deliver.getY(),
                                                          customer->getX(), customer->getY());
                if (!path.empty()) {
                    deliver.setPath(path);
                }
            }
        } else if (order->getStatus() == OrderStatus::Delivering) {
            // 向客户移动（沿规划路径）
            if (bool arrived = deliver.moveAlongPath()) {
                order->setStatus(OrderStatus::Completed);
                order->setCompleteTime(0);
                deliver.removeLoad(1);
                deliver.addEarnings(order->getDeliveryFee());
                deliver.incrementCompleted();
                deliver.removeOrderId(orderId);
                m_totalRevenue += order->getDeliveryFee();

                if (onOrderCompleted) onOrderCompleted(*order);
            }
        }
    }

    int DispatchSystem::getPendingCount() const {
        int cnt = 0;
        for (const auto& o : m_orders)
            if (o.getStatus() == OrderStatus::Pending) cnt++;
        return cnt;
    }

    int DispatchSystem::getDeliveringCount() const {
        int cnt = 0;
        for (const auto& o : m_orders)
            if (o.getStatus() == OrderStatus::PickingUp || o.getStatus() == OrderStatus::Delivering)
                cnt++;
        return cnt;
    }

    int DispatchSystem::getCompletedCount() const {
        int cnt = 0;
        for (const auto& o : m_orders)
            if (o.getStatus() == OrderStatus::Completed) cnt++;
        return cnt;
    }

}
