#include "EventPopup.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

EventPopup::EventPopup(QWidget* parent) : QDialog(parent) {
    setWindowTitle("随机事件");
    setFixedSize(350, 180);

    auto* layout = new QVBoxLayout(this);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e94560;");
    layout->addWidget(m_titleLabel);

    m_descLabel = new QLabel(this);
    m_descLabel->setAlignment(Qt::AlignCenter);
    m_descLabel->setWordWrap(true);
    m_descLabel->setStyleSheet("font-size: 14px; color: #c0c0d0;");
    layout->addWidget(m_descLabel);

    auto* btn = new QPushButton("知道了", this);
    connect(btn, &QPushButton::clicked, this, &QDialog::hide);
    layout->addWidget(btn, 0, Qt::AlignCenter);

    // 5秒后自动关闭
    QTimer* autoClose = new QTimer(this);
    connect(autoClose, &QTimer::timeout, this, &QDialog::hide);
    autoClose->setSingleShot(true);
    autoClose->start(5000);
}

void EventPopup::showEvent(const QString& title, const QString& desc) {
    m_titleLabel->setText(title);
    m_descLabel->setText(desc);
    QDialog::show();
}
