#include "MerchantPanel.h"
#include "../../logic/GameEngine.h"

#include <QVBoxLayout>
#include <QHeaderView>

MerchantPanel::MerchantPanel(GameEngine* engine, QWidget* parent)
    : QWidget(parent), m_engine(engine)
{
    auto* layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"名称", "类型", "位置", "订单数"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(m_table);
}

void MerchantPanel::refresh() const {
    const auto& merchants = m_engine->grid().getMerchants();
    m_table->setRowCount(static_cast<int>(merchants.size()));

    static const char* typeNames[] = {"餐饮", "服饰", "零售"};

    for (int i = 0; i < static_cast<int>(merchants.size()); i++) {
        const auto& m = merchants[i];
        m_table->setItem(i, 0, new QTableWidgetItem(m.getName()));
        m_table->setItem(i, 1, new QTableWidgetItem(typeNames[static_cast<int>(m.getType())]));
        m_table->setItem(i, 2, new QTableWidgetItem(
            QString("(%1,%2)").arg(m.getX()).arg(m.getY())));
        m_table->setItem(i, 3, new QTableWidgetItem(
            QString::number(m.getTotalOrders())));
    }
}
