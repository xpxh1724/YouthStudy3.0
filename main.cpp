#include "mainwidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
int main(int argc, char *argv[])
{
    //永远不应用高分屏及缩放
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
    QApplication a(argc, argv);
    //①连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sql.db"); //如果本目录下没有该文件,则会在本目录下生成,否则连接该文件
    if (!db.open()) {
        QMessageBox::warning(nullptr, QObject::tr("打开数据库失败"),db.lastError().text());
        return 1;
    }
    mainWidget w;
    return a.exec();
}
