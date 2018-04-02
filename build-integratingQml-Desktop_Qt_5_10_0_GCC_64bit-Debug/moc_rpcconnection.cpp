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
#include <QtCore/QList>
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
    QByteArrayData data[27];
    char stringdata0[403];
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
QT_MOC_LITERAL(10, 164, 11), // "socketError"
QT_MOC_LITERAL(11, 176, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(12, 205, 6), // "iError"
QT_MOC_LITERAL(13, 212, 9), // "sslErrors"
QT_MOC_LITERAL(14, 222, 16), // "QList<QSslError>"
QT_MOC_LITERAL(15, 239, 7), // "iErrors"
QT_MOC_LITERAL(16, 247, 4), // "open"
QT_MOC_LITERAL(17, 252, 4), // "iUrl"
QT_MOC_LITERAL(18, 257, 5), // "close"
QT_MOC_LITERAL(19, 263, 17), // "sendBinaryMessage"
QT_MOC_LITERAL(20, 281, 8), // "iMessage"
QT_MOC_LITERAL(21, 290, 15), // "sendTextMessage"
QT_MOC_LITERAL(22, 306, 19), // "setSslConfiguration"
QT_MOC_LITERAL(23, 326, 17), // "QSslConfiguration"
QT_MOC_LITERAL(24, 344, 14), // "iConfiguration"
QT_MOC_LITERAL(25, 359, 22), // "_binaryMessageReceived"
QT_MOC_LITERAL(26, 382, 20) // "_textMessageReceived"

    },
    "RPCConnection\0connected\0\0disconnected\0"
    "binaryMessageReceived\0textMessageReceived\0"
    "failedToSendBinaryMessage\0sentBinaryMessage\0"
    "failedToSendTextMessage\0sentTextMessage\0"
    "socketError\0QAbstractSocket::SocketError\0"
    "iError\0sslErrors\0QList<QSslError>\0"
    "iErrors\0open\0iUrl\0close\0sendBinaryMessage\0"
    "iMessage\0sendTextMessage\0setSslConfiguration\0"
    "QSslConfiguration\0iConfiguration\0"
    "_binaryMessageReceived\0_textMessageReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RPCConnection[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,
       3,    0,  100,    2, 0x06 /* Public */,
       4,    0,  101,    2, 0x06 /* Public */,
       5,    0,  102,    2, 0x06 /* Public */,
       6,    0,  103,    2, 0x06 /* Public */,
       7,    0,  104,    2, 0x06 /* Public */,
       8,    0,  105,    2, 0x06 /* Public */,
       9,    0,  106,    2, 0x06 /* Public */,
      10,    1,  107,    2, 0x06 /* Public */,
      13,    1,  110,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    1,  113,    2, 0x0a /* Public */,
      18,    0,  116,    2, 0x0a /* Public */,
      19,    1,  117,    2, 0x0a /* Public */,
      21,    1,  120,    2, 0x0a /* Public */,
      22,    1,  123,    2, 0x0a /* Public */,
      25,    1,  126,    2, 0x09 /* Protected */,
      26,    1,  129,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,   17,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   20,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, QMetaType::QByteArray,   20,
    QMetaType::Void, QMetaType::QString,   20,

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
        case 8: _t->socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: _t->sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 10: _t->open((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 11: _t->close(); break;
        case 12: _t->sendBinaryMessage((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 13: _t->sendTextMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->setSslConfiguration((*reinterpret_cast< const QSslConfiguration(*)>(_a[1]))); break;
        case 15: _t->_binaryMessageReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 16: _t->_textMessageReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSslConfiguration >(); break;
            }
            break;
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
        {
            typedef void (RPCConnection::*_t)(QAbstractSocket::SocketError );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::socketError)) {
                *result = 8;
                return;
            }
        }
        {
            typedef void (RPCConnection::*_t)(const QList<QSslError> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCConnection::sslErrors)) {
                *result = 9;
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
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
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

// SIGNAL 8
void RPCConnection::socketError(QAbstractSocket::SocketError _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void RPCConnection::sslErrors(const QList<QSslError> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
