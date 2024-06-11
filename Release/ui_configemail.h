/********************************************************************************
** Form generated from reading UI file 'configemail.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGEMAIL_H
#define UI_CONFIGEMAIL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigEmail
{
public:
    QLineEdit *emailLine;
    QLabel *title;
    QLabel *emailLab;
    QLineEdit *secretKeyLine;
    QLabel *secretKeyLab;
    QPushButton *btnReturnMainPage;
    QPushButton *btnCompleteConfiguration;

    void setupUi(QWidget *ConfigEmail)
    {
        if (ConfigEmail->objectName().isEmpty())
            ConfigEmail->setObjectName(QString::fromUtf8("ConfigEmail"));
        ConfigEmail->resize(700, 650);
        ConfigEmail->setMinimumSize(QSize(700, 650));
        ConfigEmail->setMaximumSize(QSize(700, 650));
        ConfigEmail->setStyleSheet(QString::fromUtf8("#title\n"
"{\n"
"font: 24pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"background-color: rgba(202, 221, 245,0.5);\n"
"background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:1 rgb(226,238,243),\n"
"                                stop:0 rgb(240,207,214));\n"
"border-radius:20px;\n"
"}\n"
"#ConfigEmail\n"
"{\n"
"background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 rgb(226,238,243),\n"
"                                stop:1 rgb(240,207,214));\n"
"}\n"
"#emailLab\n"
"{\n"
"font: 18pt \"\346\226\271\346\255\243\347\262\227\351\273\221\345\256\213\347\256\200\344\275\223\";\n"
"color: rgb(3, 57, 103);\n"
"}\n"
"#secretKeyLab\n"
"{\n"
"font: 18pt \"\346\226\271\346\255\243\347\262\227\351\273\221\345\256\213\347\256\200\344\275\223\";\n"
"color: rgb(3, 57, 103);\n"
"}\n"
"QLineEdit\n"
"{\n"
"font: 14pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"background-color: rgba(136, 193, 150,0.3);\n"
"border-radius:10px;\n"
""
                        "border:2px solid gray;\n"
"}\n"
"QLineEdit:hover\n"
"{\n"
"font: 14pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border:2px solid ;\n"
"}\n"
"#btnCompleteConfiguration\n"
"{\n"
"font: 20pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border-radius:10px;\n"
"border:2px solid gray;\n"
"background-color: rgba(202, 221, 245,0.7);\n"
"}\n"
"#btnCompleteConfiguration:hover\n"
"{\n"
"font: 22pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border:3px solid;\n"
"background-color: rgb(202, 221, 245);\n"
"}\n"
"#btnReturnMainPage\n"
"{\n"
"font: 20pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border-radius:10px;\n"
"border:2px solid gray;\n"
"background-color: rgba(202, 221, 245,0.7);\n"
"}\n"
"#btnReturnMainPage:hover\n"
"{\n"
"font: 22pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border:3px solid;\n"
"background-color: rgb(202, 221, 245);\n"
"}"));
        emailLine = new QLineEdit(ConfigEmail);
        emailLine->setObjectName(QString::fromUtf8("emailLine"));
        emailLine->setGeometry(QRect(100, 270, 490, 62));
        title = new QLabel(ConfigEmail);
        title->setObjectName(QString::fromUtf8("title"));
        title->setGeometry(QRect(100, 50, 500, 120));
        title->setMinimumSize(QSize(500, 120));
        title->setMaximumSize(QSize(500, 120));
        title->setStyleSheet(QString::fromUtf8(""));
        title->setAlignment(Qt::AlignCenter);
        emailLab = new QLabel(ConfigEmail);
        emailLab->setObjectName(QString::fromUtf8("emailLab"));
        emailLab->setGeometry(QRect(100, 210, 250, 60));
        emailLab->setMinimumSize(QSize(250, 60));
        emailLab->setMaximumSize(QSize(250, 60));
        emailLab->setStyleSheet(QString::fromUtf8(""));
        secretKeyLine = new QLineEdit(ConfigEmail);
        secretKeyLine->setObjectName(QString::fromUtf8("secretKeyLine"));
        secretKeyLine->setGeometry(QRect(100, 410, 490, 62));
        secretKeyLine->setEchoMode(QLineEdit::Normal);
        secretKeyLab = new QLabel(ConfigEmail);
        secretKeyLab->setObjectName(QString::fromUtf8("secretKeyLab"));
        secretKeyLab->setGeometry(QRect(100, 350, 250, 60));
        secretKeyLab->setMinimumSize(QSize(250, 60));
        secretKeyLab->setMaximumSize(QSize(250, 60));
        secretKeyLab->setStyleSheet(QString::fromUtf8(""));
        btnReturnMainPage = new QPushButton(ConfigEmail);
        btnReturnMainPage->setObjectName(QString::fromUtf8("btnReturnMainPage"));
        btnReturnMainPage->setGeometry(QRect(40, 540, 250, 61));
        btnReturnMainPage->setCheckable(false);
        btnReturnMainPage->setAutoDefault(false);
        btnCompleteConfiguration = new QPushButton(ConfigEmail);
        btnCompleteConfiguration->setObjectName(QString::fromUtf8("btnCompleteConfiguration"));
        btnCompleteConfiguration->setGeometry(QRect(410, 540, 250, 61));

        retranslateUi(ConfigEmail);

        btnReturnMainPage->setDefault(false);


        QMetaObject::connectSlotsByName(ConfigEmail);
    } // setupUi

    void retranslateUi(QWidget *ConfigEmail)
    {
        ConfigEmail->setWindowTitle(QApplication::translate("ConfigEmail", "Form", nullptr));
        emailLine->setText(QString());
        title->setText(QApplication::translate("ConfigEmail", "\351\205\215\347\275\256\351\202\256\347\256\261", nullptr));
        emailLab->setText(QApplication::translate("ConfigEmail", "Email", nullptr));
        secretKeyLine->setText(QString());
        secretKeyLab->setText(QApplication::translate("ConfigEmail", "SecretKey", nullptr));
        btnReturnMainPage->setText(QApplication::translate("ConfigEmail", "\350\277\224\345\233\236\344\270\273\351\241\265", nullptr));
        btnCompleteConfiguration->setText(QApplication::translate("ConfigEmail", "\345\256\214\346\210\220\351\205\215\347\275\256", nullptr));
#ifndef QT_NO_SHORTCUT
        btnCompleteConfiguration->setShortcut(QApplication::translate("ConfigEmail", "Return", nullptr));
#endif // QT_NO_SHORTCUT
    } // retranslateUi

};

namespace Ui {
    class ConfigEmail: public Ui_ConfigEmail {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGEMAIL_H
