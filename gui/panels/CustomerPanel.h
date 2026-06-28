#pragma once

#include <QTableWidget>

class GameEngine;

class CustomerPanel : public QWidget {
    Q_OBJECT
public:
    explicit CustomerPanel(GameEngine* engine, QWidget* parent = nullptr);
    void refresh() const;

private:
    GameEngine* m_engine = nullptr;
    QTableWidget* m_table = nullptr;
};
