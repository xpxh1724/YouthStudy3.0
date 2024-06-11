#include "editlist.h"
#include "ui_editlist.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
EditList::EditList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditList)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1需完成人员名单").arg(user));
    setModal(true);//模态窗口
}

EditList::~EditList()
{
    delete ui;
}

void EditList::EditListShow(QStringList userList,QString user)
{
    this->user=user;
    ui->textEdit->clear();
    QString displayString;
    for (int i=0;i<userList.count();++i) {
        if(i==userList.count()-1)
        {
            displayString+=userList[i];
        }
        else {
            displayString+=userList[i]+"\n";
        }
    }
    if(displayString!="请录入需完成人员名单")
    {
        ui->textEdit->setPlainText(displayString);
    }
    this->show();
}

void EditList::on_btnSave_clicked()
{
    if(ui->textEdit->toPlainText()==nullptr)
    {
        QMessageBox::information(this,"提示","名单不可保存为空");
        return;
    } 
    QSqlQuery xp;
    if(!xp.exec(QString("delete from %1;").arg(user)))
    {
        QMessageBox::information(this,"删除人员提示","删除错误！"+xp.lastError().text());
        return;
    }
   QStringList namelist=ui->textEdit->toPlainText().split('\n');
    for(auto a:namelist)
    {
        if(!xp.exec(QString("insert into %1 values('%2','%3','%4');").arg(user).arg(a.split(' ').at(0)).arg(a.split(' ').at(1)).arg(a.split(' ').at(2))))
        {
            QMessageBox::information(this,"更新人员提示","更新插入错误！"+xp.lastError().text());
            return;
        }
    }
    emit updateNeedCompTableWidget();
}

void EditList::on_btnReturn_clicked()
{
    emit toMainWidget();
}
