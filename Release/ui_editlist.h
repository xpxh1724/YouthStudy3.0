/********************************************************************************
** Form generated from reading UI file 'editlist.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITLIST_H
#define UI_EDITLIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditList
{
public:
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnReturn;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *EditList)
    {
        if (EditList->objectName().isEmpty())
            EditList->setObjectName(QString::fromUtf8("EditList"));
        EditList->resize(500, 670);
        EditList->setMinimumSize(QSize(500, 670));
        EditList->setMaximumSize(QSize(500, 670));
        EditList->setStyleSheet(QString::fromUtf8("*{\n"
"font: 12pt \"\346\245\267\344\275\223\";\n"
"}\n"
"#EditList\n"
"{\n"
"background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:1 rgb(226,238,243),\n"
"                                stop:0 rgb(240,207,214));\n"
"}\n"
"QPushButton\n"
"{\n"
"font: 12pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border-radius:8px;\n"
"border:2px solid gray;\n"
"background-color: rgba(202, 221, 245,0.7);\n"
"height:35;\n"
"}\n"
"QPushButton:hover\n"
"{\n"
"font: 13pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;\n"
"border:2px solid;\n"
"background-color: rgb(202, 221, 245);\n"
"}"));
        verticalLayout_2 = new QVBoxLayout(EditList);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(EditList);
        label->setObjectName(QString::fromUtf8("label"));
        label->setStyleSheet(QString::fromUtf8("font: 24pt \"\346\245\267\344\275\223\";\n"
"font-weight:bold;"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label);

        groupBox = new QGroupBox(EditList);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox{\n"
"border:2px solid gray;\n"
"border-radius:10px;\n"
"margin-top:11px;\n"
"}\n"
"QGroupBox::title {\n"
"      subcontrol-origin: margin;\n"
"      subcontrol-position: top;\n"
"}"));
        groupBox->setAlignment(Qt::AlignCenter);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textEdit = new QTextEdit(groupBox);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout->addWidget(textEdit);


        verticalLayout_2->addWidget(groupBox);

        widget = new QWidget(EditList);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(56, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(widget);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));
        btnSave->setMinimumSize(QSize(150, 50));

        horizontalLayout->addWidget(btnSave);

        horizontalSpacer_2 = new QSpacerItem(57, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btnReturn = new QPushButton(widget);
        btnReturn->setObjectName(QString::fromUtf8("btnReturn"));
        btnReturn->setMinimumSize(QSize(150, 50));

        horizontalLayout->addWidget(btnReturn);

        horizontalSpacer_3 = new QSpacerItem(56, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout_2->addWidget(widget);


        retranslateUi(EditList);

        QMetaObject::connectSlotsByName(EditList);
    } // setupUi

    void retranslateUi(QWidget *EditList)
    {
        EditList->setWindowTitle(QApplication::translate("EditList", "Form", nullptr));
        label->setText(QApplication::translate("EditList", "\347\274\226\350\276\221\345\220\215\345\215\225", nullptr));
        groupBox->setTitle(QApplication::translate("EditList", "\351\234\200\345\256\214\346\210\220\344\272\272\345\221\230\345\220\215\345\215\225", nullptr));
        btnSave->setText(QApplication::translate("EditList", "\344\277\235\345\255\230", nullptr));
#ifndef QT_NO_SHORTCUT
        btnSave->setShortcut(QApplication::translate("EditList", "Return", nullptr));
#endif // QT_NO_SHORTCUT
        btnReturn->setText(QApplication::translate("EditList", "\350\277\224\345\233\236", nullptr));
#ifndef QT_NO_SHORTCUT
        btnReturn->setShortcut(QApplication::translate("EditList", "Esc", nullptr));
#endif // QT_NO_SHORTCUT
    } // retranslateUi

};

namespace Ui {
    class EditList: public Ui_EditList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITLIST_H
