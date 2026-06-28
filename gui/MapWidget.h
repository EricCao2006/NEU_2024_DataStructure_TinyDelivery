#pragma once

#include <QWidget>
#include <QColor>
#include "../logic/models/CityGrid.h"

class MapWidget : public QWidget {
    Q_OBJECT
public:
    explicit MapWidget(QWidget* parent = nullptr);

    void setGrid(const models::CityGrid* grid) { m_grid = grid; update(); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    const models::CityGrid* m_grid = nullptr;
    int m_tileSize = 36;

    static QColor colorForTile(TileType type) ;
};
