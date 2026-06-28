#pragma once

#include <QTableWidget>

class GameEngine;

class MerchantPanel : public QWidget {
    Q_OBJECT
public:
    explicit MerchantPanel(GameEngine* engine, QWidget* parent = nullptr);
    void refresh() const;

private:
    GameEngine* m_engine = nullptr;
    QTableWidget* m_table = nullptr;
};
