#pragma once

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>

class GameEngine;

class ControlPanel : public QWidget {
    Q_OBJECT
public:
    explicit ControlPanel(GameEngine* engine, QWidget* parent = nullptr);
    void refresh() const;

private:
    GameEngine* m_engine = nullptr;

    QLabel* m_levelLabel = nullptr;
    QLabel* m_moneyLabel = nullptr;
    QLabel* m_reputationLabel = nullptr;
    QLabel* m_pendingLabel = nullptr;
    QLabel* m_deliveringLabel = nullptr;
    QLabel* m_completedLabel = nullptr;
    QLabel* m_revenueLabel = nullptr;

    QPushButton* m_upgradeBtn = nullptr;
    QPushButton* m_pauseBtn = nullptr;
    QPushButton* m_orderBtn = nullptr;
    QComboBox* m_merchantCombo = nullptr;
    QComboBox* m_customerCombo = nullptr;
    QSpinBox* m_speedSpin = nullptr;
};
