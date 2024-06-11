/****************************************************************************
** Meta object code from reading C++ file 'login.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../login.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'login.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Login_t {
    QByteArrayData data[12];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Login_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Login_t qt_meta_stringdata_Login = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Login"
QT_MOC_LITERAL(1, 6, 12), // "loginSuccess"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4), // "user"
QT_MOC_LITERAL(4, 25, 3), // "pwd"
QT_MOC_LITERAL(5, 29, 12), // "loginTeacher"
QT_MOC_LITERAL(6, 42, 5), // "users"
QT_MOC_LITERAL(7, 48, 4), // "team"
QT_MOC_LITERAL(8, 53, 17), // "loginCollegeAdmin"
QT_MOC_LITERAL(9, 71, 14), // "inputOtherUser"
QT_MOC_LITERAL(10, 86, 19), // "on_btnLogin_clicked"
QT_MOC_LITERAL(11, 106, 19) // "on_btnInput_clicked"

    },
    "Login\0loginSuccess\0\0user\0pwd\0loginTeacher\0"
    "users\0team\0loginCollegeAdmin\0"
    "inputOtherUser\0on_btnLogin_clicked\0"
    "on_btnInput_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Login[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       5,    2,   49,    2, 0x06 /* Public */,
       8,    2,   54,    2, 0x06 /* Public */,
       9,    0,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   60,    2, 0x08 /* Private */,
      11,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    6,    7,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QString,    6,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Login::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Login *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loginSuccess((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->loginTeacher((*reinterpret_cast< QStringList(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->loginCollegeAdmin((*reinterpret_cast< QStringList(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->inputOtherUser(); break;
        case 4: _t->on_btnLogin_clicked(); break;
        case 5: _t->on_btnInput_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Login::*)(QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Login::loginSuccess)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Login::*)(QStringList , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Login::loginTeacher)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Login::*)(QStringList , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Login::loginCollegeAdmin)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Login::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Login::inputOtherUser)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Login::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Login.data,
    qt_meta_data_Login,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Login::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Login::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Login.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Login::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Login::loginSuccess(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Login::loginTeacher(QStringList _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Login::loginCollegeAdmin(QStringList _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Login::inputOtherUser()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
