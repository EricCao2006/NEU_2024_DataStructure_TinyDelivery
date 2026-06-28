#include "DeliverPanel.h"
#include "../../logic/GameEngine.h"

#include <QVBoxLayout>
#include <QHeaderView>

DeliverPanel::DeliverPanel(GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    auto* layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"姓名", "位置", "速度", "载量", "完成单数", "收入"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(m_table);
}

void DeliverPanel::refresh() const {
    const auto& delivers = m_engine->dispatch().getDelivers();
    m_table->setRowCount(static_cast<int>(delivers.size()));

    for (int i = 0; i < static_cast<int>(delivers.size()); i++) {
        const auto& d = delivers[i];
        m_table->setItem(i, 0, new QTableWidgetItem(d.getName()));
        m_table->setItem(i, 1, new QTableWidgetItem(
            QString("(%1,%2)").arg(d.getX()).arg(d.getY())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(d.getSpeed())));
        m_table->setItem(i, 3, new QTableWidgetItem(
            QString("%1/%2").arg(d.getCurrentLoad()).arg(d.getMaxCapacity())));
        m_table->setItem(i, 4, new QTableWidgetItem(
            QString::number(d.getCompletedOrders())));
        m_table->setItem(i, 5, new QTableWidgetItem(
            QString::number(d.getTotalEarnings(), 'f', 0)));
    }
}
