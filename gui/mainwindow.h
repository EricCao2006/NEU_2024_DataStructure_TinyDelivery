#pragma once

#include <QMainWindow>
#include "../logic/GameEngine.h"

class MapWidget;
class ControlPanel;
class MerchantPanel;
class CustomerPanel;
class DeliverPanel;
class EventPopup;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onTickUpdated();
    void onEventTriggered(const QString& title, const QString& desc) const;
    void onOrderCreated(int orderId) const;
    void onOrderCompleted(int orderId) const;
    void onLevelUp(int newLevel, const QString& newName);

private:
    GameEngine* m_engine = nullptr;

    // 中心区域
    MapWidget* m_mapWidget = nullptr;

    // 面板
    ControlPanel* m_controlPanel = nullptr;
    MerchantPanel* m_merchantPanel = nullptr;
    CustomerPanel* m_customerPanel = nullptr;
    DeliverPanel* m_deliverPanel = nullptr;

    // 弹窗
    EventPopup* m_eventPopup = nullptr;

    void setupUI();
    void connectSignals();
};
