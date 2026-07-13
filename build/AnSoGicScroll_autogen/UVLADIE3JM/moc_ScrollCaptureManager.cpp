/****************************************************************************
** Meta object code from reading C++ file 'ScrollCaptureManager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ScrollCaptureManager.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScrollCaptureManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_ScrollCaptureManager_t {
    uint offsetsAndSizes[18];
    char stringdata0[21];
    char stringdata1[6];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[5];
    char stringdata5[21];
    char stringdata6[23];
    char stringdata7[18];
    char stringdata8[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ScrollCaptureManager_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ScrollCaptureManager_t qt_meta_stringdata_ScrollCaptureManager = {
    {
        QT_MOC_LITERAL(0, 20),  // "ScrollCaptureManager"
        QT_MOC_LITERAL(21, 5),  // "start"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 16),  // "onRegionSelected"
        QT_MOC_LITERAL(45, 4),  // "rect"
        QT_MOC_LITERAL(50, 20),  // "onSelectionCancelled"
        QT_MOC_LITERAL(71, 22),  // "onCaptureNextRequested"
        QT_MOC_LITERAL(94, 17),  // "onFinishRequested"
        QT_MOC_LITERAL(112, 17)   // "onCancelRequested"
    },
    "ScrollCaptureManager",
    "start",
    "",
    "onRegionSelected",
    "rect",
    "onSelectionCancelled",
    "onCaptureNextRequested",
    "onFinishRequested",
    "onCancelRequested"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ScrollCaptureManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x0a,    1 /* Public */,
       3,    1,   51,    2, 0x08,    2 /* Private */,
       5,    0,   54,    2, 0x08,    4 /* Private */,
       6,    0,   55,    2, 0x08,    5 /* Private */,
       7,    0,   56,    2, 0x08,    6 /* Private */,
       8,    0,   57,    2, 0x08,    7 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ScrollCaptureManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ScrollCaptureManager.offsetsAndSizes,
    qt_meta_data_ScrollCaptureManager,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ScrollCaptureManager_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ScrollCaptureManager, std::true_type>,
        // method 'start'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRegionSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QRect &, std::false_type>,
        // method 'onSelectionCancelled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCaptureNextRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFinishRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCancelRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ScrollCaptureManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScrollCaptureManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->start(); break;
        case 1: _t->onRegionSelected((*reinterpret_cast< std::add_pointer_t<QRect>>(_a[1]))); break;
        case 2: _t->onSelectionCancelled(); break;
        case 3: _t->onCaptureNextRequested(); break;
        case 4: _t->onFinishRequested(); break;
        case 5: _t->onCancelRequested(); break;
        default: ;
        }
    }
}

const QMetaObject *ScrollCaptureManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScrollCaptureManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ScrollCaptureManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScrollCaptureManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
