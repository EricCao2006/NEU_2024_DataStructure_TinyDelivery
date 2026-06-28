#pragma once

#include <QDialog>
#include <QLabel>

class EventPopup : public QDialog {
    Q_OBJECT
public:
    explicit EventPopup(QWidget* parent = nullptr);
    void showEvent(const QString& title, const QString& desc);

private:
    QLabel* m_titleLabel = nullptr;
    QLabel* m_descLabel = nullptr;
};
