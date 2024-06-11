#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    void initUser();
    void setInitMsg(QString user,QString pwd,QString identity);
    void againLogin();
signals:
    void loginSuccess(QString user,QString pwd);
    void loginTeacher(QStringList users,QString team);
    void loginCollegeAdmin(QStringList users,QString team);
    void inputOtherUser();
private slots:
    void on_btnLogin_clicked();

    void on_btnInput_clicked();

private:
    Ui::Login *ui;
    QStringList userList;
};

#endif // LOGIN_H
