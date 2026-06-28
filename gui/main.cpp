#include <QApplication>
#include <QFile>
#include "MainWindow.h"
#include "../logic/utils/log_.h"

int main(int argc, char *argv[]) {
    system("chcp 65001");

    QApplication app(argc, argv);
    QApplication::setApplicationName("TinyDelivery");
    QApplication::setOrganizationName("NEU");

    // 加载样式表
    if (QFile styleFile(":/gui/styles.qss"); styleFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    MainWindow window;
    window.show();

    return QApplication::exec();
}
