#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QHostInfo>
#include<QFile>
#include<QTimer>
#include<QObject>
#include<QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendData();//发送文件数据
    void fileDataRead();//
private:
    Ui::MainWindow *ui;
private:
    QTcpSocket *tcpclient;
    QFile file;//文件对象
    QString fileName;//文件名字
    qint64 fileSize;//文件大小
    qint64 sendSize;//已经发送文件大小
    qint64 recvSize;//已经接收文件大小

    QTimer timer;//定时器


    bool isStart;//判断是否是文件内容
    QString m_downloadPath;//下载路径

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void connectfunc();
    void disconnectfunc();
   // void socketreaddata();
    void on_pushButton_Connect_clicked();
    void on_pushButton_Send_clicked();
    void on_pushButton_Disconnect_clicked();

    void on_buttonFile_clicked();
    void on_buttonSend_clicked();

};
#endif // MAINWINDOW_H
