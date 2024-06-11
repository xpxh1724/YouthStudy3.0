#include "psmtp.h"

PSmtp::PSmtp(QString serverIP,QObject *parent) : QObject(parent)
{
    this->serverIP = serverIP;
    tcpSocket = new QTcpSocket;
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connectToServer()));//连接成功
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectFromServer()));//断开连接
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getMessage()));//处理服务器消息
    qDebug()<<"PSmtp";
}

PSmtp::~PSmtp()
{
    delete tcpSocket;
    qDebug()<<"~PSmtp";
}

void PSmtp::sendEmail(QString username, QString password, QString to, QString title, QString text, QString ip)
{
    if(ip != "")
    {
        serverIP = ip;
    }
    this->username = username;
    this->password = password;
    this->to = to;
    this->title = title;
    this->text = text;
    tcpSocket->abort();
    serverText.clear();
    status = 0;
    tcpSocket->connectToHost(serverIP, 25);
}

void PSmtp::setTestUser(bool value)
{
    testUser = value;
}

void PSmtp::testUserIsOk(QString username, QString password)
{
    this->username = username;
    this->password = password;
    tcpSocket->abort();
    serverText.clear();
    status = 0;
    tcpSocket->connectToHost(serverIP, 25);
}

//连接成功
void PSmtp::connectToServer()
{
    qDebug()<<"连接stmp服务器成功！";
}
//断开连接
void PSmtp::disconnectFromServer()
{
    qDebug()<<"已经断开了与stmp服务器的连接！";
    emit disconnected();
}
//处理服务器消息
void PSmtp::getMessage()
{
    QByteArray curText = tcpSocket->readAll();
    serverText.append(curText);
    QByteArray text;
//    qDebug()<<curText;
    //出错了
    if(serverText.indexOf("Error")!=-1 || serverText.indexOf("503")!=-1)
    {
        qDebug()<<"出错了:\n"<<curText;
        emit emailStatus(EMAIL_ERROR);
        tcpSocket->disconnectFromHost();
        return ;
    }
    if(serverText.indexOf("Login Fail.")!=-1 ||serverText.indexOf("535")!=-1)
    {
        qDebug()<<"邮箱登录失败！";
        emit testUserState(false);
        return;
    }
    //已经发送完毕
    if(status==5 && serverText.indexOf("250")!=-1)
    {
        //登出
        text.append("QUIT\r\n");
        status = 6;
    }
    //发信息
    else if(status==4 && serverText.indexOf("354")!=-1)
    {
        text.append("Subject:=?utf-8?B?");
        text.append(QByteArray().append(title).toBase64());
        text.append("?=\r\n");
        text.append("To: ");
        text.append(to);
        text.append("\r\nFrom: ");
        text.append(username);
        text.append(" <");
        text.append(username);
        text.append(">\r\n");
        text.append("Content-Type: text/plain; charset=UTF8;\r\n");
        text.append("Content-Transfer-Encoding: 7BIT\r\n\r\n");
        QString t = this->text;
        t.replace("\n", "\r\n").replace("\r\n.\r\n", "\r\n..\r\n");
        text.append(t);
        text.append("\r\n.\r\n");
        status = 5;
    }
    //DATA
    else if(status==3 && serverText.indexOf("250")!=-1)
    {
        text.append("DATA\r\n");
        status = 4;
    }
    //发送方和接收方
    else if(status==2 && serverText.indexOf("235")!=-1)
    {
        if(testUser)
        {
            qDebug()<<"邮箱登录成功";
            emit testUserState(true);
            tcpSocket->disconnectFromHost();
        }
        text.append("MAIL FROM: <");
        text.append(username);
        text.append(">\r\n");
        text.append("RCPT TO: <");
        text.append(to);
        text.append(">\r\n");
        status = 3;
    }
    //登录
    else if(status==1 && serverText.indexOf("334")!=-1)
    {
        text.append(QByteArray().append(username).toBase64());
        text.append("\r\n");
        text.append(QByteArray().append(password).toBase64());
        text.append("\r\n");
        status = 2;
    }
    //打招呼
    else if(status==0 && serverText.indexOf("220")!=-1)
    {
        text.append("EHLO ");
        text.append(serverIP);
        text.append("\r\nAUTH LOGIN\r\n");//在stmp中\r\n表示结束
        status = 1;
    }
    tcpSocket->write(text);
    //qDebug()<<text;
    emit progress(double(status/STATUS_MAX));   //进度
    //发送成功
    if(status == 6)
    {
        emit emailStatus(EMAIL_SUCCEED);
        tcpSocket->disconnectFromHost();
    }
}
//成功/失败
void PSmtp::emailStatusSlot(int status)
{
    if(status == EMAIL_SUCCEED)	//邮件发送成功
    {
        qDebug()<<"邮件发送成功";
    }
    else if(status == EMAIL_ERROR)
    {
        qDebug()<<"邮件发送失败（原因可能是发、收件人邮箱号写错了，smtp服务器崩了等）";
    }
}
//进度
void PSmtp::progressChanged(double p)
{
    qDebug()<<"邮件发送进度：%"<<p*100;
}

void PSmtp::sendEmailSlot(QString username, QString password, QString to, QString title, QString text)
{
    sendEmail(username,password,to,title,text);
}
