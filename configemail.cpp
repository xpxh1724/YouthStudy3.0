#include "configemail.h"
#include "psmtp.h"
#include "ui_configemail.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

ConfigEmail::ConfigEmail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigEmail)
{
    ui->setupUi(this);
    setModal(true);//模态窗口
    init();
}

ConfigEmail::~ConfigEmail()
{
    delete ui;
}

void ConfigEmail::init()
{
    QSqlQuery xp;
    if(xp.exec("select user,pwd from email;"))
    {
        if(xp.next())
        {
            ui->emailLine->setText(xp.value(0).toString());
            ui->secretKeyLine->setText(xp.value(1).toString());
        }
    }
    else {
        QMessageBox::information(this,"邮箱配置-提醒","查询已配置的邮箱信息失败！\n"+xp.lastError().text());
    }
}

void ConfigEmail::on_btnReturnMainPage_clicked()
{
    emit returnMainPage();
}

void ConfigEmail::on_btnCompleteConfiguration_clicked()
{
    QString email=ui->emailLine->text();
    QString secretKey=ui->secretKeyLine->text();
    if(email==nullptr)
    {
        QMessageBox::information(this,"邮箱配置-提醒","Email不可为空！");
        return;
    }
    if(secretKey==nullptr)
    {
        QMessageBox::information(this,"邮箱配置-提醒","secretKey不可为空！");
        return;
    }
    //检查输入的邮箱信息是否可用
    PSmtp*  smtpTest= new PSmtp;
    smtpTest->setTestUser(true);
    connect(smtpTest,&PSmtp::testUserState,this,[=](bool ok){
        if(!ok) //邮箱账号不可用
        {
            QMessageBox::information(this,"邮箱配置-提示","邮箱配置不可用!!!\n请重新配置!!!");
            smtpTest->deleteLater();
            return;
        }
        //邮箱账号可用
        QSqlQuery xp;
        if(!xp.exec(QString("update email set user='%1',pwd='%2';").arg(email).arg(secretKey)))
        {
            QMessageBox::information(this,"邮箱配置-提示","修改邮箱配置失败！\n"+xp.lastError().text());
            return;
        }
        else {
            QMessageBox::information(this,"邮箱配置-提示","邮箱配置成功！\n");
            emit returnMainPage();
        }
    });
    //测试账号是否可用
    smtpTest->testUserIsOk(email,secretKey);
}
