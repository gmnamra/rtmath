/****************************************************************************
** Meta object code from reading C++ file 'frmmain.h'
**
** Created: Mon Mar 21 14:13:30 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\frmmain.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'frmmain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_frmMain[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x08,
      23,    8,    8,    8, 0x08,
      36,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_frmMain[] = {
    "frmMain\0\0model_props()\0show_about()\0"
    "atmos_open()\0"
};

const QMetaObject frmMain::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_frmMain,
      qt_meta_data_frmMain, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &frmMain::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *frmMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *frmMain::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_frmMain))
        return static_cast<void*>(const_cast< frmMain*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int frmMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: model_props(); break;
        case 1: show_about(); break;
        case 2: atmos_open(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE