#include "MapWidget.h"
#include <QPainter>

MapWidget::MapWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(720, 720);
}

void MapWidget::paintEvent(QPaintEvent*) {
    if (!m_grid) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int size = m_grid->getSize();
    int totalPx = size * m_tileSize;

    // 绘制网格
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            QRect rect(x * m_tileSize, y * m_tileSize, m_tileSize, m_tileSize);
            TileType tile = m_grid->getTileType(x, y);
            painter.fillRect(rect, colorForTile(tile));
            painter.setPen(QColor(200, 200, 200));
            painter.drawRect(rect);
        }
    }

    // 绘制商家标记
    painter.setPen(Qt::black);
    painter.setFont(QFont("Microsoft YaHei", 8, QFont::Bold));
    for (const auto& m : m_grid->getMerchants()) {
        QPoint center(m.getX() * m_tileSize + m_tileSize / 2,
                      m.getY() * m_tileSize + m_tileSize / 2);
        painter.setPen(Qt::white);
        painter.drawText(center, m.getName().left(2));
    }

    // 绘制客户标记
    for (const auto& c : m_grid->getCustomers()) {
        QPoint center(c.getX() * m_tileSize + m_tileSize / 2,
                      c.getY() * m_tileSize + m_tileSize / 2);
        painter.setPen(QColor(0, 100, 200));
        painter.drawText(center, c.getName().left(2));
    }
}

QColor MapWidget::colorForTile(const TileType type) {
    switch (type) {
        case TileType::Road:      return QColor(240, 240, 240);
        case TileType::Obstacle:  return QColor(120, 120, 120);
        case TileType::Merchant:  return QColor(255, 160, 60);
        case TileType::Customer:  return QColor(80, 180, 255);
        case TileType::Warehouse: return QColor(100, 200, 100);
        default:                  return Qt::white;
    }
}
