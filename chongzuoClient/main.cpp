#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //解决中文显示问题(没用啊）
   // QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
   // QTextCodec::setCodecForLocale(codec);

    MainWindow w;
    w.show();
    return a.exec();
}
