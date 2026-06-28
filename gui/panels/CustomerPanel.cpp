#include "CustomerPanel.h"
#include "../../logic/GameEngine.h"

#include <QVBoxLayout>
#include <QHeaderView>

CustomerPanel::CustomerPanel(GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    auto* layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"名称", "VIP", "位置", "订单数", "满意度"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(m_table);
}

void CustomerPanel::refresh() const {
    const auto& customers = m_engine->grid().getCustomers();
    m_table->setRowCount(static_cast<int>(customers.size()));

    for (int i = 0; i < static_cast<int>(customers.size()); i++) {
        const auto& c = customers[i];
        m_table->setItem(i, 0, new QTableWidgetItem(c.getName()));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(c.getVipLevel())));
        m_table->setItem(i, 2, new QTableWidgetItem(
            QString("(%1,%2)").arg(c.getX()).arg(c.getY())));
        m_table->setItem(i, 3, new QTableWidgetItem(
            QString::number(c.getTotalOrders())));
        m_table->setItem(i, 4, new QTableWidgetItem(
            QString::number(c.getSatisfaction(), 'f', 1) + "%"));
    }
}
