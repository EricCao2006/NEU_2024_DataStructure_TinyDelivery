#pragma once

#include <QWidget>
#include <QColor>
#include "../logic/models/CityGrid.h"
#include "../logic/models/Deliver.h"

class MapWidget : public QWidget {
    Q_OBJECT
public:
    explicit MapWidget(QWidget* parent = nullptr);

    void setGrid(const models::CityGrid* grid) { m_grid = grid; update(); }
    void setDelivers(const std::vector<models::Deliver>* delivers) { m_delivers = delivers; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    const models::CityGrid* m_grid = nullptr;
    const std::vector<models::Deliver>* m_delivers = nullptr;
    int m_tileSize = 36;

    static QColor colorForTile(TileType type) ;
};
