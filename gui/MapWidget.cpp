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
            painter.setPen(QColor(60, 70, 90));  // 深色网格线
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
        painter.setPen(QColor(0, 150, 255));  // 亮蓝客户
        painter.drawText(center, c.getName().left(2));
    }

    // 绘制骑手（红色圆点 + 姓名）
    if (m_delivers) {
        for (const auto& d : *m_delivers) {
            int cx = d.getX() * m_tileSize + m_tileSize / 2;
            int cy = d.getY() * m_tileSize + m_tileSize / 2;
            // 红色圆点
            painter.setBrush(QColor(220, 50, 50));
            painter.setPen(QPen(Qt::white, 2));
            painter.drawEllipse(QPoint(cx, cy), m_tileSize / 3, m_tileSize / 3);
            // 姓名
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QColor(220, 50, 50));
            painter.setFont(QFont("Microsoft YaHei", 7, QFont::Bold));
            painter.drawText(QPoint(cx, cy - m_tileSize / 3 - 2), d.getName().left(2));
        }
    }
}

QColor MapWidget::colorForTile(const TileType type) {
    switch (type) {
        case TileType::Road:      return QColor(45, 52, 70);      // 深灰蓝路面
        case TileType::Obstacle:  return QColor(30, 35, 50);      // 深色障碍
        case TileType::Merchant:  return QColor(233, 69, 96);     // 玫红商家
        case TileType::Customer:  return QColor(100, 180, 255);   // 亮蓝客户
        case TileType::Warehouse: return QColor(80, 200, 120);    // 绿色仓库
        default:                  return QColor(45, 52, 70);
    }
}
