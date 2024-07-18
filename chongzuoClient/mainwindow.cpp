#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QJsonDocument>

#include<QFileDialog>
#include<QFile>
#include<QDebug>
#include<QFileInfo>
#include<QTimer>
#include<QMessageBox>
#include <QDataStream>
#include <QDir>
#include <QDesktopServices>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpclient=new QTcpSocket(this);
    setWindowTitle("TCP Client");

        connect(tcpclient,SIGNAL(connected()),this,SLOT(connectfunc()));
        connect(tcpclient,SIGNAL(disconnected()),this,SLOT(disconnectfunc()));
        //connect(tcpclient,SIGNAL(readyRead()),this,SLOT(socketreaddata()));
    //isStart = true;也许改成点击发送文件后再开启？
        // ui->progressBar->setMinimum(0);
       //QByteArray buf;
        connect(tcpclient,&QTcpSocket::readyRead,[=](){
                QByteArray buf= tcpclient->readAll();
               if(true==isStart)//代表发送过来的是文件信息
                {
                    qDebug()<<"filename:"<<fileName;
                    qDebug()<<"fileSize:"<<fileSize;
                    qDebug()<< ".............readServerMsg................";
                    isStart=false;
                     recvSize = 0;
                   // tcpclient->write(buf);
                    //解析头部信息 QString buf = "hello##1024"
                    //QString str = "hello##1024##mike";
                    //str.section("##",0,0);//以##进行拆分
                       QString temp=QString(buf);
                                //初始化
                                //fileName = QString(buf).section("##",0,0);
                                //fileSize = QString(buf).section("##",1,1).toInt();
                                fileName = QString(buf).section("#",1,1);
                                fileSize = QString(buf).section("#",2,2).toInt();
                                qDebug()<<"filename:"<<fileName;
                                qDebug()<<"fileSize:"<<fileSize;


                                //打开文件
                                file.setFileName(fileName);

                                bool isOK = file.open(QIODevice::WriteOnly);
                                //bool isOK = file.open(QIODevice::ReadWrite);
                                if(false == isOK)
                                {
                                    //qDebug()<<"filename:"<<fileName;
                                    //qDebug()<<"fileSize:"<<fileSize;
                                    qDebug()<<"WriteOnly error 40";
                                }
                    }

                            else//文件信息
                            {
                                qint64 len = file.write(buf);
                                if(len>0){
                                    recvSize=0;
                                    recvSize  += len;
                                    qDebug()<<len;
                                    //fileSize=recvSize;
                                }
                                qDebug()<<"filename:"<<fileName;
                                qDebug()<<"fileSize:"<<fileSize;
                                qDebug()<<"recvSize:"<<recvSize;
                                //fileSize=recvSize;
                                if(recvSize==fileSize)
                                {
                                    qDebug()<<"filename:"<<fileName;
                                    qDebug()<<"fileSize:"<<fileSize;
                                    qDebug()<<"recvSize:"<<recvSize;

                                     ui->plainTextEdit_DispMsg->appendPlainText("wenjian recv success");
                                    file.close();
                                    QMessageBox::information(this,"ok","wenjian recv success");
                                     ui->plainTextEdit_DispMsg->appendPlainText("recvclient:"+buf);
                                    qDebug()<<"wenjian recv success";
                                    tcpclient->disconnectFromHost();
                                    tcpclient->close();
                                }
                            }
                //取出接收的内容
                ui->plainTextEdit_DispMsg->appendPlainText("fuwuduan:"+buf);

});
       //timer=new QTimer(this);
        connect(&timer,&QTimer::timeout,[=](){
                //关闭定时器
                this->timer.stop();

                //发送文件
                sendData();
            }
        );


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_Connect_clicked()
{
   // tcpclient=tcpServer->nextPendingConnection();
    QString addr=ui->comboBoxIp->currentText();
    quint16 port=ui->spinBoxPort->value();
    tcpclient->abort();
    tcpclient->connectToHost(addr,port);

    //成功连接后，才可以选择文件
    ui->buttonFile->setEnabled(true);
}


void MainWindow::on_pushButton_Send_clicked()
{
    QString strmsg=ui->lineEdit_InputMsg->text();
    ui->plainTextEdit_DispMsg->appendPlainText("[out]:"+strmsg);
    ui->lineEdit_InputMsg->clear();

    QByteArray str=strmsg.toUtf8();
    str.append('\n');
    tcpclient->write(str);
}

