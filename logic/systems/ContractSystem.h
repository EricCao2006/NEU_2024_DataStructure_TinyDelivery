#pragma once
/**
 * @file ContractSystem.h
 * @brief 客户合同系统
 */

#include "../GameState.h"
#include <vector>

namespace delivery {

    /**
     * @brief 合同状态
     */
    enum class ContractStatus : uint8_t {
        ACTIVE,      ///< 有效
        EXPIRED,     ///< 已过期
        RENEWING     ///< 续约中
    };

    /**
     * @brief 合同信息
     */
    struct Contract {
        int customerId{};
        ContractStatus status = ContractStatus::ACTIVE;
        int startTime = 0;
        int duration = 3600;           ///< 合同时长（秒），默认1小时
        int renewalCost = 500;         ///< 续约费用
    };

    class ContractSystem {
    public:
        ContractSystem() = default;
        ~ContractSystem() = default;

        /**
         * @brief 检查所有合同状态
         */
        void update();

        /**
         * @brief 获取客户当前合同
         */
        Contract* getContract(int customerId);

        /**
         * @brief 续约合同
         */
        bool renewContract(int customerId);

        /**
         * @brief 计算续约费用
         */
        [[nodiscard]] int calculateRenewalCost(int customerId) const;

    private:
        std::vector<Contract> m_contracts;
    };

} // namespace delivery