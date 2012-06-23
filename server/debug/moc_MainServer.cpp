/****************************************************************************
** Meta object code from reading C++ file 'MainServer.h'
**
** Created: Sat 23. Jun 19:19:25 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MainServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      33,   11,   11,   11, 0x08,
      48,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainServer[] = {
    "MainServer\0\0onNewUserConnected()\0"
    "receivedData()\0onTimer()\0"
};

void MainServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainServer *_t = static_cast<MainServer *>(_o);
        switch (_id) {
        case 0: _t->onNewUserConnected(); break;
        case 1: _t->receivedData(); break;
        case 2: _t->onTimer(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MainServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MainServer,
      qt_meta_data_MainServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainServer))
        return static_cast<void*>(const_cast< MainServer*>(this));
    return QObject::qt_metacast(_clname);
}

int MainServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
