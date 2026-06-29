#include <QApplication>
#include <QDir>

#include "MainWindow.h"
#include "MapWidget.h"
#include "panels/ControlPanel.h"
#include "panels/MerchantPanel.h"
#include "panels/CustomerPanel.h"
#include "panels/DeliverPanel.h"
#include "EventPopup.h"

#include <QVBoxLayout>
#include <QDockWidget>
#include <qguiapplication.h>
#include <QMenuBar>
#include <QStatusBar>
#include <QDialog>
#include <QPushButton>

#include "../logic/utils/log_.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("迷你物流 TinyDelivery");
    // 默认全屏
    showMaximized();

    // 确保深色背景
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");

    // 创建引擎
    m_engine = new GameEngine(this);
    utils::Logger::log(0, "[MainWindow] Engine created");

    setupUI();
    utils::Logger::log(0, "[MainWindow] UI setup done");

    connectSignals();
    utils::Logger::log(0, "[MainWindow] Signals connected");

    // 计算配置目录路径（相对于可执行文件位置）
    QString exeDir = QApplication::applicationDirPath();
    QString configDir;
    // 如果在 cmake-build-debug 目录中，向上一级找 resource
    if (QDir(exeDir + "/resource/config").exists()) {
        configDir = exeDir + "/resource/config";
    } else if (QDir(exeDir + "/../resource/config").exists()) {
        configDir = exeDir + "/../resource/config";
    } else {
        configDir = exeDir + "/../resource/config";  // 默认回退
    }
    utils::Logger::log(0, "[MainWindow] Config dir: " + configDir.toStdString());

    // 初始化引擎（加载配置）
    m_engine->init(configDir);
    utils::Logger::log(0, "[MainWindow] Engine init done");

    // 初始化地图
    m_mapWidget->setGrid(&m_engine->grid());
    utils::Logger::log(0, "[MainWindow] Grid set to MapWidget");

    // 招募初始骑手
    bool h1 = m_engine->hireDeliver("小张");
    utils::Logger::log(0, std::string("[MainWindow] Hire 1: ") + (h1 ? "OK" : "FAIL"));
    bool h2 = m_engine->hireDeliver("小李");
    utils::Logger::log(0, std::string("[MainWindow] Hire 2: ") + (h2 ? "OK" : "FAIL"));
    bool h3 = m_engine->hireDeliver("小王");
    utils::Logger::log(0, std::string("[MainWindow] Hire 3: ") + (h3 ? "OK" : "FAIL"));

    // 启动游戏循环
    m_engine->start();
    utils::Logger::log(0, "[MainWindow] Engine started");
}

MainWindow::~MainWindow() {
    if (m_engine) {
        m_engine->printAlgorithmStats();
    }
}

void MainWindow::setupUI() {
    // 中心：地图
    m_mapWidget = new MapWidget(this);
    setCentralWidget(m_mapWidget);

    // 左侧：调控中心
    m_controlPanel = new ControlPanel(m_engine, this);
    auto* controlDock = new QDockWidget("调控中心", this);
    controlDock->setWidget(m_controlPanel);
    controlDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::LeftDockWidgetArea, controlDock);
    controlDock->setMinimumWidth(280);  // 确保不折叠

    // 右侧上：商家面板
    m_merchantPanel = new MerchantPanel(m_engine, this);
    auto* merchantDock = new QDockWidget("商家列表", this);
    merchantDock->setWidget(m_merchantPanel);
    merchantDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, merchantDock);
    merchantDock->setMinimumWidth(320);  // 确保不折叠

    // 右侧中：客户面板
    m_customerPanel = new CustomerPanel(m_engine, this);
    auto* customerDock = new QDockWidget("客户列表", this);
    customerDock->setWidget(m_customerPanel);
    customerDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, customerDock);
    customerDock->setMinimumWidth(320);  // 确保不折叠

    // 右侧下：骑手面板
    m_deliverPanel = new DeliverPanel(m_engine, this);
    auto* deliverDock = new QDockWidget("骑手状态", this);
    deliverDock->setWidget(m_deliverPanel);
    deliverDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, deliverDock);
    deliverDock->setMinimumWidth(320);  // 确保不折叠

    // 消除 Dock 间距
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // 事件弹窗
    m_eventPopup = new EventPopup(this);

    // 菜单栏
    auto* fileMenu = menuBar()->addMenu("文件");
    fileMenu->addAction("保存存档", m_engine, [this]() { m_engine->saveGame("resource/save/save_000.json"); });
    fileMenu->addSeparator();
    fileMenu->addAction("退出", qApp, &QApplication::quit);

    auto* viewMenu = menuBar()->addMenu("视图");
    viewMenu->addAction("暂停/继续", [this]() {
        if (m_engine->isPaused()) m_engine->resume();
        else m_engine->pause();
    });

    // 状态栏
    statusBar()->showMessage("欢迎来到迷你物流！");
}

void MainWindow::connectSignals() {
    connect(m_engine, &GameEngine::tickUpdated, this, &MainWindow::onTickUpdated);
    connect(m_engine, &GameEngine::eventTriggered, this, &MainWindow::onEventTriggered);
    connect(m_engine, &GameEngine::orderCreated, this, &MainWindow::onOrderCreated);
    connect(m_engine, &GameEngine::orderCompleted, this, &MainWindow::onOrderCompleted);
    connect(m_engine, &GameEngine::levelUp, this, &MainWindow::onLevelUp);
}

void MainWindow::onTickUpdated() {
    m_controlPanel->refresh();
    m_merchantPanel->refresh();
    m_customerPanel->refresh();
    m_deliverPanel->refresh();
    m_mapWidget->setDelivers(&m_engine->dispatch().getDelivers());
    m_mapWidget->update();
}

void MainWindow::onEventTriggered(const QString& title, const QString& desc) const {
    m_eventPopup->showEvent(title, desc);
    statusBar()->showMessage("事件: " + title, 5000);
}

void MainWindow::onOrderCreated(const int orderId) const {
    statusBar()->showMessage("新订单 #" + QString::number(orderId) + " 已创建", 3000);
}

void MainWindow::onOrderCompleted(const int orderId) const {
    statusBar()->showMessage("订单 #" + QString::number(orderId) + " 配送完成！", 3000);
}

void MainWindow::onLevelUp(const int newLevel, const QString& newName) {
    // 自定义深色升级弹窗
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("升级成功！");
    dialog->setFixedSize(350, 180);
    dialog->setStyleSheet(R"(
        QDialog {
            background-color: #16213e;
            border-radius: 12px;
        }
        QLabel {
            color: #e0e0e0;
            font-size: 14px;
        }
        QPushButton {
            background-color: #e94560;
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 8px 24px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #ff6b8a;
        }
    )");

    auto* layout = new QVBoxLayout(dialog);
    layout->setSpacing(16);

    auto* titleLabel = new QLabel("🎉 恭喜升级！", dialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e94560;");
    layout->addWidget(titleLabel);

    auto* descLabel = new QLabel("公司升级为: " + newName + "\n等级: " + QString::number(newLevel), dialog);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    auto* btn = new QPushButton("太棒了！", dialog);
    connect(btn, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(btn, 0, Qt::AlignCenter);

    dialog->exec();
    delete dialog;
}
