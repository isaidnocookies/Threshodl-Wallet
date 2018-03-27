/****************************************************************************
** Meta object code from reading C++ file 'rpcconnection.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../RPC/rpcconnection.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rpcconnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RPCConnection_t {
    QByteArrayData data[18];
    char stringdata0[267];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RPCConnection_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RPCConnection_t qt_meta_stringdata_RPCConnection = {
    {
QT_MOC_LITERAL(0, 0, 13), // "RPCConnection"
QT_MOC_LITERAL(1, 14, 9), // "connected"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 12), // "disconnected"
QT_MOC_LITERAL(4, 38, 21), // "binaryMessageReceived"
QT_MOC_LITERAL(5, 60, 19), // "textMessageReceived"
QT_MOC_LITERAL(6, 80, 25), // "failedToSendBinaryMessage"
QT_MOC_LITERAL(7, 106, 17), // "sentBinaryMessage"
QT_MOC_LITERAL(8, 124, 23), // "failedToSendTextMessage"
QT_MOC_LITERAL(9, 148, 15), // "sentTextMessage"
QT_MOC_LITERAL(10, 164, 4), // "open"
QT_MOC_LITERAL(11, 169, 4), // "iUrl"
QT_MOC_LITERAL(12, 174, 5), // "close"
QT_MOC_LITERAL(13, 180, 17), // "sendBinaryMessage"
QT_MOC_LITERAL(14, 198, 8), // "iMessage"
QT_MOC_LITERAL(15, 207, 15), // "sendTextMessage"
QT_MOC_LITERAL(16, 223, 22), // "_binaryMessageReceived"
QT_MOC_LITERAL(17, 246, 20) // "_textMessageReceived"

    },
    "RPCConnection\0connected\0\0disconnected\0"
    "binaryMessageReceived\0textMessageReceived\0"
    "failedToSendBinaryMessage\0sentBinaryMessage\0"
    "failedToSendTextMessage\0sentTextMessage\0"
    "open\0iUrl\0close\0sendBinaryMessage\0"
    "iMessage\0sendTextMessage\0"
    "_binaryMessageReceived\0_textMessageReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RPCConnection[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    0,   85,    2, 0x06 /* Public */,
       4,    0,   86,    2, 0x06 /* Public */,
       5,    0,   87,    2, 0x06 /* Public */,
       6,    0,   88,    2, 0x06 /* Public */,
       7,    0,   89,    2, 0x06 /* Public */,
       8,    0,   90,    2, 0x06 /* Public */,
       9,    0,   91,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   92,    2, 0x0a /* Public */,
      12,    0,   95,    2, 0x0a /* Public */,
      13,    1,   96,    2, 0x0a /* Public */,
      15,    1,   99,    2, 0x0a /* Public */,
      16,    1,  102,    2, 0x09 /* Protected */,
      17,    1,  105,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QByteArray,   14,
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

void RPCConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RPCConnection *_t = static_cast<RPCConnection *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->binaryMessageReceived(); break;
        case 3: _t->textMessageReceived(); break;
        case 4: _t->failedToSendBinaryMessage(); break;
        case 5: _t->sentBinaryMessage(); break;
        case 6: _t->failedToSendTextMessage(); break;
        case 7: _t->sentTextMessage(); break;
        case 8: _t->open((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 9: _t->close(); break;
        case 10: _t->sendBinaryMessage((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 11: _t->sendTextMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->_binaryMessageReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 13: _t->_textMessageReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::connected)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::binaryMessageReceived)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::textMessageReceived)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::failedToSendBinaryMessage)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::sentBinaryMessage)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::failedToSendTextMessage)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::sentTextMessage)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject RPCConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RPCConnection.data,
      qt_meta_data_RPCConnection,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RPCConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RPCConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RPCConnection.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RPCConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void RPCConnection::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RPCConnection::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RPCConnection::binaryMessageReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RPCConnection::textMessageReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RPCConnection::failedToSendBinaryMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RPCConnection::sentBinaryMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void RPCConnection::failedToSendTextMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void RPCConnection::sentTextMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
