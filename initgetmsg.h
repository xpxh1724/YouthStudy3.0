#ifndef INITGETMSG_H
#define INITGETMSG_H

#include <QWidget>

namespace Ui {
class initGetMsg;
}

class initGetMsg : public QWidget
{
    Q_OBJECT

public:
    explicit initGetMsg(QWidget *parent = nullptr);
    ~initGetMsg();
    void setReturnEnabled(bool ok);
signals:
    void initGetMsgOK(QString user,QString pwd,QString identity,QString path);
    void returnLogin();
private slots:
    void on_btnInput_clicked();

    void on_btnReturn_clicked();

private:
    Ui::initGetMsg *ui;
};

#endif // INITGETMSG_H
