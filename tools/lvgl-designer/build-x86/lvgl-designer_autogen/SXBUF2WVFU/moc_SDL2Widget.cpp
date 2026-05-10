/****************************************************************************
** Meta object code from reading C++ file 'SDL2Widget.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/canvas/SDL2Widget.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SDL2Widget.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_designer__SDL2Widget_t {
    QByteArrayData data[14];
    char stringdata0[149];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_designer__SDL2Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_designer__SDL2Widget_t qt_meta_stringdata_designer__SDL2Widget = {
    {
QT_MOC_LITERAL(0, 0, 20), // "designer::SDL2Widget"
QT_MOC_LITERAL(1, 21, 9), // "lvglReady"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 13), // "widgetDropped"
QT_MOC_LITERAL(4, 46, 10), // "widgetType"
QT_MOC_LITERAL(5, 57, 1), // "x"
QT_MOC_LITERAL(6, 59, 1), // "y"
QT_MOC_LITERAL(7, 61, 13), // "wheelScrolled"
QT_MOC_LITERAL(8, 75, 5), // "delta"
QT_MOC_LITERAL(9, 81, 13), // "widgetClicked"
QT_MOC_LITERAL(10, 95, 21), // "Qt::KeyboardModifiers"
QT_MOC_LITERAL(11, 117, 9), // "modifiers"
QT_MOC_LITERAL(12, 127, 16), // "ctrlStateChanged"
QT_MOC_LITERAL(13, 144, 4) // "ctrl"

    },
    "designer::SDL2Widget\0lvglReady\0\0"
    "widgetDropped\0widgetType\0x\0y\0wheelScrolled\0"
    "delta\0widgetClicked\0Qt::KeyboardModifiers\0"
    "modifiers\0ctrlStateChanged\0ctrl"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_designer__SDL2Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    3,   40,    2, 0x06 /* Public */,
       7,    3,   47,    2, 0x06 /* Public */,
       9,    3,   54,    2, 0x06 /* Public */,
      12,    1,   61,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    4,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    8,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 10,    5,    6,   11,
    QMetaType::Void, QMetaType::Bool,   13,

       0        // eod
};

void designer::SDL2Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SDL2Widget *_t = static_cast<SDL2Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->lvglReady(); break;
        case 1: _t->widgetDropped((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->wheelScrolled((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->widgetClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< Qt::KeyboardModifiers(*)>(_a[3]))); break;
        case 4: _t->ctrlStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SDL2Widget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SDL2Widget::lvglReady)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SDL2Widget::*_t)(const QString & , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SDL2Widget::widgetDropped)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SDL2Widget::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SDL2Widget::wheelScrolled)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SDL2Widget::*_t)(int , int , Qt::KeyboardModifiers );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SDL2Widget::widgetClicked)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (SDL2Widget::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SDL2Widget::ctrlStateChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject designer::SDL2Widget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_designer__SDL2Widget.data,
      qt_meta_data_designer__SDL2Widget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *designer::SDL2Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *designer::SDL2Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_designer__SDL2Widget.stringdata0))
        return static_cast<void*>(const_cast< SDL2Widget*>(this));
    return QWidget::qt_metacast(_clname);
}

int designer::SDL2Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void designer::SDL2Widget::lvglReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void designer::SDL2Widget::widgetDropped(const QString & _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void designer::SDL2Widget::wheelScrolled(int _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void designer::SDL2Widget::widgetClicked(int _t1, int _t2, Qt::KeyboardModifiers _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void designer::SDL2Widget::ctrlStateChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
