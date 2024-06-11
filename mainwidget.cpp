#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTextCodec>
#include <QTimer>
#include <QScrollBar>
#include <QJsonArray>
#include <QClipboard>
#include <QMovie>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "configemail.h"
#include "user.h"
mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    setWindowTitle("青年大学习后台管理系统");
    initLogin();//初始化登录类
    initSqliteQuery();//初始化数据库
}

mainWidget::~mainWidget()
{
    editlist->deleteLater();
    login->deleteLater();
    movie->deleteLater();
    loadLabel->deleteLater();
    delete ui;
}
//初始化数据库
void mainWidget::initSqliteQuery()
{
    //检查是否初始化过
    QSqlQuery xp;
    bool ret=xp.exec("SELECT COUNT(*) AS table_count FROM sqlite_master WHERE type='table';");//查询所有表
    if(!ret) qDebug()<<xp.lastError().text();
    if(!xp.next()) qDebug()<<xp.lastError().text();
    if(0==xp.value(0).toInt())//数据库为空
    {
        //创建表init
        if(!xp.exec("CREATE TABLE init ('isinit' varchar(255) );"))
        {
            QMessageBox::information(this,"创建表init",QString("创建表init失败:\n%1").arg(xp.lastError().text()));
            return;
        }
        //创建表email
        if(!xp.exec("CREATE TABLE email ('user' varchar(255),'pwd' varchar(255) );"))
        {
            QMessageBox::information(this,"创建表email",QString("创建表email失败:\n%1").arg(xp.lastError().text()));
            return;
        }
        //创建表userlist
        if(!xp.exec("CREATE TABLE userlist ('user' varchar(255) PRIMARY KEY,'pwd' varchar(255),'identity' varchar(255) );"))
        {
            QMessageBox::information(this,"创建表userlist",QString("创建表userlist失败:\n%1").arg(xp.lastError().text()));
            return;
        }
        //创建表defaultmsg
        if(!xp.exec("CREATE TABLE defaultmsg ('user' varchar(255),'pwd' varchar(255),'identity' varchar(255) );"))
        {
            QMessageBox::information(this,"创建表defaultmsg",QString("创建表defaultmsg失败:\n%1").arg(xp.lastError().text()));
            return;
        }
        csh=new initGetMsg;
        csh->show();
        connect(csh,&initGetMsg::initGetMsgOK,this,[=](QString cshUser,QString cshPwd,QString cshIdentity,QString filePath){
            if("团支书"==cshIdentity)
            {
                QXlsx::Document xlsx(filePath);// 打开 Excel 文件
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();// 获取工作表对象
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                QStringList cshIdList,cshNameList,cshEmailList;
                for(int i=1;i<=lastRow;++i)
                {
                    cshIdList.append(worksheet->read(i,1).toString());
                    cshNameList.append(worksheet->read(i,2).toString());
                    cshEmailList.append(worksheet->read(i,3).toString());
                }
                QSqlQuery axp;
                //查询账号是否存在
                if(axp.exec(QString("select *from userlist where user='%1';").arg(cshUser)))
                {
                    if(!axp.next())//账号不存在，则插入
                    {
                        //插入账号
                        if(!axp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                        {
                            QMessageBox::information(this,"插入账号-提示",QString("插入账号失败:\n%1").arg(axp.lastError().text()));
                            return;
                        }
                    }
                    else {
                        QMessageBox::information(this,"插入账号-提示",QString("插入账号失败,账号已经存在！"));
                        return;
                    }
                }

                //查询团支部是否存在
                if(axp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(cshUser)))
                {
                    if(!axp.next())//如果不存在，则插入
                    {
                        //创建团支部表
                        if(!axp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(cshUser)))
                        {
                            QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+axp.lastError().text());
                            return;
                        }
                    }
                }
                else {
                    QMessageBox::information(this,"查询团支部是否存在-提示","查询团支部是否存在失败:\n"+axp.lastError().text());
                    return;
                }

                //插入团支部成员信息
                for(int i=0;i<cshNameList.size();++i)
                {
                    QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                              Q_ARG(QString,cshUser),
                                              Q_ARG(QString,cshIdList[i]),
                                              Q_ARG(QString,cshNameList[i]),
                                              Q_ARG(QString,cshEmailList[i]));
                }
            }
            else if ("老师"==cshIdentity)
            {
                // 打开 Excel 文件
                QXlsx::Document xlsx(filePath);
                // 获取工作表对象
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                QStringList initTzbList,initIdList,initNameList,initEmailList;
                for(int i=1;i<=lastRow;++i)
                {
                    QString bj = worksheet->read(i,1).toString();
                    initIdList.append(worksheet->read(i,2).toString());
                    QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                    initTzbList.append(tzb);
                    initNameList.append(worksheet->read(i,3).toString());
                    initEmailList.append(worksheet->read(i,4).toString());
                }
                QSet<QString>tzbSet(initTzbList.toSet());
                //遍历所有团支部
                for(QString t:tzbSet)
                {
                    QSqlQuery xp;
                    //查询团支部账号是否存在
                    if(xp.exec(QString("select *from userlist where user='%1'").arg(t)))
                    {
                        if(!xp.next())//不存在则插入
                        {
                            //插入账号
                            if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(t)))
                            {
                                QMessageBox::information(this,"插入账号提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                return;
                            }
                        }
                    }
                    else {
                        QMessageBox::information(this,"查询账号(老师)-提示",QString("查询账号(老师)失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }

                    //查询团支部表是否存在
                    if(xp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(t)))
                    {
                        if(!xp.next())
                        {
                            //创建团支部表
                            if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(t)))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                return;
                            }
                        }
                    }
                    else {
                        QMessageBox::information(this,"查询团支部表是否存在(老师)-提示",QString("查询团支部表是否存在(老师)失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }
                }
                //插入团支部成员信息
                for (int i=0;i<initIdList.size();++i)
                {
                    QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                              Q_ARG(QString,initTzbList[i]),
                                              Q_ARG(QString,initIdList[i]),
                                              Q_ARG(QString,initNameList[i]),
                                              Q_ARG(QString,initEmailList[i]));
                }
                QSqlQuery axp;
                //插入老师账号
                if(!axp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                {
                    QMessageBox::information(this,"插入老师账号提示",QString("插入老师账号失败:\n%1").arg(axp.lastError().text()));
                    return;
                }
            }
            else if ("学院管理员"==cshIdentity) {
                QXlsx::Document xlsx(filePath);// 打开 Excel 文件
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet(); // 获取工作表对象
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                QStringList initTzbList,initIdList,initNameList,initEmailList;
                QSet<QString>tzbSet;
                for(int i=1;i<=lastRow;++i)
                {
                    QString bj = worksheet->read(i,1).toString();
                    initIdList.append(worksheet->read(i,2).toString());
                    QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                    QString name = worksheet->read(i,3).toString();
                    initTzbList.append(tzb);
                    tzbSet.insert(tzb);
                    initNameList.append(name);
                    initEmailList.append(worksheet->read(i,4).toString());
                    qDebug()<<"团支部："<<tzb<<" 学号： "<<worksheet->read(i,2).toString()<<" 姓名："<<name<<" 邮箱："<<worksheet->read(i,4).toString();
                }
                QSqlQuery xp;
                //创建学院表
                if(!xp.exec(QString("CREATE TABLE %1 ('name' varchar(255) NOT NULL);").arg(cshUser)))
                {
                    QMessageBox::information(this,"创建学院表提示","创建学院表失败:\n"+xp.lastError().text());
                    return;
                }
                //遍历所有团支部
                for(QString t:tzbSet)
                {
                    //查询对应团支书账号是否存在
                    if(xp.exec(QString("select *from userlist where user='%1'").arg(t)))
                    {
                        if(!xp.next())//如果不存在则插入
                        {
                            //插入对应团支书账号
                            if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(t)))
                            {
                                QMessageBox::information(this,"插入账号-提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                return;
                            }
                        }
                    }
                    else
                    {
                        QMessageBox::information(this,"查询对应团支书账号-提示",QString("查询对应团支书账号失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }

                    //查询对应团支部表是否存在
                    if(xp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(t)))
                    {
                        if(!xp.next())//不存在则创建
                        {
                            //创建对应团支部表
                            if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(t)))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                return;
                            }
                        }
                    }
                    else
                    {
                        QMessageBox::information(this,"查询对应团支部表-提示",QString("查询对应团支部表失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }

                    //插入学院表中对应团支部
                    if(!xp.exec(QString("INSERT INTO %1 VALUES ('%2');").arg(cshUser).arg(t)))
                    {
                        QMessageBox::information(this,"插入学院表中对应团支部-提示",QString("插入学院表中对应团支部失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }
                }
                //插入团支部对应成员信息
                for (int i=0;i<initIdList.size();++i)
                {
                    QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                              Q_ARG(QString,initTzbList[i]),
                                              Q_ARG(QString,initIdList[i]),
                                              Q_ARG(QString,initNameList[i]),
                                              Q_ARG(QString,initEmailList[i]));
                }
                //插入学院管理员账号
                if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                {
                    QMessageBox::information(this,"插入学院管理员账号提示",QString("插入学院管理员账号失败:\n%1").arg(xp.lastError().text()));
                    return;
                }
            }
            QSqlQuery p;
            p.exec("INSERT INTO init VALUES ('已经初始化');");
            p.exec("INSERT INTO email VALUES ('','');");
            p.exec(QString("INSERT INTO defaultmsg VALUES('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity));
            csh->close();
            csh->deleteLater();
            login->setInitMsg(cshUser,cshPwd,cshIdentity);
            login->show();
        });
    }
    else
    {
        qDebug()<<xp.value(0).toInt();
        if(!xp.exec(QString("select isinit from init;")))
        {
            QMessageBox::information(this,"查询是否初始化提示","查询是否初始化，查询失败:\n"+xp.lastError().text());
            return;
        }
        xp.next();
        if("已经初始化"==xp.value(0).toString())
        {
            login->initUser();
            login->show();
        }
        else {
            csh=new initGetMsg;
            csh->show();
            connect(csh,&initGetMsg::initGetMsgOK,this,[=](QString cshUser,QString cshPwd,QString cshIdentity,QString filePath){
                if("团支书"==cshIdentity)
                {
                    QXlsx::Document xlsx(filePath);// 打开 Excel 文件
                    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();// 获取工作表对象
                    int lastRow=findLastRowWithDataInColumnA(worksheet);
                    qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                    QStringList cshIdList,cshNameList,cshEmailList;
                    for(int i=1;i<=lastRow;++i)
                    {
                        cshIdList.append(worksheet->read(i,1).toString());
                        cshNameList.append(worksheet->read(i,2).toString());
                        cshEmailList.append(worksheet->read(i,3).toString());
                    }
                    QSqlQuery axp;
                    //查询账号是否存在
                    if(axp.exec(QString("select *from userlist where user='%1';").arg(cshUser)))
                    {
                        if(!axp.next())//账号不存在，则插入
                        {
                            //插入账号
                            if(!axp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                            {
                                QMessageBox::information(this,"插入账号-提示",QString("插入账号失败:\n%1").arg(axp.lastError().text()));
                                return;
                            }
                        }
                        else {
                            QMessageBox::information(this,"插入账号-提示",QString("插入账号失败,账号已经存在！"));
                            return;
                        }
                    }

                    //查询团支部是否存在
                    if(axp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(cshUser)))
                    {
                        if(!axp.next())//如果不存在，则插入
                        {
                            //创建团支部表
                            if(!axp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(cshUser)))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+axp.lastError().text());
                                return;
                            }
                        }
                    }
                    else {
                        QMessageBox::information(this,"查询团支部是否存在-提示","查询团支部是否存在失败:\n"+axp.lastError().text());
                        return;
                    }

                    //插入团支部成员信息
                    for(int i=0;i<cshNameList.size();++i)
                    {
                        QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                                  Q_ARG(QString,cshUser),
                                                  Q_ARG(QString,cshIdList[i]),
                                                  Q_ARG(QString,cshNameList[i]),
                                                  Q_ARG(QString,cshEmailList[i]));
                    }
                }
                else if ("老师"==cshIdentity)
                {
                    // 打开 Excel 文件
                    QXlsx::Document xlsx(filePath);
                    // 获取工作表对象
                    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
                    int lastRow=findLastRowWithDataInColumnA(worksheet);
                    qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                    QStringList initTzbList,initIdList,initNameList,initEmailList;
                    for(int i=1;i<=lastRow;++i)
                    {
                        QString bj = worksheet->read(i,1).toString();
                        initIdList.append(worksheet->read(i,2).toString());
                        QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                        initTzbList.append(tzb);
                        initNameList.append(worksheet->read(i,3).toString());
                        initEmailList.append(worksheet->read(i,4).toString());
                    }
                    QSet<QString>tzbSet(initTzbList.toSet());
                    //遍历所有团支部
                    for(QString t:tzbSet)
                    {
                        QSqlQuery xp;
                        //查询团支部账号是否存在
                        if(xp.exec(QString("select *from userlist where user='%1'").arg(t)))
                        {
                            if(!xp.next())//不存在则插入
                            {
                                //插入账号
                                if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(t)))
                                {
                                    QMessageBox::information(this,"插入账号提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                    return;
                                }
                            }
                        }
                        else {
                            QMessageBox::information(this,"查询账号(老师)-提示",QString("查询账号(老师)失败:\n%1").arg(xp.lastError().text()));
                            return;
                        }

                        //查询团支部表是否存在
                        if(xp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(t)))
                        {
                            if(!xp.next())
                            {
                                //创建团支部表
                                if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(t)))
                                {
                                    QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                    return;
                                }
                            }
                        }
                        else {
                            QMessageBox::information(this,"查询团支部表是否存在(老师)-提示",QString("查询团支部表是否存在(老师)失败:\n%1").arg(xp.lastError().text()));
                            return;
                        }
                    }
                    //插入团支部成员信息
                    for (int i=0;i<initIdList.size();++i)
                    {
                        QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                                  Q_ARG(QString,initTzbList[i]),
                                                  Q_ARG(QString,initIdList[i]),
                                                  Q_ARG(QString,initNameList[i]),
                                                  Q_ARG(QString,initEmailList[i]));
                    }
                    QSqlQuery axp;
                    //插入老师账号
                    if(!axp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                    {
                        QMessageBox::information(this,"插入老师账号提示",QString("插入老师账号失败:\n%1").arg(axp.lastError().text()));
                        return;
                    }
                }
                else if ("学院管理员"==cshIdentity) {
                    QXlsx::Document xlsx(filePath);// 打开 Excel 文件
                    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet(); // 获取工作表对象
                    int lastRow=findLastRowWithDataInColumnA(worksheet);
                    qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                    QStringList initTzbList,initIdList,initNameList,initEmailList;
                    QSet<QString>tzbSet;
                    for(int i=1;i<=lastRow;++i)
                    {
                        QString bj = worksheet->read(i,1).toString();
                        initIdList.append(worksheet->read(i,2).toString());
                        QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                        QString name = worksheet->read(i,3).toString();
                        initTzbList.append(tzb);
                        tzbSet.insert(tzb);
                        initNameList.append(name);
                        initEmailList.append(worksheet->read(i,4).toString());
                        qDebug()<<"团支部："<<tzb<<" 学号： "<<worksheet->read(i,2).toString()<<" 姓名："<<name<<" 邮箱："<<worksheet->read(i,4).toString();
                    }
                    QSqlQuery xp;
                    //创建学院表
                    if(!xp.exec(QString("CREATE TABLE %1 ('name' varchar(255) NOT NULL);").arg(cshUser)))
                    {
                        QMessageBox::information(this,"创建学院表提示","创建学院表失败:\n"+xp.lastError().text());
                        return;
                    }
                    //遍历所有团支部
                    for(QString t:tzbSet)
                    {
                        //查询对应团支书账号是否存在
                        if(xp.exec(QString("select *from userlist where user='%1'").arg(t)))
                        {
                            if(!xp.next())//如果不存在则插入
                            {
                                //插入对应团支书账号
                                if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(t)))
                                {
                                    QMessageBox::information(this,"插入账号-提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                    return;
                                }
                            }
                        }
                        else
                        {
                            QMessageBox::information(this,"查询对应团支书账号-提示",QString("查询对应团支书账号失败:\n%1").arg(xp.lastError().text()));
                            return;
                        }

                        //查询对应团支部表是否存在
                        if(xp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(t)))
                        {
                            if(!xp.next())//不存在则创建
                            {
                                //创建对应团支部表
                                if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(t)))
                                {
                                    QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                    return;
                                }
                            }
                        }
                        else
                        {
                            QMessageBox::information(this,"查询对应团支部表-提示",QString("查询对应团支部表失败:\n%1").arg(xp.lastError().text()));
                            return;
                        }

                        //插入学院表中对应团支部
                        if(!xp.exec(QString("INSERT INTO %1 VALUES ('%2');").arg(cshUser).arg(t)))
                        {
                            QMessageBox::information(this,"插入学院表中对应团支部-提示",QString("插入学院表中对应团支部失败:\n%1").arg(xp.lastError().text()));
                            return;
                        }
                    }
                    //插入团支部对应成员信息
                    for (int i=0;i<initIdList.size();++i)
                    {
                        QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                                  Q_ARG(QString,initTzbList[i]),
                                                  Q_ARG(QString,initIdList[i]),
                                                  Q_ARG(QString,initNameList[i]),
                                                  Q_ARG(QString,initEmailList[i]));
                    }
                    //插入学院管理员账号
                    if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                    {
                        QMessageBox::information(this,"插入学院管理员账号提示",QString("插入学院管理员账号失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }
                }
                QSqlQuery p;
                p.exec("INSERT INTO init VALUES ('已经初始化');");
                p.exec("INSERT INTO email VALUES ('','');");
                p.exec(QString("INSERT INTO defaultmsg VALUES('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity));
                csh->close();
                csh->deleteLater();
                login->setInitMsg(cshUser,cshPwd,cshIdentity);
                login->show();
            });
        }
    }
}
//初始化登录类
void mainWidget::initLogin()
{
    login=new Login;
    //团支书登录信号
    connect(login,&Login::loginSuccess,this,[=](QString User,QString Pwd){
        this->user=User;this->pwd=Pwd;
        ui->UserName->setText(user);
        //============================团支书TableWidget初始化============================
        {
            QString horQss="QTableView QHeaderView{background-color:rgb(240,207,214);}"
                           "QTableView QHeaderView::section{color: rgb(122, 175, 227);"
                           "border-right: 1px solid rgb(240,207,214);border-left: 1px solid rgb(240,207,214);"
                           "font-weight:bold;height: 35px;}";
            QString scrollBarStyle = "QScrollBar { background: rgb(240,207,214); width: 12px; }"
                                     "QScrollBar::handle { background: rgba(136, 193, 150,0.5); border: 0px solid rgb(255, 255, 255); border-radius: 5px; }"
                                     "QScrollBar::handle:hover { background: rgba(136, 193, 150,0.7); }"
                                     "QScrollBar::sub-line, QScrollBar::add-line { background: rgb(210,225,243); }"
                                     "QScrollBar::sub-page:vertical, QScrollBar::add-page:vertical { background: rgb(210,225,243); }";
            ui->NeedCompTableWidget->horizontalHeader()->setVisible(true);//显示水平头
            ui->NeedCompTableWidget->verticalHeader()->setVisible(false);//显示垂直头
            ui->NeedCompTableWidget->horizontalHeader()->setStretchLastSection(true);
            ui->NeedCompTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//用户不能直接在表格或列表的单元格中进行编辑
            ui->NeedCompTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);// 设置表头自适应列宽
            ui->NeedCompTableWidget->horizontalHeader()->setStyleSheet(horQss);
            ui->NeedCompTableWidget->verticalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->NeedCompTableWidget->horizontalScrollBar()->setStyleSheet(scrollBarStyle);

            ui->InCompTableWidget->horizontalHeader()->setVisible(true);//显示水平头
            ui->InCompTableWidget->verticalHeader()->setVisible(false);//显示垂直头
            ui->InCompTableWidget->horizontalHeader()->setStretchLastSection(true);
            ui->InCompTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//用户不能直接在表格或列表的单元格中进行编辑
            ui->InCompTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);// 设置表头自适应列宽
            ui->InCompTableWidget->horizontalHeader()->setStyleSheet(horQss);
            ui->InCompTableWidget->verticalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->InCompTableWidget->horizontalScrollBar()->setStyleSheet(scrollBarStyle);

            ui->UserTableWidget->horizontalHeader()->setVisible(true);//显示水平头
            ui->UserTableWidget->verticalHeader()->setVisible(false);//显示垂直头
            ui->UserTableWidget->horizontalHeader()->setStretchLastSection(true);
            ui->UserTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//用户不能直接在表格或列表的单元格中进行编辑
            ui->UserTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);// 设置表头自适应列宽
            ui->UserTableWidget->horizontalHeader()->setStyleSheet(horQss);
            ui->UserTableWidget->verticalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->UserTableWidget->horizontalScrollBar()->setStyleSheet(scrollBarStyle);
        }

        //============================团支书-按钮初始化============================
        {
            //编辑名单
            connect(ui->btnEditList,&QPushButton::clicked,this,[=]{
                editlist=new EditList;
                //返回信号
                connect(editlist,&EditList::toMainWidget,this,[=]{editlist->close();});
                //保存文件信号
                connect(editlist,&EditList::updateNeedCompTableWidget,this,[=]{
                    editlist->close();
                    int rowCount = ui->NeedCompTableWidget->rowCount();
                    for (int i = rowCount - 1; i >= 0; --i) {
                        ui->NeedCompTableWidget->removeRow(i);
                    }
                    NeedCompList.clear();
                    QSqlQuery xp;
                    if(!xp.exec(QString("select name from %1;").arg(user)))
                    {
                        QMessageBox::information(this,"更新名单查询","查询失败！"+xp.lastError().text());
                        return;
                    }
                    while(xp.next())
                    {
                        NeedCompList.append(xp.value(0).toString());
                        int nowRow=ui->NeedCompTableWidget->rowCount();
                        ui->NeedCompTableWidget->insertRow(nowRow);//增加一行
                        //                ui->NeedCompTableWidget->setRowHeight(nowRow,30);//设置行高度
                        QTableWidgetItem *nameItem=new QTableWidgetItem(xp.value(0).toString());
                        nameItem->setTextAlignment(Qt::AlignCenter);
                        ui->NeedCompTableWidget->setItem(nowRow,0,nameItem);
                    }
                    QMessageBox::information(this,"保存文件","保存成功!");
                });
                QStringList editList;
                int rowCount = ui->NeedCompTableWidget->rowCount();
                for(int i=0;i<rowCount;++i)
                {
                    QSqlQuery xp;
                    if(xp.exec(QString("select *from %1 where name='%2';").arg(user).arg(ui->NeedCompTableWidget->item(i,0)->text())))
                    {
                        xp.next();
                        editList.append(xp.value(0).toString()+" "+xp.value(1).toString()+" "+xp.value(2).toString());
                    }
                }
                editlist->EditListShow(editList,user);
            });
            //重新查询
            connect(ui->btnXlsx,&QPushButton::clicked,this,[=]{
                if(token.isEmpty())
                {
                    QMessageBox::information(this,"提示","token为空");
                    return;
                }
                if(id==0)
                {
                    QMessageBox::information(this,"提示","id为空");
                    return;
                }
                //发送下载用户学习数据请求
                startRequestGet(QUrl(QString("https://dxx.scyol.com/backend//study/student/excel?stagesId=%1&orgId=%2&name=&tel=&token=%3").arg(id).arg(orgId).arg(token)));
                loadLabel->show();
                loadLabel->move(QPoint(ui->UserTableWidget->pos()+QPoint(ui->UserTableWidget->width()/2-loadLabel->width()/2,ui->UserTableWidget->height()/2-loadLabel->height())));
                ui->btnEditList->setEnabled(false);
                ui->btnXlsx->setEnabled(false);
                ui->btnConfigEmail->setEnabled(false);
                ui->btnExitLogin->setEnabled(false);
                ui->btnCopyList->setEnabled(false);
                ui->btnEmail->setEnabled(false);
                int rowCount = ui->UserTableWidget->rowCount();
                for (int i = rowCount - 1; i >= 0; --i) {
                    ui->UserTableWidget->removeRow(i);
                }
                int rowCount2 = ui->InCompTableWidget->rowCount();
                for (int i = rowCount2 - 1; i >= 0; --i) {
                    ui->InCompTableWidget->removeRow(i);
                }
                ui->UserCount->setText(QString("共计 XX 条数据"));
                ui->schedule->setText(QString("0/%1").arg(NeedCompList.count()));
            });
            //配置邮箱
            connect(ui->btnConfigEmail,&QPushButton::clicked,this,[=]{
                ConfigEmail *configEmail=new ConfigEmail;
                connect(configEmail,&ConfigEmail::returnMainPage,this,[=]{configEmail->close();configEmail->deleteLater();});
                configEmail->show();
            });
            //退出登录
            connect(ui->btnExitLogin,&QPushButton::clicked,this,[=]{
                int rowCount = ui->UserTableWidget->rowCount();
                for (int i = rowCount - 1; i >= 0; --i) {
                    ui->UserTableWidget->removeRow(i);
                }

                int rowCount2 = ui->InCompTableWidget->rowCount();
                for (int i = rowCount2 - 1; i >= 0; --i) {
                    ui->InCompTableWidget->removeRow(i);
                }
                int rowCount3 = ui->NeedCompTableWidget->rowCount();
                for (int i = rowCount3 - 1; i >= 0; --i) {
                    ui->NeedCompTableWidget->removeRow(i);
                }
                this->user.clear();
                this->pwd.clear();
                this->token.clear();
                this->orgId=0;
                this->NeedCompList.clear();
                ui->UserCount->setText(QString("共计 XX 条数据"));
                this->hide();
                login->show();
            });
            //复制名单
            connect(ui->btnCopyList,&QPushButton::clicked,this,[=]{
                QClipboard *clipboard = QApplication::clipboard();
                QString incomplist;
                for(int i=0;i<ui->InCompTableWidget->rowCount();++i)
                {
                    QString name=ui->InCompTableWidget->item(i,0)->text();
                    if(!name.contains("非本支部"))
                    {
                        if(i==ui->InCompTableWidget->rowCount()-1)
                        {
                            incomplist+=name;
                        }
                        else {
                            incomplist+=name+"\n";
                        }
                    }
                }
                if(incomplist==nullptr)
                {
                    QMessageBox::information(this,"复制名单提醒","本支部成员已全部完成！");
                    return;
                }
                clipboard->setText(incomplist);
                QMessageBox::information(this,"复制名单提醒","未完成人员名单已复制到粘贴板！");
            });
            //邮箱提醒
            connect(ui->btnEmail,&QPushButton::clicked,this,[=]{
                QSqlQuery xp;
                if(!xp.exec("select *from email;"))
                {
                    QMessageBox::information(this,"查询邮箱是否初始化-提醒","查询邮箱是否初始化失败！\n"+xp.lastError().text());
                    return;
                }
                xp.next();
                QString sendUser=xp.value(0).toString();
                QString sendPwd=xp.value(1).toString();
                if(sendUser==nullptr||sendPwd==nullptr)
                {
                    QMessageBox::information(this,"邮箱初始化-提醒","请先配置发送邮箱账号和密钥！");
                    return;
                }
                //检查邮箱信息是否可用
                PSmtp*  smtpTest= new PSmtp;
                smtpTest->setTestUser(true);
                connect(smtpTest,&PSmtp::testUserState,this,[=](bool ok){
                    if(!ok) //邮箱账号不可用
                    {
                        QMessageBox::information(this,"邮箱配置不可用-提示","邮箱配置不可用!!!\n请重新配置!!!");
                        return;
                    }
                    //邮箱账号可用
                    QSqlQuery xp;
                    QStringList sendNameList,sendEmailList;
                    for (int i=0;i<ui->InCompTableWidget->rowCount();++i) {
                        //如果是本支部成员
                        if(!ui->InCompTableWidget->item(i,0)->text().contains("非本支部"))
                        {
                            sendNameList.append(ui->InCompTableWidget->item(i,0)->text());
                        }
                    }
                    //本支部人员是否已完成
                    if(sendNameList.count()>0)
                    {
                        //遍历查询未完成人员邮箱(同班重名情况未处理)
                        for (int i=0;i<sendNameList.count();++i)
                        {
                            //查询邮箱失败
                            if(!xp.exec(QString("select email from %1 where name='%2';").arg(user).arg(sendNameList[i])))
                            {
                                QMessageBox::information(this,"查询未完成人员邮箱提示","查询未完成人员邮箱失败！\n"+xp.lastError().text());
                                return;
                            }
                            else //查询邮箱成功
                            {
                                if(xp.next()) sendEmailList.append(xp.value(0).toString()); //邮箱信息存在
                            }
                        }
                        //遍历进行发送邮箱提醒
                        for(int i=0;i<sendNameList.count();++i)
                        {
                            qDebug()<<sendNameList[i]<<" "<<sendEmailList[i];
                        }
                    }
                    else
                    {
                        QMessageBox::information(this,"邮箱提醒-提示","本支部成员已全部完成,无需进行邮箱提醒！");
                        return;
                    }
                    //开始遍历发送提醒邮箱
                    for (int i=0;i<sendNameList.count();++i)
                    {
                        //异步执行sendEmail函数
                        PSmtp *smtp= new PSmtp;//默认是qq邮箱服务器，想用别的服务器就 new PSmtp(Smtp服务器地址)
                        connect(smtp, SIGNAL(disconnected()), smtp, SLOT(deleteLater()));	//发送完毕自行销毁
                        connect(smtp, SIGNAL(emailStatus(int)), this, SLOT(emailStatusSlot(int)));//状态

                        QMetaObject::invokeMethod(smtp, "sendEmailSlot", Qt::QueuedConnection,
                                                  Q_ARG(QString,sendUser),
                                                  Q_ARG(QString,sendPwd),
                                                  Q_ARG(QString,sendEmailList[i]),
                                                  Q_ARG(QString,"青年大学习完成提醒"),
                                                  Q_ARG(QString,QString("%1同学，你好！\n  请尽快完成本周青年大学习谢谢！").arg(sendNameList[i])));
                    }
                     QMessageBox::information(this,"邮箱提醒-提示","邮箱提醒发送成功！(学生信息邮箱为空除外)");
                });
                //测试账号是否可用
                smtpTest->testUserIsOk(sendUser,sendPwd);
            });
        }
        ui->stackedWidget->setCurrentIndex(0);
        login->hide();
        this->show();
        movie = new QMovie(":/ptr/load.gif");
        loadLabel = new QLabel;
        loadLabel->setMovie(movie);
        movie->start();
        loadLabel->setParent(ui->UserTableWidget);
        loadLabel->resize(200,200);
        loadLabel->show();
        loadLabel->move(QPoint(ui->UserTableWidget->pos()+QPoint(ui->UserTableWidget->width()/2-loadLabel->width()/2,ui->UserTableWidget->height()/2-loadLabel->height())));
        ui->btnEditList->setEnabled(false);
        ui->btnXlsx->setEnabled(false);
        ui->btnConfigEmail->setEnabled(false);
        ui->btnExitLogin->setEnabled(false);
        ui->btnCopyList->setEnabled(false);
        ui->btnEmail->setEnabled(false);
        //初始化需完成人员名单
        if(!initCompList())
        {
            QMessageBox::information(this,"提示","初始化需完成人员名单失败！");
            return;
        }
        else {
            for(QString str:NeedCompList)
            {
                int nowRow=ui->NeedCompTableWidget->rowCount();
                ui->NeedCompTableWidget->insertRow(nowRow);//增加一行
                ui->NeedCompTableWidget->setRowHeight(nowRow,30);//设置行高度
                QTableWidgetItem *nameItem=new QTableWidgetItem(str);
                nameItem->setTextAlignment(Qt::AlignCenter);
                ui->NeedCompTableWidget->setItem(nowRow,0,nameItem);
            }
            qDebug()<<"初始化需完成人员名单成功！";
        }
        ui->schedule->setText(QString("0/%1").arg(NeedCompList.count()));
        startRequestPost(QUrl("https://dxx.scyol.com/backend/adminUser/login"));//发送登录账号请求
    });
    //老师登录信号
    connect(login,&Login::loginTeacher,this,[=](QStringList users,QString team){
        Users=users;
        ui->UsersName->setText(team);
        //============================老师TableWidget初始化============================
        {
            QString horQss="QTableView QHeaderView{background-color:rgb(240,207,214);}"
                           "QTableView QHeaderView::section{color: rgb(122, 175, 227);"
                           "border-right: 1px solid rgb(240,207,214);border-left: 1px solid rgb(240,207,214);"
                           "font-weight:bold;height: 35px;}";
            QString scrollBarStyle = "QScrollBar { background: rgb(240,207,214); width: 12px; }"
                                     "QScrollBar::handle { background: rgba(136, 193, 150,0.5); border: 0px solid rgb(255, 255, 255); border-radius: 5px; }"
                                     "QScrollBar::handle:hover { background: rgba(136, 193, 150,0.7); }"
                                     "QScrollBar::sub-line, QScrollBar::add-line { background: rgb(210,225,243); }"
                                     "QScrollBar::sub-page:vertical, QScrollBar::add-page:vertical { background: rgb(210,225,243); }";
            ui->UsersTableWidget->horizontalHeader()->setVisible(true);//显示水平头
            ui->UsersTableWidget->verticalHeader()->setVisible(false);//显示垂直头
            ui->UsersTableWidget->horizontalHeader()->setStretchLastSection(true);
            ui->UsersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//用户不能直接在表格或列表的单元格中进行编辑
            ui->UsersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);// 设置表头自适应列宽
            ui->UsersTableWidget->horizontalHeader()->setStyleSheet(horQss);
            ui->UsersTableWidget->verticalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->UsersTableWidget->horizontalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->schedule_2->setText(QString("0/%1").arg(Users.count()));
        }
        //============================老师按钮初始化============================
        {
            //一键重新查询
            connect(ui->btnReQuery,&QPushButton::clicked,this,[=]{
                loadLabel->show();
                ui->btnReQuery->setEnabled(false);
                ui->btnCopyAllInCompList->setEnabled(false);
                ui->btnImport->setEnabled(false);
                ui->btnConfigEmailTeacher->setEnabled(false);
                ui->btnEmailTeacher->setEnabled(false);
                ui->btnExit->setEnabled(false);
                loadLabel->move(QPoint(ui->UsersTableWidget->pos()+QPoint(ui->UsersTableWidget->width()/2-loadLabel->width()/2,ui->UsersTableWidget->height()/2-loadLabel->height())));

                int row=ui->UsersTableWidget->rowCount();
                for (int i=0;i<row;++i) {
                    QWidget *widget = ui->UsersTableWidget->cellWidget(i, 0);
                    if (widget)
                    {
                        User *myuser = qobject_cast<User *>(widget); //判断是不是User控件
                        if (myuser)
                        {
                            // 确实是User 控件
                            myuser->ReQuery();
                        }
                    }
                }
                loadLabel->hide();
                ui->btnReQuery->setEnabled(true);
                ui->btnCopyAllInCompList->setEnabled(true);
                ui->btnImport->setEnabled(true);
                ui->btnConfigEmailTeacher->setEnabled(true);
                ui->btnEmailTeacher->setEnabled(true);
                ui->btnExit->setEnabled(true);
            });
            //复制全部未完成名单
            connect(ui->btnCopyAllInCompList,&QPushButton::clicked,this,[=]{
                QString msg;
                int row=ui->UsersTableWidget->rowCount();
                for (int i=0;i<row;++i) {
                    QWidget *widget = ui->UsersTableWidget->cellWidget(i, 0);
                    if (widget)
                    {
                        User *myuser = qobject_cast<User *>(widget); //判断是不是User控件
                        if (myuser)
                        {
                            // 确实是User 控件
                            if(i==row-1)
                            {
                                msg+=myuser->CopyAllInCompList();
                            }
                            else {
                                msg+=myuser->CopyAllInCompList()+"\n";
                            }
                        }
                    }
                }
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(msg);
                QMessageBox::information(this,"提醒","团支部全部未完成人员名单已复制到粘贴板！");
            });
            //导入人员数据
            connect(ui->btnImport,&QPushButton::clicked,this,[=]{
                QString filePath = QFileDialog::getOpenFileName(nullptr, tr("选择xlsx文件"), "", tr("Excel Files (*.xlsx)"));
                // 打开 Excel 文件
                QXlsx::Document xlsx(filePath);
                // 获取工作表对象
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;

                QMap<QString,QVector<QString>>shuju;
                QStringList initIdList,initEmailList;
                for(int i=1;i<=lastRow;++i)
                {
                    QString bj = worksheet->read(i,1).toString();
                    initIdList.append(worksheet->read(i,2).toString());
                    QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                    QString name = worksheet->read(i,3).toString();
                    initEmailList.append(worksheet->read(i,4).toString());
                    //        qDebug()<<"团支部："<<tzb<<" 姓名："<<name;
                    shuju[tzb].push_back(name);
                }

                for(int i=0;i<shuju.size();++i)
                {
                    QSqlQuery xp;
                    //查询团支部是否存在
                    if(xp.exec(QString("select *from userlist where user='%1'").arg(shuju.keys()[i])))
                    {
                        //团支部不存在
                        if(!xp.next())
                        {
                            //插入账号
                            if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(shuju.keys()[i])))
                            {
                                QMessageBox::information(this,"插入账号提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                return;
                            }
                            //创建团支部表
                            if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(shuju.keys()[i])))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                return;
                            }
                        }
                        else{
                            qDebug()<<"团支部："<<shuju.keys()[i]<<" 已存在";
                        }
                    }

                    //插入团支部成员信息
                    for (int j=0;j<shuju.value(shuju.keys()[i]).size();++j)
                    {
                        if(xp.exec(QString("select *from %1 where id='%2';").arg(shuju.keys()[i]).arg(initIdList[j])))
                        {
                            if(!xp.next())
                            {
                                //不存在则插入
                                if(!xp.exec(QString("INSERT INTO %1 VALUES ('%2','%3','%4');").arg(shuju.keys()[i]).arg(initIdList[j]).arg(shuju.value(shuju.keys()[i])[j]).arg(initEmailList[j])))
                                {
                                    QMessageBox::information(this,"插入团支部成员信息提示","插入团支部成员信息失败:\n"+xp.lastError().text());
                                    return;
                                }
                            }
                            else
                            {
                                //存在则修改
                                if(!xp.exec(QString("update %1 set name='%2',email='%3' where id='%4';").arg(shuju.keys()[i]).arg(shuju.value(shuju.keys()[i])[j]).arg(initEmailList[j]).arg(initIdList[j])))
                                {
                                    QMessageBox::information(this,"修改团支部成员信息提示","修改团支部成员信息失败:\n"+xp.lastError().text());
                                    return;
                                }
                            }
                        }
                        else {
                            QMessageBox::information(this,"查询团支部成员信息提示","查询团支部成员信息失败:\n"+xp.lastError().text());
                            return;
                        }
                    }
                    xp.exec(QString("select *from %1;").arg(shuju.keys()[i]));
                    while (xp.next()) qDebug()<<xp.value(0).toString()<<" "<<xp.value(1).toString()<<" "<<xp.value(2).toString();
                }
                QMessageBox::information(this,"导入人员数据提示","导入人员数据成功！");
            });
            //配置邮箱
            connect(ui->btnConfigEmailTeacher,&QPushButton::clicked,this,[=]{
                ConfigEmail *configEmail=new ConfigEmail;
                connect(configEmail,&ConfigEmail::returnMainPage,this,[=]{configEmail->close();configEmail->deleteLater();});
                configEmail->show();
            });
            //邮箱提醒
            connect(ui->btnEmailTeacher,&QPushButton::clicked,this,[=]{
                QSqlQuery xp;
                if(!xp.exec("select *from email;"))
                {
                    QMessageBox::information(this,"查询邮箱是否初始化-提醒","查询邮箱是否初始化失败！\n"+xp.lastError().text());
                    return;
                }
                xp.next();
                QString sendUser=xp.value(0).toString();
                QString sendPwd=xp.value(1).toString();
                if(sendUser==nullptr||sendPwd==nullptr)
                {
                    QMessageBox::information(this,"邮箱初始化-提醒","请先配置发送邮箱账号和密钥！");
                    return;
                }
                //检查邮箱信息是否可用
                PSmtp*  smtpTest= new PSmtp;
                smtpTest->setTestUser(true);
                connect(smtpTest,&PSmtp::testUserState,this,[=](bool ok){
                    if(!ok) //邮箱账号不可用
                    {
                        QMessageBox::information(this,"邮箱配置不可用-提示","邮箱配置不可用!!!\n请重新配置!!!");
                        return;
                    }
                    //邮箱账号可用
                    QSqlQuery xp;
                    QStringList sendNameList,sendTzbList,sendEmailList;
                    //遍历每个班未完成人员
                    int row=ui->UsersTableWidget->rowCount();
                    for (int i=0;i<row;++i) {
                        QWidget *widget = ui->UsersTableWidget->cellWidget(i, 0);
                        if (widget)
                        {
                            User *myuser = qobject_cast<User *>(widget); //判断是不是User控件
                            if (myuser)
                            {
                                // 确实是User 控件
                                QStringList l=myuser->CopyAllInCompList().split('\n');
                                QString tzb=l[0].split(' ').at(0);
                                l.removeAt(0);
                                for(QString n:l)
                                {
                                    if(nullptr!=n)
                                    {
                                        sendNameList.append(n);
                                        sendTzbList.append(tzb);
                                    }
                                }
                            }
                        }
                    }
                    qDebug()<<sendNameList;
                    //本支部人员是否已完成
                    if(sendNameList.count()>0)
                    {
                        //遍历查询未完成人员邮箱(同班重名情况未处理)
                        for (int i=0;i<sendNameList.count();++i)
                        {
                            //查询邮箱失败
                            if(!xp.exec(QString("select email from %1 where name='%2';").arg(sendTzbList[i]).arg(sendNameList[i])))
                            {
                                QMessageBox::information(this,"查询未完成人员邮箱提示","查询未完成人员邮箱失败！\n"+xp.lastError().text());
                                return;
                            }
                            else //查询邮箱成功
                            {
                                if(xp.next()) sendEmailList.append(xp.value(0).toString()); //邮箱信息存在
                            }
                        }
                        //开始遍历发送提醒邮箱
                        for (int i=0;i<sendNameList.count();++i)
                        {
                            if(sendEmailList[i].isEmpty())
                            {
                                qDebug()<<sendNameList[i]<<" 邮箱为空！";
                            }
                            else {
                                //异步执行sendEmail函数
                                PSmtp *smtp= new PSmtp;//默认是qq邮箱服务器，想用别的服务器就 new PSmtp(Smtp服务器地址)
                                connect(smtp, SIGNAL(disconnected()), smtp, SLOT(deleteLater()));	//发送完毕自行销毁
                                connect(smtp, SIGNAL(emailStatus(int)), this, SLOT(emailStatusSlot(int)));//状态

                                QMetaObject::invokeMethod(smtp, "sendEmailSlot", Qt::QueuedConnection,
                                                          Q_ARG(QString,sendUser),
                                                          Q_ARG(QString,sendPwd),
                                                          Q_ARG(QString,sendEmailList[i]),
                                                          Q_ARG(QString,"青年大学习完成提醒"),
                                                          Q_ARG(QString,QString("%1同学，你好！\n  请尽快完成本周青年大学习谢谢！").arg(sendNameList[i])));
                            }
                        }
                        QMessageBox::information(this,"邮箱发送成功-提示","邮箱提醒发送成功！(学生信息邮箱为空除外)");
                    }
                    else
                    {
                        QMessageBox::information(this,"邮箱提醒-提示","本支部成员已全部完成,无需进行邮箱提醒！");
                        return;
                    }
                });
                //测试账号是否可用
                smtpTest->testUserIsOk(sendUser,sendPwd);
            });
            //切换账号
            connect(ui->btnExit,&QPushButton::clicked,this,[=]{
                ui->UsersName->clear();
                ui->schedule_2->clear();
                int rowCount = ui->UsersTableWidget->rowCount();
                for (int i = rowCount - 1; i >= 0; --i) {
                    ui->UsersTableWidget->removeRow(i);
                }
                Users.clear();
                atomicInt=0;
                this->hide();
                login->againLogin();
                login->show();
            });
        }
        login->hide();
        ui->stackedWidget->setCurrentIndex(1);
        this->show();
        movie = new QMovie(":/ptr/load.gif");
        loadLabel = new QLabel;
        loadLabel->setMovie(movie);
        movie->start();
        loadLabel->setParent(ui->UsersTableWidget);
        loadLabel->resize(200,200);
        loadLabel->show();
        ui->btnReQuery->setEnabled(false);
        ui->btnCopyAllInCompList->setEnabled(false);
        ui->btnImport->setEnabled(false);
        ui->btnConfigEmailTeacher->setEnabled(false);
        ui->btnEmailTeacher->setEnabled(false);
        ui->btnExit->setEnabled(false);
        loadLabel->move(QPoint(ui->UsersTableWidget->pos()+QPoint(ui->UsersTableWidget->width()/2-loadLabel->width()/2,ui->UsersTableWidget->height()/2-loadLabel->height())));
        //初始化旗下支部控件及信息
        for(QString msg:Users)
        {
            User *myuser=new User;
            connect(myuser,&User::allSuccess,this,[=]{++atomicInt;ui->schedule_2->setText(QString("%1/%2").arg(atomicInt).arg(Users.count()));});
            connect(myuser,&User::loadSelect,this,[=]{
                if(atomicInt!=0)  --atomicInt;
                ui->schedule_2->setText(QString("%1/%2").arg(atomicInt).arg(Users.count()));
            });
            connect(myuser,&User::updateSnum2,this,[=](QString snum2){ui->snum2->setText(snum2);});
            int nowRow=ui->UsersTableWidget->rowCount();
            ui->UsersTableWidget->insertRow(nowRow);
            ui->UsersTableWidget->setRowHeight(nowRow,65);
            ui->UsersTableWidget->setCellWidget(nowRow,0,myuser);
            myuser->UserWorking(msg.split(' ').at(0),msg.split(' ').at(1));
        }

        loadLabel->hide();
        ui->btnReQuery->setEnabled(true);
        ui->btnCopyAllInCompList->setEnabled(true);
        ui->btnImport->setEnabled(true);
        ui->btnConfigEmailTeacher->setEnabled(true);
        ui->btnEmailTeacher->setEnabled(true);
        ui->btnExit->setEnabled(true);

    });
    //学院管理员登录信号
    connect(login,&Login::loginCollegeAdmin,this,[=](QStringList users,QString team){
        Users=users;
        ui->UsersName->setText(team);
        //============================老师TableWidget初始化============================
        {
            QString horQss="QTableView QHeaderView{background-color:rgb(240,207,214);}"
                           "QTableView QHeaderView::section{color: rgb(122, 175, 227);"
                           "border-right: 1px solid rgb(240,207,214);border-left: 1px solid rgb(240,207,214);"
                           "font-weight:bold;height: 35px;}";
            QString scrollBarStyle = "QScrollBar { background: rgb(240,207,214); width: 12px; }"
                                     "QScrollBar::handle { background: rgba(136, 193, 150,0.5); border: 0px solid rgb(255, 255, 255); border-radius: 5px; }"
                                     "QScrollBar::handle:hover { background: rgba(136, 193, 150,0.7); }"
                                     "QScrollBar::sub-line, QScrollBar::add-line { background: rgb(210,225,243); }"
                                     "QScrollBar::sub-page:vertical, QScrollBar::add-page:vertical { background: rgb(210,225,243); }";
            ui->UsersTableWidget->horizontalHeader()->setVisible(true);//显示水平头
            ui->UsersTableWidget->verticalHeader()->setVisible(false);//显示垂直头
            ui->UsersTableWidget->horizontalHeader()->setStretchLastSection(true);
            ui->UsersTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//用户不能直接在表格或列表的单元格中进行编辑
            ui->UsersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);// 设置表头自适应列宽
            ui->UsersTableWidget->horizontalHeader()->setStyleSheet(horQss);
            ui->UsersTableWidget->verticalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->UsersTableWidget->horizontalScrollBar()->setStyleSheet(scrollBarStyle);
            ui->schedule_2->setText(QString("0/%1").arg(Users.count()));
        }
        //============================老师按钮初始化============================
        {
            //一键重新查询
            connect(ui->btnReQuery,&QPushButton::clicked,this,[=]{
                loadLabel->show();
                ui->btnReQuery->setEnabled(false);
                ui->btnCopyAllInCompList->setEnabled(false);
                ui->btnImport->setEnabled(false);
                ui->btnConfigEmailTeacher->setEnabled(false);
                ui->btnEmailTeacher->setEnabled(false);
                ui->btnExit->setEnabled(false);
                loadLabel->move(QPoint(ui->UsersTableWidget->pos()+QPoint(ui->UsersTableWidget->width()/2-loadLabel->width()/2,ui->UsersTableWidget->height()/2-loadLabel->height())));

                int row=ui->UsersTableWidget->rowCount();
                for (int i=0;i<row;++i) {
                    QWidget *widget = ui->UsersTableWidget->cellWidget(i, 0);
                    if (widget)
                    {
                        User *myuser = qobject_cast<User *>(widget); //判断是不是User控件
                        if (myuser)
                        {
                            // 确实是User 控件
                            myuser->ReQuery();
                        }
                    }
                }
                loadLabel->hide();
                ui->btnReQuery->setEnabled(true);
                ui->btnCopyAllInCompList->setEnabled(true);
                ui->btnImport->setEnabled(true);
                ui->btnConfigEmailTeacher->setEnabled(true);
                ui->btnEmailTeacher->setEnabled(true);
                ui->btnExit->setEnabled(true);
            });
            //复制全部未完成名单
            connect(ui->btnCopyAllInCompList,&QPushButton::clicked,this,[=]{
                QString msg;
                int row=ui->UsersTableWidget->rowCount();
                for (int i=0;i<row;++i) {
                    QWidget *widget = ui->UsersTableWidget->cellWidget(i, 0);
                    if (widget)
                    {
                        User *myuser = qobject_cast<User *>(widget); //判断是不是User控件
                        if (myuser)
                        {
                            // 确实是User 控件
                            if(i==row-1)
                            {
                                msg+=myuser->CopyAllInCompList();
                            }
                            else {
                                msg+=myuser->CopyAllInCompList()+"\n";
                            }
                        }
                    }
                }
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(msg);
                QMessageBox::information(this,"提醒","团支部全部未完成人员名单已复制到粘贴板！");
            });
            //导入人员数据
            connect(ui->btnImport,&QPushButton::clicked,this,[=]{
                QString filePath = QFileDialog::getOpenFileName(nullptr, tr("选择xlsx文件"), "", tr("Excel Files (*.xlsx)"));
                // 打开 Excel 文件
                QXlsx::Document xlsx(filePath);
                // 获取工作表对象
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;

                QMap<QString,QVector<QString>>shuju;
                QStringList initIdList,initEmailList;
                for(int i=1;i<=lastRow;++i)
                {
                    QString bj = worksheet->read(i,1).toString();
                    initIdList.append(worksheet->read(i,2).toString());
                    QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                    QString name = worksheet->read(i,3).toString();
                    initEmailList.append(worksheet->read(i,4).toString());
                    //        qDebug()<<"团支部："<<tzb<<" 姓名："<<name;
                    shuju[tzb].push_back(name);
                }

                for(int i=0;i<shuju.size();++i)
                {
                    QSqlQuery xp;
                    //查询团支部是否存在
                    if(xp.exec(QString("select *from userlist where user='%1'").arg(shuju.keys()[i])))
                    {
                        //团支部不存在
                        if(!xp.next())
                        {
                            //插入账号
                            if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(shuju.keys()[i])))
                            {
                                QMessageBox::information(this,"插入账号提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                return;
                            }
                            //创建团支部表
                            if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(shuju.keys()[i])))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                return;
                            }
                        }
                        else{
                            qDebug()<<"团支部："<<shuju.keys()[i]<<" 已存在";
                        }
                    }

                    //插入团支部成员信息
                    for (int j=0;j<shuju.value(shuju.keys()[i]).size();++j)
                    {
                        if(xp.exec(QString("select *from %1 where id='%2';").arg(shuju.keys()[i]).arg(initIdList[j])))
                        {
                            if(!xp.next())
                            {
                                //不存在则插入
                                if(!xp.exec(QString("INSERT INTO %1 VALUES ('%2','%3','%4');").arg(shuju.keys()[i]).arg(initIdList[j]).arg(shuju.value(shuju.keys()[i])[j]).arg(initEmailList[j])))
                                {
                                    QMessageBox::information(this,"插入团支部成员信息提示","插入团支部成员信息失败:\n"+xp.lastError().text());
                                    return;
                                }
                            }
                            else
                            {
                                //存在则修改
                                if(!xp.exec(QString("update %1 set name='%2',email='%3' where id='%4';").arg(shuju.keys()[i]).arg(shuju.value(shuju.keys()[i])[j]).arg(initEmailList[j]).arg(initIdList[j])))
                                {
                                    QMessageBox::information(this,"修改团支部成员信息提示","修改团支部成员信息失败:\n"+xp.lastError().text());
                                    return;
                                }
                            }
                        }
                        else {
                            QMessageBox::information(this,"查询团支部成员信息提示","查询团支部成员信息失败:\n"+xp.lastError().text());
                            return;
                        }
                    }
                    xp.exec(QString("select *from %1;").arg(shuju.keys()[i]));
                    while (xp.next()) qDebug()<<xp.value(0).toString()<<" "<<xp.value(1).toString()<<" "<<xp.value(2).toString();
                }
                QMessageBox::information(this,"导入人员数据提示","导入人员数据成功！");
            });
            //配置邮箱
            connect(ui->btnConfigEmailTeacher,&QPushButton::clicked,this,[=]{
                ConfigEmail *configEmail=new ConfigEmail;
                connect(configEmail,&ConfigEmail::returnMainPage,this,[=]{configEmail->close();configEmail->deleteLater();});
                configEmail->show();
            });
            //邮箱提醒
            connect(ui->btnEmailTeacher,&QPushButton::clicked,this,[=]{
                QSqlQuery xp;
                if(!xp.exec("select *from email;"))
                {
                    QMessageBox::information(this,"查询邮箱是否初始化-提醒","查询邮箱是否初始化失败！\n"+xp.lastError().text());
                    return;
                }
                xp.next();
                QString sendUser=xp.value(0).toString();
                QString sendPwd=xp.value(1).toString();
                if(sendUser==nullptr||sendPwd==nullptr)
                {
                    QMessageBox::information(this,"邮箱初始化-提醒","请先配置发送邮箱账号和密钥！");
                    return;
                }
                //检查邮箱信息是否可用
                PSmtp*  smtpTest= new PSmtp;
                smtpTest->setTestUser(true);
                connect(smtpTest,&PSmtp::testUserState,this,[=](bool ok){
                    if(!ok) //邮箱账号不可用
                    {
                        QMessageBox::information(this,"邮箱配置不可用-提示","邮箱配置不可用!!!\n请重新配置!!!");
                        return;
                    }
                    //邮箱账号可用
                    QSqlQuery xp;
                    QStringList sendNameList,sendTzbList,sendEmailList;
                    //遍历每个班未完成人员
                    int row=ui->UsersTableWidget->rowCount();
                    for (int i=0;i<row;++i) {
                        QWidget *widget = ui->UsersTableWidget->cellWidget(i, 0);
                        if (widget)
                        {
                            User *myuser = qobject_cast<User *>(widget); //判断是不是User控件
                            if (myuser)
                            {
                                // 确实是User 控件
                                QStringList l=myuser->CopyAllInCompList().split('\n');
                                QString tzb=l[0].split(' ').at(0);
                                l.removeAt(0);
                                for(QString n:l)
                                {
                                    if(nullptr!=n)
                                    {
                                        sendNameList.append(n);
                                        sendTzbList.append(tzb);
                                    }
                                }
                            }
                        }
                    }
                    qDebug()<<sendNameList;
                    //本支部人员是否已完成
                    if(sendNameList.count()>0)
                    {
                        //遍历查询未完成人员邮箱(同班重名情况未处理)
                        for (int i=0;i<sendNameList.count();++i)
                        {
                            //查询邮箱失败
                            if(!xp.exec(QString("select email from %1 where name='%2';").arg(sendTzbList[i]).arg(sendNameList[i])))
                            {
                                QMessageBox::information(this,"查询未完成人员邮箱提示","查询未完成人员邮箱失败！\n"+xp.lastError().text());
                                return;
                            }
                            else //查询邮箱成功
                            {
                                if(xp.next()) sendEmailList.append(xp.value(0).toString()); //邮箱信息存在
                            }
                        }
                        //开始遍历发送提醒邮箱
                        for (int i=0;i<sendNameList.count();++i)
                        {
                            if(sendEmailList[i].isEmpty())
                            {
                                qDebug()<<sendNameList[i]<<" 邮箱为空！";
                            }
                            else {
                                //异步执行sendEmail函数
                                PSmtp *smtp= new PSmtp;//默认是qq邮箱服务器，想用别的服务器就 new PSmtp(Smtp服务器地址)
                                connect(smtp, SIGNAL(disconnected()), smtp, SLOT(deleteLater()));	//发送完毕自行销毁
                                connect(smtp, SIGNAL(emailStatus(int)), this, SLOT(emailStatusSlot(int)));//状态

                                QMetaObject::invokeMethod(smtp, "sendEmailSlot", Qt::QueuedConnection,
                                                          Q_ARG(QString,sendUser),
                                                          Q_ARG(QString,sendPwd),
                                                          Q_ARG(QString,sendEmailList[i]),
                                                          Q_ARG(QString,"青年大学习完成提醒"),
                                                          Q_ARG(QString,QString("%1同学，你好！\n  请尽快完成本周青年大学习谢谢！").arg(sendNameList[i])));
                            }
                        }
                        QMessageBox::information(this,"邮箱发送成功-提示","邮箱提醒发送成功！(学生信息邮箱为空除外)");
                    }
                    else
                    {
                        QMessageBox::information(this,"邮箱提醒-提示","本支部成员已全部完成,无需进行邮箱提醒！");
                        return;
                    }
                });
                //测试账号是否可用
                smtpTest->testUserIsOk(sendUser,sendPwd);
            });
            //切换账号
            connect(ui->btnExit,&QPushButton::clicked,this,[=]{
                ui->UsersName->clear();
                ui->schedule_2->clear();
                int rowCount = ui->UsersTableWidget->rowCount();
                for (int i = rowCount - 1; i >= 0; --i) {
                    ui->UsersTableWidget->removeRow(i);
                }
                Users.clear();
                atomicInt=0;
                this->hide();
                login->againLogin();
                login->show();
            });
        }
        login->hide();
        ui->stackedWidget->setCurrentIndex(1);
        this->show();
        movie = new QMovie(":/ptr/load.gif");
        loadLabel = new QLabel;
        loadLabel->setMovie(movie);
        movie->start();
        loadLabel->setParent(ui->UsersTableWidget);
        loadLabel->resize(200,200);
        loadLabel->show();
        ui->btnReQuery->setEnabled(false);
        ui->btnCopyAllInCompList->setEnabled(false);
        ui->btnImport->setEnabled(false);
        ui->btnConfigEmailTeacher->setEnabled(false);
        ui->btnEmailTeacher->setEnabled(false);
        ui->btnExit->setEnabled(false);
        loadLabel->move(QPoint(ui->UsersTableWidget->pos()+QPoint(ui->UsersTableWidget->width()/2-loadLabel->width()/2,ui->UsersTableWidget->height()/2-loadLabel->height())));
        //初始化旗下支部控件及信息
        for(QString msg:Users)
        {
            User *myuser=new User;
            connect(myuser,&User::allSuccess,this,[=]{++atomicInt;ui->schedule_2->setText(QString("%1/%2").arg(atomicInt).arg(Users.count()));});
            connect(myuser,&User::loadSelect,this,[=]{
                if(atomicInt!=0)  --atomicInt;
                ui->schedule_2->setText(QString("%1/%2").arg(atomicInt).arg(Users.count()));
            });
            connect(myuser,&User::updateSnum2,this,[=](QString snum2){ui->snum2->setText(snum2);});
            int nowRow=ui->UsersTableWidget->rowCount();
            ui->UsersTableWidget->insertRow(nowRow);
            ui->UsersTableWidget->setRowHeight(nowRow,65);
            ui->UsersTableWidget->setCellWidget(nowRow,0,myuser);
            myuser->UserWorking(msg.split(' ').at(0),msg.split(' ').at(1));
        }

        loadLabel->hide();
        ui->btnReQuery->setEnabled(true);
        ui->btnCopyAllInCompList->setEnabled(true);
        ui->btnImport->setEnabled(true);
        ui->btnConfigEmailTeacher->setEnabled(true);
        ui->btnEmailTeacher->setEnabled(true);
        ui->btnExit->setEnabled(true);
    });
    //录入其他账号
    connect(login,&Login::inputOtherUser,this,[=]{
        initGetMsg *inputUser=new initGetMsg;
        connect(inputUser,&initGetMsg::returnLogin,this,[=]{
            inputUser->close();
            inputUser->deleteLater();
            login->show();
        });
        connect(inputUser,&initGetMsg::initGetMsgOK,this,[=](QString cshUser,QString cshPwd,QString cshIdentity,QString filePath){
            if("团支书"==cshIdentity)
            {
                QXlsx::Document xlsx(filePath);// 打开 Excel 文件
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();// 获取工作表对象
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                QStringList cshIdList,cshNameList,cshEmailList;
                for(int i=1;i<=lastRow;++i)
                {
                    cshIdList.append(worksheet->read(i,1).toString());
                    cshNameList.append(worksheet->read(i,2).toString());
                    cshEmailList.append(worksheet->read(i,3).toString());
                }
                QSqlQuery axp;
                //查询账号是否存在
                if(axp.exec(QString("select *from userlist where user='%1';").arg(cshUser)))
                {
                    if(!axp.next())//账号不存在，则插入
                    {
                        //插入账号
                        if(!axp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                        {
                            QMessageBox::information(this,"插入账号-提示",QString("插入账号失败:\n%1").arg(axp.lastError().text()));
                            return;
                        }
                    }
                    else {
                        QMessageBox::information(this,"插入账号-提示",QString("插入账号失败,账号已经存在！"));
                        return;
                    }
                }

                //查询团支部是否存在
                if(axp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(cshUser)))
                {
                    if(!axp.next())//如果不存在，则插入
                    {
                        //创建团支部表
                        if(!axp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(cshUser)))
                        {
                            QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+axp.lastError().text());
                            return;
                        }
                    }
                }
                else {
                    QMessageBox::information(this,"查询团支部是否存在-提示","查询团支部是否存在失败:\n"+axp.lastError().text());
                    return;
                }

                //插入团支部成员信息
                for(int i=0;i<cshNameList.size();++i)
                {
                    QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                              Q_ARG(QString,cshUser),
                                              Q_ARG(QString,cshIdList[i]),
                                              Q_ARG(QString,cshNameList[i]),
                                              Q_ARG(QString,cshEmailList[i]));
                }
            }
            else if ("老师"==cshIdentity)
            {
                // 打开 Excel 文件
                QXlsx::Document xlsx(filePath);
                // 获取工作表对象
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                QStringList initTzbList,initIdList,initNameList,initEmailList;
                for(int i=1;i<=lastRow;++i)
                {
                    QString bj = worksheet->read(i,1).toString();
                    initIdList.append(worksheet->read(i,2).toString());
                    QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                    initTzbList.append(tzb);
                    initNameList.append(worksheet->read(i,3).toString());
                    initEmailList.append(worksheet->read(i,4).toString());
                }
                QSet<QString>tzbSet(initTzbList.toSet());
                //遍历所有团支部
                for(QString t:tzbSet)
                {
                    QSqlQuery xp;
                    //查询团支部账号是否存在
                    if(xp.exec(QString("select *from userlist where user='%1'").arg(t)))
                    {
                        if(!xp.next())//不存在则插入
                        {
                            //插入账号
                            if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(t)))
                            {
                                QMessageBox::information(this,"插入账号提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                return;
                            }
                        }
                    }
                    else {
                        QMessageBox::information(this,"查询账号(老师)-提示",QString("查询账号(老师)失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }

                    //查询团支部表是否存在
                    if(xp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(t)))
                    {
                        if(!xp.next())
                        {
                            //创建团支部表
                            if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(t)))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                return;
                            }
                        }
                    }
                    else {
                        QMessageBox::information(this,"查询团支部表是否存在(老师)-提示",QString("查询团支部表是否存在(老师)失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }
                }
                //插入团支部成员信息
                for (int i=0;i<initIdList.size();++i)
                {
                    QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                              Q_ARG(QString,initTzbList[i]),
                                              Q_ARG(QString,initIdList[i]),
                                              Q_ARG(QString,initNameList[i]),
                                              Q_ARG(QString,initEmailList[i]));
                }
                QSqlQuery axp;
                //插入老师账号
                if(!axp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                {
                    QMessageBox::information(this,"插入老师账号提示",QString("插入老师账号失败:\n%1").arg(axp.lastError().text()));
                    return;
                }
            }
            else if ("学院管理员"==cshIdentity) {
                QXlsx::Document xlsx(filePath);// 打开 Excel 文件
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet(); // 获取工作表对象
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;
                QStringList initTzbList,initIdList,initNameList,initEmailList;
                QSet<QString>tzbSet;
                for(int i=1;i<=lastRow;++i)
                {
                    QString bj = worksheet->read(i,1).toString();
                    initIdList.append(worksheet->read(i,2).toString());
                    QString tzb= bj.left(bj.length() - 5)+"20"+bj.right(5).left(2)+QString("级%1班团支部").arg(bj.right(2).toInt());
                    QString name = worksheet->read(i,3).toString();
                    initTzbList.append(tzb);
                    tzbSet.insert(tzb);
                    initNameList.append(name);
                    initEmailList.append(worksheet->read(i,4).toString());
                    qDebug()<<"团支部："<<tzb<<" 学号： "<<worksheet->read(i,2).toString()<<" 姓名："<<name<<" 邮箱："<<worksheet->read(i,4).toString();
                }
                QSqlQuery xp;
                //创建学院表
                if(!xp.exec(QString("CREATE TABLE %1 ('name' varchar(255) NOT NULL);").arg(cshUser)))
                {
                    QMessageBox::information(this,"创建学院表提示","创建学院表失败:\n"+xp.lastError().text());
                    return;
                }
                //遍历所有团支部
                for(QString t:tzbSet)
                {
                    //查询对应团支书账号是否存在
                    if(xp.exec(QString("select *from userlist where user='%1'").arg(t)))
                    {
                        if(!xp.next())//如果不存在则插入
                        {
                            //插入对应团支书账号
                            if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','123456','团支书');").arg(t)))
                            {
                                QMessageBox::information(this,"插入账号-提示",QString("插入账号失败:\n%1").arg(xp.lastError().text()));
                                return;
                            }
                        }
                    }
                    else
                    {
                        QMessageBox::information(this,"查询对应团支书账号-提示",QString("查询对应团支书账号失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }

                    //查询对应团支部表是否存在
                    if(xp.exec(QString("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';").arg(t)))
                    {
                        if(!xp.next())//不存在则创建
                        {
                            //创建对应团支部表
                            if(!xp.exec(QString("CREATE TABLE %1 ('id' varchar(255) NOT NULL,'name' varchar(255) NOT NULL,'email' varchar(255));").arg(t)))
                            {
                                QMessageBox::information(this,"创建团支部表提示","创建团支部表失败:\n"+xp.lastError().text());
                                return;
                            }
                        }
                    }
                    else
                    {
                        QMessageBox::information(this,"查询对应团支部表-提示",QString("查询对应团支部表失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }

                    //插入学院表中对应团支部
                    if(!xp.exec(QString("INSERT INTO %1 VALUES ('%2');").arg(cshUser).arg(t)))
                    {
                        QMessageBox::information(this,"插入学院表中对应团支部-提示",QString("插入学院表中对应团支部失败:\n%1").arg(xp.lastError().text()));
                        return;
                    }
                }
                //插入团支部对应成员信息
                for (int i=0;i<initIdList.size();++i)
                {
                    QMetaObject::invokeMethod(this, "insertTzbCY", Qt::QueuedConnection,
                                              Q_ARG(QString,initTzbList[i]),
                                              Q_ARG(QString,initIdList[i]),
                                              Q_ARG(QString,initNameList[i]),
                                              Q_ARG(QString,initEmailList[i]));
                }
                //插入学院管理员账号
                if(!xp.exec(QString("INSERT INTO userlist VALUES ('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity)))
                {
                    QMessageBox::information(this,"插入学院管理员账号提示",QString("插入学院管理员账号失败:\n%1").arg(xp.lastError().text()));
                    return;
                }
            }
            QSqlQuery p;
            p.exec("INSERT INTO init VALUES ('已经初始化');");
            p.exec("INSERT INTO email VALUES ('','');");
            p.exec(QString("INSERT INTO defaultmsg VALUES('%1','%2','%3');").arg(cshUser).arg(cshPwd).arg(cshIdentity));
            inputUser->close();
            inputUser->deleteLater();
            login->setInitMsg(cshUser,cshPwd,cshIdentity);
            login->show();
        });
        login->hide();
        inputUser->setReturnEnabled(true);
        inputUser->show();
    });

}
//查询xlsx文件行数
int mainWidget::findLastRowWithDataInColumnA(Worksheet *worksheet)
{
    int lastRow = 0; // 初始化行号为0，假设没有数据
    for (int row = 1; row <= worksheet->dimension().lastRow(); ++row) {
        if (worksheet->read(row, 0).toString().isEmpty()) { // 读取A列的单元格，列索引为0
            lastRow = row; // 更新最后一个有数据的行号
        }
    }
    return lastRow;
}
//初始化需完成人员名单
bool mainWidget::initCompList()
{
    //读取需完成人员名单
    QSqlQuery xp;
    if(!xp.exec(QString("select name from %1;").arg(user)))
    {
        QMessageBox::information(this,"初始化人员名单提示","初始化人员名单失败！\n"+xp.lastError().text());
        return false;
    }
    while(xp.next())
    {
        NeedCompList.append(xp.value(0).toString());
    }
    return true;
}
//Post请求(获取orgId,token)
void mainWidget::startRequestPost(const QUrl &requestedUrl)
{
    url=requestedUrl;
    //1.json格式传输报文
    QJsonObject json;
    QJsonDocument jsonDoc;
    //构建json数据
    json["username"] = user;
    json["password"] = pwd;
    jsonDoc.setObject(json);
    QByteArray dataArray = jsonDoc.toJson(QJsonDocument::Compact);
    //2.构造URL&&请求头
    //构造请求头
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json;charset=UTF-8"));
    //3.post数据
    manager = new QNetworkAccessManager(this);
    reply = manager->post(req,dataArray);
    QEventLoop eventLoop;
    connect(manager,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    connect(reply,&QNetworkReply::finished,this,&mainWidget::replyFinishedPost);
    eventLoop.exec();
}
//Post请求2(获取id下载号,name年,snum期数)
void mainWidget::startRequestPost2(const QUrl &requestedUrl)
{
    url=requestedUrl;
    //1.json格式传输报文
    QJsonObject json;
    QJsonDocument jsonDoc;
    //构建json数据
    json["pageNo"] = 1;
    json["pageSize"] = 500;
    jsonDoc.setObject(json);
    QByteArray dataArray = jsonDoc.toJson(QJsonDocument::Compact);
    //2.构造URL&&请求头
    //构造请求头
    req2.setUrl(url);
    req2.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json;charset=UTF-8"));
    //3.post数据
    manager2 = new QNetworkAccessManager(this);
    reply2 = manager2->post(req2,dataArray);
    QEventLoop eventLoop;
    connect(manager2,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    connect(reply2,&QNetworkReply::finished,this,&mainWidget::replyFinishedPost2);
    eventLoop.exec();
}
//Get请求
void mainWidget::startRequestGet(const QUrl &requestedUrl)
{
    url=requestedUrl;
    manager3 = new QNetworkAccessManager(this);
    req3.setUrl(url);
    //由于请求发生重定向，所以一定要加上这行代码，设置自动跳转，否则会返回 302
    req3.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    //get方式发送请求
    reply3 = manager3->get(req3);
    //    //将服务器的返回信号与replyFinished槽连接起来，当服务器返回消息时，会在槽里做相应操作
    connect(reply3,&QNetworkReply::finished,this,&mainWidget::replyFinishedGet);
}
//获取id下载号,name年,snum期数
void mainWidget::getIdNameSnum()
{
    if(token.isEmpty())
    {
        QMessageBox::information(this,"提示","getIdNameSnum时token为空！");
        return;
    }
    startRequestPost2(QString("https://dxx.scyol.com/backend/stages/select/list?token=%1").arg(token));
}
//查询显示未完成
void mainWidget::select()
{
    ui->btnXlsx->clicked();
}
//Post返回结果处理(获取orgId、token)
void mainWidget::replyFinishedPost()
{
    // <1>判断有没有错误
    if (reply->error()){
        qDebug()<<reply->errorString();
        reply->deleteLater();
        return;
    }
    // <2>检测网页返回状态码，常见是200，404等，200为成功
    int statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // <3>判断是否需要重定向
    if (statusCode >= 200 && statusCode <300){
        // ok 准备读数据
        QTextCodec *codec = QTextCodec::codecForName("utf8");
        QString all = codec->toUnicode(reply->readAll());
        //转为json格式
        QJsonDocument jsonDocument = QJsonDocument::fromJson(all.toUtf8().data());
        if (jsonDocument.isNull()){
            qDebug()<<"数据解析有误"; //判断是否解析出问题
        }
        QJsonObject jsonObject = jsonDocument.object();
        if(jsonObject["data"].isObject())
        {
            QJsonObject dataObj=jsonObject["data"].toObject();
            orgId=dataObj["orgId"].toInt();
            token=dataObj["token"].toString();
            qDebug()<<"orgId:"<<orgId<<" token:"<<token;
            if(orgId!=0&&!token.isEmpty())
                QMetaObject::invokeMethod(this, "getIdNameSnum", Qt::QueuedConnection);
            else
            {
                QMessageBox::information(this,"提示","orgId或token为空无法继续执行getIdNameSnum");
                return;
            }
        }
        // 数据读取完成之后，清除reply
        reply->deleteLater();
        reply = nullptr;

    }
    else if (statusCode >=300 && statusCode <400){
        // redirect
        // 获取重定向信息
        const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        // 检测是否需要重定向，如果不需要则读数据
        if (!redirectionTarget.isNull()) {
            const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
            reply->deleteLater();
            reply = nullptr;
            qDebug()<< "http redirect to " << redirectedUrl.toString();
            return;
        }
    }

}
//Post返回结果处理2(获取id、name、snum)
void mainWidget::replyFinishedPost2()
{
    // <1>判断有没有错误
    if (reply2->error()){
        qDebug()<<reply2->errorString();
        reply2->deleteLater();
        return;
    }
    // <2>检测网页返回状态码，常见是200，404等，200为成功
    int statusCode  = reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // <3>判断是否需要重定向
    if (statusCode >= 200 && statusCode <300){
        // ok 准备读数据
        QTextCodec *codec = QTextCodec::codecForName("utf8");
        QString all = codec->toUnicode(reply2->readAll());
        //转为json格式
        QJsonDocument jsonDocument = QJsonDocument::fromJson(all.toUtf8().data());
        if (jsonDocument.isNull())
        {
            qDebug()<<"数据解析有误"; //判断是否解析出问题
        }
        QJsonObject jsonObject = jsonDocument.object();
        if (jsonObject["data"].isArray())
        {
            QJsonArray dataArray=jsonObject["data"].toArray();
            QJsonValue dataArrayFrist=dataArray[0];
            if (dataArrayFrist.isObject()) {
                QJsonObject dataArrayFristObj = dataArrayFrist.toObject();
                if(dataArrayFristObj["snumList"].isArray())
                {
                    QJsonArray snumListArray=dataArrayFristObj["snumList"].toArray();
                    QJsonValue snumListArrayFrist=snumListArray[0];
                    if(snumListArrayFrist.isObject())
                    {
                        QJsonObject nowObj = snumListArrayFrist.toObject();
                        id=nowObj["id"].toInt();
                        name=nowObj["name"].toString();
                        snum=nowObj["snum"].toString();
                        qDebug()<<"id:"<<id<<"name:"<<name<<"snum:"<<snum;
                        ui->snum->setText(name+snum+" 完成进度:");
                        if(id!=0&&!name.isEmpty()&&!snum.isEmpty())
                        {
                            QMetaObject::invokeMethod(this, "select", Qt::QueuedConnection);
                        }
                        else {
                            QMessageBox::information(this,"提示",QString("id:%1 name:%2 snum:%3 数据异常无法下载xlsx文件！").arg(id).arg(name).arg(snum));
                            return;
                        }
                    }
                }
            }
            else
            {
                // 不是对象
                qDebug() << "解析获取当前期数失败！";
            }
        }
        // 数据读取完成之后，清除reply
        reply2->deleteLater();
        reply2 = nullptr;

    }
    else if (statusCode >=300 && statusCode <400){
        // redirect

        // 获取重定向信息
        const QVariant redirectionTarget = reply2->attribute(QNetworkRequest::RedirectionTargetAttribute);
        // 检测是否需要重定向，如果不需要则读数据
        if (!redirectionTarget.isNull()) {
            const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
            reply2->deleteLater();
            reply2 = nullptr;
            qDebug()<< "http redirect to " << redirectedUrl.toString();
            return;
        }
    }
}
//Get返回结果处理(获取xlsx数据)
void mainWidget::replyFinishedGet()
{
    int rowCount = ui->UserTableWidget->rowCount();
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->UserTableWidget->removeRow(i);
    }

    int rowCount2 = ui->InCompTableWidget->rowCount();
    for (int i = rowCount2 - 1; i >= 0; --i) {
        ui->InCompTableWidget->removeRow(i);
    }
    bool success = QFile::remove("学习用户明细.xlsx");
    if (success) {
        qDebug() << "学习用户明细.xlsx文件已删除";
    } else {
        qDebug() << "学习用户明细.xlsx删除文件失败";
    }
    // <1>判断有没有错误
    if (reply3->error()){
        qDebug()<<reply3->errorString();
        reply3->deleteLater();
        return;
    }
    // <2>检测网页返回状态码，常见是200，404等，200为成功
    int statusCode  = reply3->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // <3>判断是否需要重定向
    if (statusCode >= 200 && statusCode <300){
        // ok 准备读数据
        QFile file("学习用户明细.xlsx");
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open file for writing:" << file.errorString();
            return;
        }
        QByteArray byteArray=reply3->readAll(); //包含XLSX数据的QByteArray
        if (byteArray.size() > 0) {
            qint64 bytesWritten = file.write(byteArray);
            if (bytesWritten == -1) {
                qDebug() << "文件下载失败，原因如下:\n" << file.errorString();
            } else {
                qDebug() << bytesWritten << "bytes 文件下载成功！";
                file.close();
                QStringList incompList=NeedCompList;//拷贝需要完成的人员名单
                // 打开 Excel 文件
                QXlsx::Document xlsx("学习用户明细.xlsx");
                // 获取工作表对象
                QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();
                int lastRow=findLastRowWithDataInColumnA(worksheet);
                qDebug()<<"一共 "<<lastRow<<"条数据"<<endl;

                for(int i=2;i<=lastRow;++i)
                {
                    QVariant name = worksheet->read(i,1);
                    QVariant iphone = worksheet->read(i,2);
                    QVariant organization = worksheet->read(i,3);
                    QVariant time = worksheet->read(i,4);
                    int nowRow=ui->UserTableWidget->rowCount();
                    ui->UserTableWidget->insertRow(nowRow);//增加一行
                    ui->UserTableWidget->setRowHeight(nowRow,30);//设置行高度
                    QTableWidgetItem *nameItem=new QTableWidgetItem(name.toString());
                    nameItem->setTextAlignment(Qt::AlignCenter);
                    ui->UserTableWidget->setItem(nowRow,0,nameItem);
                    QTableWidgetItem *iphoneItem=new QTableWidgetItem(iphone.toString());
                    iphoneItem->setTextAlignment(Qt::AlignCenter);
                    ui->UserTableWidget->setItem(nowRow,1,iphoneItem);
                    QTableWidgetItem *organizationItem=new QTableWidgetItem(organization.toString());
                    organizationItem->setTextAlignment(Qt::AlignCenter);
                    ui->UserTableWidget->setItem(nowRow,2,organizationItem);
                    QTableWidgetItem *timeItem=new QTableWidgetItem(time.toString());
                    timeItem->setTextAlignment(Qt::AlignCenter);
                    ui->UserTableWidget->setItem(nowRow,3,timeItem);
                }

                // 获取单元格内容
                for(int i=2;i<=lastRow;++i)
                {
                    QVariant value = worksheet->read(i,1);
                    QString strValue=value.toString();
                    if(incompList.contains(strValue))
                        incompList.removeOne(strValue);
                    else{
                        incompList.append(strValue+"(非本支部)");
                    }
                }
                // 保存文件
                xlsx.save();
                for(auto a:incompList)
                {
                    int nowRow=ui->InCompTableWidget->rowCount();
                    ui->InCompTableWidget->insertRow(nowRow);//增加一行
                    ui->InCompTableWidget->setRowHeight(nowRow,30);//设置行高度
                    QTableWidgetItem *cidItem=new QTableWidgetItem(a);
                    cidItem->setTextAlignment(Qt::AlignCenter);
                    ui->InCompTableWidget->setItem(nowRow,0,cidItem);
                }
                ui->UserCount->setText(QString("共计 %1 条数据").arg(lastRow-1));
                ui->schedule->setText(QString("%1/%2").arg(lastRow-1).arg(NeedCompList.count()));
                loadLabel->hide();
                ui->btnEditList->setEnabled(true);
                ui->btnXlsx->setEnabled(true);
                ui->btnConfigEmail->setEnabled(true);
                ui->btnExitLogin->setEnabled(true);
                ui->btnCopyList->setEnabled(true);
                ui->btnEmail->setEnabled(true);
            }
        } else {
            qDebug() << "文件为空!";
        }
        // 数据读取完成之后，清除reply
        reply3->deleteLater();
        reply3 = nullptr;
    }
    else if (statusCode >=300 && statusCode <400)
    {
        // 获取重定向信息
        const QVariant redirectionTarget = reply3->attribute(QNetworkRequest::RedirectionTargetAttribute);
        // 检测是否需要重定向，如果不需要则读数据
        if (!redirectionTarget.isNull()) {
            const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
            reply3->deleteLater();
            reply3 = nullptr;
            qDebug()<< "http redirect to " << redirectedUrl.toString();
            return;
        }
    }

}
//插入团支部成员槽函数
void mainWidget::insertTzbCY(QString tzb, QString id, QString name, QString email)
{
    QSqlQuery querySelect;
    QSqlQuery queryUpdateOrInsert;
    // 查询成员是否存在
    if(querySelect.exec(QString("SELECT * FROM %1 WHERE id='%2';").arg(tzb).arg(id)))
    {
        if(!querySelect.next()) // 不存在则插入
        {
            qDebug() << "不存在，插入 "<<tzb<<" "<<id<<" "<<name<<" "<<email;
            // 假设表有三个字段：id, name, email
            queryUpdateOrInsert.prepare(QString("INSERT INTO %1 (id,name, email) VALUES (:id, :name, :email)").arg(tzb));
            queryUpdateOrInsert.bindValue(":id", id);
            queryUpdateOrInsert.bindValue(":name", name);
            queryUpdateOrInsert.bindValue(":email", email);
            if(!queryUpdateOrInsert.exec())
            {
                QMessageBox::information(nullptr, "插入团支部对应成员信息-提示", "插入团支部对应成员信息失败:\n" + queryUpdateOrInsert.lastError().text());
                return;
            }
        }
        else // 存在则修改
        {
            qDebug()<<"存在则修改！"<<tzb<<" "<<id<<" "<<name<<" "<<email;
            queryUpdateOrInsert.prepare(QString("UPDATE %1 SET name=:name, email=:email WHERE id=:id").arg(tzb));
            queryUpdateOrInsert.bindValue(":name", name);
            queryUpdateOrInsert.bindValue(":email", email);
            queryUpdateOrInsert.bindValue(":id", id);
            if(!queryUpdateOrInsert.exec())
            {
                QMessageBox::information(nullptr, "修改团支部对应成员信息-提示", "修改团支部对应成员信息失败:\n" + queryUpdateOrInsert.lastError().text());
                return;
            }
        }
    }
    else
    {
        QMessageBox::information(nullptr, "插入团支部对应成员信息-查询成员是否存在-提示", "查询成员是否存在失败:\n" + querySelect.lastError().text());
        return;
    }
    querySelect.clear();
    queryUpdateOrInsert.clear();
}
