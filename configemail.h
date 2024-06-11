#ifndef CONFIGEMAIL_H
#define CONFIGEMAIL_H

#include <QWidget>
#include <QDialog>
namespace Ui {
class ConfigEmail;
}

class ConfigEmail : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigEmail(QWidget *parent = nullptr);
    ~ConfigEmail();
protected:
    void init();
private slots:
    void on_btnReturnMainPage_clicked();
    void on_btnCompleteConfiguration_clicked();

signals:
    void returnMainPage();
private:
    Ui::ConfigEmail *ui;
};

#endif // CONFIGEMAIL_H
