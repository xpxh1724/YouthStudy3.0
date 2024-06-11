/****************************************************************************
** Meta object code from reading C++ file 'psmtp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../psmtp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'psmtp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PSmtp_t {
    QByteArrayData data[20];
    char stringdata0[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PSmtp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PSmtp_t qt_meta_stringdata_PSmtp = {
    {
QT_MOC_LITERAL(0, 0, 5), // "PSmtp"
QT_MOC_LITERAL(1, 6, 13), // "testUserState"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 2), // "ok"
QT_MOC_LITERAL(4, 24, 12), // "disconnected"
QT_MOC_LITERAL(5, 37, 11), // "emailStatus"
QT_MOC_LITERAL(6, 49, 6), // "status"
QT_MOC_LITERAL(7, 56, 8), // "progress"
QT_MOC_LITERAL(8, 65, 1), // "p"
QT_MOC_LITERAL(9, 67, 15), // "connectToServer"
QT_MOC_LITERAL(10, 83, 20), // "disconnectFromServer"
QT_MOC_LITERAL(11, 104, 10), // "getMessage"
QT_MOC_LITERAL(12, 115, 15), // "emailStatusSlot"
QT_MOC_LITERAL(13, 131, 15), // "progressChanged"
QT_MOC_LITERAL(14, 147, 13), // "sendEmailSlot"
QT_MOC_LITERAL(15, 161, 8), // "username"
QT_MOC_LITERAL(16, 170, 8), // "password"
QT_MOC_LITERAL(17, 179, 2), // "to"
QT_MOC_LITERAL(18, 182, 5), // "title"
QT_MOC_LITERAL(19, 188, 4) // "text"

    },
    "PSmtp\0testUserState\0\0ok\0disconnected\0"
    "emailStatus\0status\0progress\0p\0"
    "connectToServer\0disconnectFromServer\0"
    "getMessage\0emailStatusSlot\0progressChanged\0"
    "sendEmailSlot\0username\0password\0to\0"
    "title\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PSmtp[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    0,   67,    2, 0x06 /* Public */,
       5,    1,   68,    2, 0x06 /* Public */,
       7,    1,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   74,    2, 0x0a /* Public */,
      10,    0,   75,    2, 0x0a /* Public */,
      11,    0,   76,    2, 0x0a /* Public */,
      12,    1,   77,    2, 0x0a /* Public */,
      13,    1,   80,    2, 0x0a /* Public */,
      14,    5,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Double,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   15,   16,   17,   18,   19,

       0        // eod
};

void PSmtp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PSmtp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->testUserState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->disconnected(); break;
        case 2: _t->emailStatus((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->progress((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->connectToServer(); break;
        case 5: _t->disconnectFromServer(); break;
        case 6: _t->getMessage(); break;
        case 7: _t->emailStatusSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->progressChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->sendEmailSlot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PSmtp::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PSmtp::testUserState)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PSmtp::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PSmtp::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PSmtp::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PSmtp::emailStatus)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PSmtp::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PSmtp::progress)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PSmtp::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_PSmtp.data,
    qt_meta_data_PSmtp,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PSmtp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PSmtp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PSmtp.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PSmtp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void PSmtp::testUserState(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PSmtp::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PSmtp::emailStatus(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PSmtp::progress(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
