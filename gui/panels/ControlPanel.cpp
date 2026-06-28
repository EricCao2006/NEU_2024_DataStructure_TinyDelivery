#include "ControlPanel.h"
#include "../../logic/GameEngine.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

ControlPanel::ControlPanel(GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    auto* layout = new QVBoxLayout(this);

    // ── 公司信息 ──
    auto* companyGroup = new QGroupBox("公司信息", this);
    auto* companyLayout = new QFormLayout(companyGroup);

    m_levelLabel = new QLabel(this);
    companyLayout->addRow("等级:", m_levelLabel);

    m_moneyLabel = new QLabel(this);
    companyLayout->addRow("资金:", m_moneyLabel);

    m_reputationLabel = new QLabel(this);
    companyLayout->addRow("声望:", m_reputationLabel);

    m_upgradeBtn = new QPushButton("升级公司", this);
    connect(m_upgradeBtn, &QPushButton::clicked, [this]() { m_engine->tryUpgrade(); });
    companyLayout->addRow(m_upgradeBtn);

    layout->addWidget(companyGroup);

    // ── 订单统计 ──
    auto* statsGroup = new QGroupBox("订单统计", this);
    auto* statsLayout = new QFormLayout(statsGroup);

    m_pendingLabel = new QLabel(this);
    statsLayout->addRow("待接单:", m_pendingLabel);

    m_deliveringLabel = new QLabel(this);
    statsLayout->addRow("配送中:", m_deliveringLabel);

    m_completedLabel = new QLabel(this);
    statsLayout->addRow("已完成:", m_completedLabel);

    m_revenueLabel = new QLabel(this);
    statsLayout->addRow("总收入:", m_revenueLabel);

    layout->addWidget(statsGroup);

    // ── 操作区 ──
    auto* actionGroup = new QGroupBox("操作", this);
    auto* actionLayout = new QFormLayout(actionGroup);

    m_merchantCombo = new QComboBox(this);
    actionLayout->addRow("商家:", m_merchantCombo);

    m_customerCombo = new QComboBox(this);
    actionLayout->addRow("客户:", m_customerCombo);

    m_orderBtn = new QPushButton("手动下单", this);
    connect(m_orderBtn, &QPushButton::clicked, [this]() {
        int mIdx = m_merchantCombo->currentIndex();
        int cIdx = m_customerCombo->currentIndex();
        if (mIdx >= 0 && cIdx >= 0) {
            m_engine->createOrder(mIdx, cIdx);
        }
    });
    actionLayout->addRow(m_orderBtn);

    m_pauseBtn = new QPushButton("暂停", this);
    connect(m_pauseBtn, &QPushButton::clicked, [this]() {
        if (m_engine->isPaused()) {
            m_engine->resume();
            m_pauseBtn->setText("暂停");
        } else {
            m_engine->pause();
            m_pauseBtn->setText("继续");
        }
    });
    actionLayout->addRow(m_pauseBtn);

    layout->addWidget(actionGroup);
    layout->addStretch();
}

void ControlPanel::refresh() const {
    const auto& upg = m_engine->upgrade();
    m_levelLabel->setText(QString::number(upg.getLevel()) + " - " + upg.getCurrentLevelConfig().name);
    m_moneyLabel->setText(QString::number(upg.getMoney(), 'f', 0) + " 元");
    m_reputationLabel->setText(QString::number(upg.getReputation(), 'f', 1));
    m_upgradeBtn->setEnabled(upg.canUpgrade());

    const auto& disp = m_engine->dispatch();
    m_pendingLabel->setText(QString::number(disp.getPendingCount()));
    m_deliveringLabel->setText(QString::number(disp.getDeliveringCount()));
    m_completedLabel->setText(QString::number(disp.getCompletedCount()));
    m_revenueLabel->setText(QString::number(disp.getTotalRevenue(), 'f', 0) + " 元");

    // 更新下拉框
    if (m_merchantCombo->count() == 0) {
        for (const auto& m : m_engine->grid().getMerchants()) {
            m_merchantCombo->addItem(m.getName());
        }
        for (const auto& c : m_engine->grid().getCustomers()) {
            m_customerCombo->addItem(c.getName());
        }
    }
}
