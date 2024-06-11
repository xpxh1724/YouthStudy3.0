#ifndef PSMTP_H
#define PSMTP_H

#include <QObject>
#include <QTcpSocket>

#define EMAIL_ERROR     0   //邮件发送失败
#define EMAIL_SUCCEED   1   //邮件发送成功
#define STATUS_MAX 6
class PSmtp : public QObject
{
    Q_OBJECT
public:
    explicit PSmtp(QString serverIP = "smtp.qq.com",QObject *parent = nullptr);
    ~PSmtp();
    void sendEmail(QString username, QString password, QString to, QString title, QString text, QString ip = "");
    void setTestUser(bool value);
    void testUserIsOk(QString username, QString password);
private:
    QString serverIP;
    QTcpSocket *tcpSocket;
    QString username;
    QString password;
    QString to;
    QString title;
    QString text;
    QByteArray serverText;
    int status;
    bool testUser=false;

signals:
    void testUserState(bool ok);
    void disconnected();
    void emailStatus(int status);   //邮件发送的状态
    void progress(double p);

public slots:
    void connectToServer();
    void disconnectFromServer();
    void getMessage();
    void emailStatusSlot(int status);
    void progressChanged(double p);
    void sendEmailSlot(QString username, QString password, QString to, QString title, QString text);
};

#endif // PSMTP_H