void MainWindow::on_pushButton_Disconnect_clicked()
{
    //tcpclient->abort();
    if(tcpclient->state()==QAbstractSocket::ConnectedState)
        tcpclient->disconnectFromHost();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(tcpclient->state()==QAbstractSocket::ConnectedState)
    {
        tcpclient->disconnectFromHost();
    }
    event->accept();

}

void MainWindow::connectfunc()
{
    ui->plainTextEdit_DispMsg->appendPlainText("**********lianjie server**********");
    ui->plainTextEdit_DispMsg->appendPlainText("**********peer address:"+
                                               tcpclient->peerAddress().toString());
    ui->plainTextEdit_DispMsg->appendPlainText("**********peer port:"+
                                               QString::number(tcpclient->peerPort()));

    ui->pushButton_Connect->setEnabled(false);
    ui->pushButton_Disconnect->setEnabled(true);

}
void MainWindow::disconnectfunc()
{
    ui->plainTextEdit_DispMsg->appendPlainText("********** 断开服务器 duankai server**********");

    ui->pushButton_Connect->setEnabled(true);
    ui->pushButton_Disconnect->setEnabled(false);

}
/*void MainWindow::socketreaddata()
{
   /* while(tcpclient->canReadLine())
        ui->plainTextEdit_DispMsg->appendPlainText("[in]:"+tcpclient->readLine());*/
    //也许上面的可以让发送的信息后面没乱码？
    //tcpclient->waitForReadyRead();
    /*QByteArray buffer=tcpclient->readAll();
        ui->plainTextEdit_DispMsg->appendPlainText("[fuwuduan]:"+buffer);


}*/

void MainWindow::on_buttonFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),"../");
            fileName.clear();
            fileSize = 0;//初始化文件数据大小

            //获取文件信息
            QFileInfo info(filePath);
            fileName = info.fileName();//获取文件名字
            fileSize = info.size();//获取文件大小
            // 接收文件大小，数据总大小信息和文件名大小,文件名信息

            sendSize = 0;//发送文件的大小

            //只读方式打开
            //指定文件的名字
            if(!filePath.isEmpty()){//如果选择文件路径有效

            file.setFileName(filePath);

            //打开文件
            bool isOK = file.open(QIODevice::ReadOnly);
            if(false == isOK)//文件未成功打开
            {
                qDebug()<<"只读方式错误 77";
            }

            //提示打开文件的路径
            qDebug()<<"filename:"<<fileName;
            qDebug()<<"fileSize:"<<fileSize;
            ui->plainTextEdit_DispMsg->appendPlainText(filePath);
            ui->buttonFile->setEnabled(false);
            ui->buttonSend->setEnabled(true);
        }
        else
            {
                qDebug()<<"选择文件路径出错 62";
            }

}

void MainWindow::on_buttonSend_clicked()
{
    //先发送文件头信息
       // QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    QString head = QString("head#%1#%2").arg(fileName).arg(fileSize);
        //发送头部信息
        qint64 len = tcpclient->write(head.toUtf8().data());
        tcpclient->waitForBytesWritten();//等待数据发送完毕
        ui->plainTextEdit_DispMsg->appendPlainText("yijingzai send wenjian");
        if(len > 0)//头部信息发送成功
        {
            //发送真正的文件信息
           sendData();
            //防止TCP黏包问题
            //需要通过定时器延时20 ms
            this->timer.start(1000);//间隔10ms触发timeout

        }
        else
        {
            qDebug()<<"toubuwenjian send lost 110";
            file.close();//关闭文件
            ui->buttonFile->setEnabled(true);
            ui->buttonSend->setEnabled(false);
        }
    isStart = true;
     ui->buttonSend->setEnabled(true);
}

void MainWindow::sendData()
{
    qint64 len = 0;
    //sendSize=0;
    //循环，直至发送文件完毕为止
        do
        {

            char buf[4*1024] = {0};//缓冲区
            len = 0;//初始化为0

            //读文件内容数据读了多少
            len = file.read(buf,sizeof(buf));
            //发送数据，读多少，发多少
            len =tcpclient->write(buf,len);

            //发送的数据需要积累
            sendSize += len;//已发送多少文件内容
            qDebug()<<"sendSize="<<sendSize;
        }while(len > 0);//只要有发送，就一直循环

        //是否发送文件完毕
        if(sendSize == fileSize)//发送大小等于文件大小
        {
            ui->plainTextEdit_DispMsg->appendPlainText("wenjian send wangbi");
            file.close();

            /*//断开服务器
            tcpclient->disconnectFromHost();//断开套接字
            tcpclient->close();*/
        }


}


