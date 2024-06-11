#include "initgetmsg.h"
#include "ui_initgetmsg.h"

#include <QFileDialog>
#include <QListView>
#include <QMessageBox>
#include <QDebug>
initGetMsg::initGetMsg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::initGetMsg)
{
    ui->setupUi(this);
    setWindowTitle("青年大学习后台管理系统-初始化");
    ui->IdentityBox->setView(new QListView());
    ui->btnReturn->setEnabled(false);
}

initGetMsg::~initGetMsg()
{
    delete ui;
}

void initGetMsg::setReturnEnabled(bool ok)
{
    ui->btnReturn->setEnabled(ok);
}

void initGetMsg::on_btnInput_clicked()
{
    QString user=ui->usernameLine->text();
    QString pwd=ui->passwordLine->text();
    QString identity=ui->IdentityBox->currentText();
    if(user==nullptr||pwd==nullptr)
    {
        QMessageBox::information(this,"录入信息-提示","账号/密码不能为空");
        return;
    }
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择要录入的xlsx文件"), "", tr("Excel Files (*.xlsx)"));
    if (filePath.isEmpty())
    {
        // 用户点击了取消按钮或者进行了其他非法操作
        QMessageBox::information(this, tr("录入信息-提示"), tr("没有选择文件，操作已取消"));
        qDebug()<<"没有选择文件，操作已取消";
    }
    else
    {
        emit initGetMsgOK(user,pwd,identity,filePath);
    }

}

void initGetMsg::on_btnReturn_clicked()
{
    emit returnLogin();
}
