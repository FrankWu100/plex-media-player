/****************************************************************************
** Meta object code from reading C++ file 'qhttpclient.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../external/qhttp/src/qhttpclient.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhttpclient.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_qhttp__client__QHttpClient_t {
    QByteArrayData data[11];
    char stringdata0[130];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_qhttp__client__QHttpClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_qhttp__client__QHttpClient_t qt_meta_stringdata_qhttp__client__QHttpClient = {
    {
QT_MOC_LITERAL(0, 0, 26), // "qhttp::client::QHttpClient"
QT_MOC_LITERAL(1, 27, 13), // "httpConnected"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 13), // "QHttpRequest*"
QT_MOC_LITERAL(4, 56, 3), // "req"
QT_MOC_LITERAL(5, 60, 11), // "newResponse"
QT_MOC_LITERAL(6, 72, 14), // "QHttpResponse*"
QT_MOC_LITERAL(7, 87, 3), // "res"
QT_MOC_LITERAL(8, 91, 12), // "disconnected"
QT_MOC_LITERAL(9, 104, 17), // "connectingTimeOut"
QT_MOC_LITERAL(10, 122, 7) // "timeOut"

    },
    "qhttp::client::QHttpClient\0httpConnected\0"
    "\0QHttpRequest*\0req\0newResponse\0"
    "QHttpResponse*\0res\0disconnected\0"
    "connectingTimeOut\0timeOut"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_qhttp__client__QHttpClient[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       1,   42, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,
       8,    0,   40,    2, 0x06 /* Public */,
       9,    0,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      10, QMetaType::UInt, 0x00095103,

       0        // eod
};

void qhttp::client::QHttpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QHttpClient *_t = static_cast<QHttpClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->httpConnected((*reinterpret_cast< QHttpRequest*(*)>(_a[1]))); break;
        case 1: _t->newResponse((*reinterpret_cast< QHttpResponse*(*)>(_a[1]))); break;
        case 2: _t->disconnected(); break;
        case 3: _t->connectingTimeOut(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (QHttpClient::*_t)(QHttpRequest * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttpClient::httpConnected)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QHttpClient::*_t)(QHttpResponse * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttpClient::newResponse)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QHttpClient::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttpClient::disconnected)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QHttpClient::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QHttpClient::connectingTimeOut)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        QHttpClient *_t = static_cast<QHttpClient *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< quint32*>(_v) = _t->timeOut(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        QHttpClient *_t = static_cast<QHttpClient *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTimeOut(*reinterpret_cast< quint32*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject qhttp::client::QHttpClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qhttp__client__QHttpClient.data,
      qt_meta_data_qhttp__client__QHttpClient,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *qhttp::client::QHttpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *qhttp::client::QHttpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_qhttp__client__QHttpClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int qhttp::client::QHttpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void qhttp::client::QHttpClient::httpConnected(QHttpRequest * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qhttp::client::QHttpClient::newResponse(QHttpResponse * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void qhttp::client::QHttpClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void qhttp::client::QHttpClient::connectingTimeOut()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE