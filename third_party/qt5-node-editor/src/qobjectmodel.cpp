#include "qobjectmodel.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>

#if QT_VERSION < 0x050700
//Q_FOREACH is deprecated and Qt CoW containers are detached on C++11 for loops
template<typename T>
const T& qAsConst(const T& v)
{
    return const_cast<const T&>(v);
}
#endif

struct InternalItem;

// The reason why this isn't part of InternalItem is to reduce the number of
// cache fault when displaying the model. It isn't possible to mix generic
// QMetaMethod based connections with lambdas. Obviously, doing it this way
// can potentially create many, many QObjects.
//
// One future optimization would be to have many slots per objects and keep
// an internal mapping back to the right InternalItem.
//
// Another one would be to re-implement the low level signal emit catcher
// and avoid all the QObject::connect.
class PropertyChangeReceiver : public QObject
{
    Q_OBJECT
public:
    explicit PropertyChangeReceiver(QObjectModel* m, InternalItem* i, QObject* p, int sigIdx);

    InternalItem* item;
    QObjectModel* model;

public Q_SLOTS:
    void changed();
    void slotDestroyed();
};


class PropertyChangeReceiverFactory
{
    static PropertyChangeReceiver* connectForId(InternalItem* item, int id);
private:
    static QHash<int, PropertyChangeReceiver*> m_hReceivers;
};

QHash<int, PropertyChangeReceiver*> PropertyChangeReceiverFactory::m_hReceivers;

struct MetaPropertyColumnMapper
{
    struct Property {
        const uchar      flags;
        const int        index;
        const int        metaType;
        const QByteArray name;
        int              sigIdx;
    };

    const QMetaObject* m_pMetaObject;
    QVector<Property*> m_lProperties {};

private:
    friend class QObjectModelPrivate;
    explicit MetaPropertyColumnMapper(const QMetaObject* m) : m_pMetaObject(m){}
};

struct InternalItem
{
    int           m_Index;
    QObject*      m_pObject;
    bool          m_IsObjectRoot;
    QObjectModel* m_pModel;
    MetaPropertyColumnMapper::Property *m_pProp;
    QVector<InternalItem*> m_lColumns;

    InternalItem* getChild(int id) {
        return id ? m_lColumns[id] : this;
    }
};

class QObjectModelPrivate final
{
public:
    bool m_IsHeterogeneous {false};
    bool m_IsVertical      {false};
    bool m_IsReadOnly      {false};
    int  m_DisplayRole     {Qt::DisplayRole};
    QVector<InternalItem*> m_lRows;
    static QHash<const QMetaObject*, MetaPropertyColumnMapper*> m_hMapper;

    // Helpers
    void clear();
    void regen();
    static MetaPropertyColumnMapper* getMapper(QObject*);
};

QHash<const QMetaObject*, MetaPropertyColumnMapper*> QObjectModelPrivate::m_hMapper;

QObjectModel::QObjectModel(QObject* parent) : QAbstractItemModel(parent),
d_ptr(new QObjectModelPrivate)
{
}

QObjectModel::QObjectModel(const QList<QObject*> objs, Qt::Orientation o, int dr, QObject* p) :
    QObjectModel(p)
{
    d_ptr->m_IsVertical = o == Qt::Vertical;
    d_ptr->m_DisplayRole = dr;
    addObjects(objs);
}


QObjectModel::~QObjectModel()
{
    d_ptr->clear();
    delete d_ptr;
}

QHash<int, QByteArray> QObjectModel::roleNames() const
{
    static QHash<int, QByteArray> ret;

    if (ret.isEmpty()) {
        ret[ ValueRole        ] = "valueRole";
        ret[ PropertyIdRole   ] = "propertyIdRole";
        ret[ PropertyNameRole ] = "propertyNameRole";
        ret[ CapabilitiesRole ] = "capabilitiesRole";
        ret[ MetaTypeRole     ] = "metaTypeRole";
        ret[ TypeNameRole     ] = "typeNameRole";
    }

    return ret;
}

QVariant QObjectModel::data(const QModelIndex& idx, int role) const
{
    if (!idx.isValid()) return {};

    const auto item = static_cast<InternalItem*>(idx.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
            return data(idx, d_ptr->m_DisplayRole);
        case Qt::EditRole:
            [[clang::fallthrough]];
        case Role::ValueRole:
            return item->m_pObject->property(item->m_pProp->name);
        case Role::PropertyIdRole:
            return item->m_pProp->index;
        case Role::CapabilitiesRole:
            return item->m_pProp->flags;
        case Role::MetaTypeRole:
            return item->m_pProp->metaType;
        case Role::TypeNameRole:
            return QStringLiteral("TODO");
        case Role::PropertyNameRole:
            return item->m_pProp->name;
    }

    return {};
}

bool QObjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;

    const auto item = static_cast<InternalItem*>(index.internalPointer());

    if (!value.canConvert(item->m_pProp->metaType))
        return false;

    item->m_pObject->setProperty(item->m_pProp->name, value);
    return true;
}

int QObjectModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : d_ptr->m_lRows.size();
}

int QObjectModel::columnCount(const QModelIndex& parent) const
{
    return (parent.isValid() || !d_ptr->m_lRows.size()) ?
        0 : d_ptr->m_lRows[0]->m_lColumns.size();
}

QModelIndex QObjectModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || column < 0 || row < 0)
        return {};

    if (row < d_ptr->m_lRows.size() && (
      column == 0 || column <= d_ptr->m_lRows[row]->m_lColumns.size()-1)
    )
        return createIndex(row, column, d_ptr->m_lRows[row]->getChild(column));

    return {};
}

Qt::ItemFlags QObjectModel::flags(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return Qt::NoItemFlags;

    const auto item = static_cast<InternalItem*>(idx.internalPointer());

    return Qt::ItemIsEnabled
        | Qt::ItemIsSelectable
        | Qt::ItemNeverHasChildren
        | (
            // Ok, that's disputable as a generic rule, but for the use
            // case where this model was developed, it is necessary
            item->m_pProp->flags & Capabilities::NOTIFY ?
            Qt::ItemIsDragEnabled : Qt::NoItemFlags
          )
        | (
            item->m_pProp->flags & Capabilities::WRITE ?
            (Qt::ItemIsEditable | Qt::ItemIsDropEnabled) : Qt::NoItemFlags
          );
}

QModelIndex QObjectModel::parent(const QModelIndex& idx) const
{
    // This model doesn't support trees yet
    return {};
}

QVariant QObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return {};
}

bool QObjectModel::heterogeneous() const
{
    return d_ptr->m_IsHeterogeneous; //TODO
}

void QObjectModel::setHeterogeneous(bool value)
{
    if (value == d_ptr->m_IsHeterogeneous) return;

    d_ptr->m_IsHeterogeneous = value; //TODO
    d_ptr->regen();
}

Qt::Orientation QObjectModel::oridentation() const
{
    return d_ptr->m_IsVertical ? Qt::Vertical : Qt::Horizontal;
}

void QObjectModel::setOrientation(Qt::Orientation o)
{
    d_ptr->m_IsVertical = o == Qt::Vertical;
}

int QObjectModel::displayRole() const
{
    return d_ptr->m_DisplayRole;
}

void QObjectModel::setDisplayRole(int role)
{
    d_ptr->m_DisplayRole = role;
    Q_EMIT dataChanged(index(0,0), index(rowCount()-1, columnCount() -1));
}

bool QObjectModel::isReadOnly() const
{
    return d_ptr->m_IsReadOnly;
}

void QObjectModel::setReadOnly(bool value)
{
    const bool changed = value != d_ptr->m_IsReadOnly;
    d_ptr->m_IsReadOnly = value;

    // Emit dataChanged so the ::flags() method is called by the view
    Q_EMIT dataChanged(index(0,0), index(rowCount()-1, columnCount() -1));
}

void QObjectModel::addObject(QObject* obj)
{
    if (!obj) return;

    auto mapper = d_ptr->getMapper(obj);

    if (mapper->m_lProperties.isEmpty()) return;

    beginInsertRows({}, d_ptr->m_lRows.size(), d_ptr->m_lRows.size());
    auto item = new InternalItem {
        d_ptr->m_lRows.size(),
        obj,
        true,
        this,
        mapper->m_lProperties[0],
        {}
    };
    endInsertRows();

    if (!d_ptr->m_IsVertical)
        d_ptr->m_lRows << item;

    auto list = d_ptr->m_IsVertical ? &d_ptr->m_lRows : &item->m_lColumns;

    beginInsertRows({}, list->size(), list->size()+mapper->m_lProperties.size()-2); //FIXME support columns
    for (auto p : qAsConst(mapper->m_lProperties)) {
        // Add the existing item to its own column to simplify the code elsewhere
        auto ip = ((p==item->m_pProp) ? item : new InternalItem {
            d_ptr->m_lRows.size(), //FIXME this doesn't support columns
            obj,
            false,
            this,
            p,
            {}
        });
        (*list) << ip;

        // The notify signal could be anything. It doesn't have to have an argument
        // with the same QMetaType as the property. Even if it has, there is no way
        // to know if it matches the property without trying to get it. So better
        // just assume the arguments is irrelevant and use the getter. Note that it
        // could be done using a custom qt_static_metacall, but again, it is a
        // little pointless to implement.
        if (p->sigIdx >= 0)
            auto r = new PropertyChangeReceiver(this, ip, obj, p->sigIdx);
    }
    endInsertRows();
}

void QObjectModel::addObjects(const QVector<QObject*>& objs)
{
    for (auto o : qAsConst(objs))
        addObject(o);
}

void QObjectModel::addObjects(const QList<QObject*>& objs)
{
    for (auto o : qAsConst(objs))
        addObject(o);
}

void QObjectModelPrivate::clear()
{
    QList<InternalItem*> items;

    for (auto i : qAsConst(m_lRows))
        if (i->m_IsObjectRoot)
            items << i;

    for (auto i : qAsConst(items)) {
        if (i->m_lColumns.size())
            for (auto ii : qAsConst(i->m_lColumns)) delete ii;
        delete i;
    }

    m_lRows.clear();
}

void QObjectModelPrivate::regen()
{
    // Gather all structures. This is very costly, but it should not be called
    // often (if ever) so lets save memory and keep only 1 index.
    QList<InternalItem*> rootItems;

    for (auto i : qAsConst(m_lRows))
        if (i->m_IsObjectRoot) {
            i->m_Index = m_lRows.size();
            rootItems << i;
        }

    m_lRows.clear();

    if (m_IsVertical) {
        for (auto i : qAsConst(rootItems))
            for (auto ii : qAsConst(i->m_lColumns)) {
                ii->m_Index = m_lRows.size();
                m_lRows << ii;
            }
    } else
        m_lRows = rootItems.toVector();
}

MetaPropertyColumnMapper* QObjectModelPrivate::getMapper(QObject* o)
{
    const QMetaObject* m = o->metaObject();
    auto mapper = m_hMapper[m];
    if (mapper) return mapper;

    mapper = new MetaPropertyColumnMapper(m);

    // Usually, I prefer enum classes, but Qt doesn't have much magic for
    // them yet...
    typedef QObjectModel::Capabilities CAP;

    const int pCount = m->propertyCount();

    for (int i=0; i < pCount; i++) {
        const auto p = m->property(i);

        if (!p.isUser()) //TODO remove this and add capability filters
            continue;

        mapper->m_lProperties << new MetaPropertyColumnMapper::Property {
            static_cast<unsigned char>(
                (p.isReadable     () ? CAP::READ   : CAP::NONE) |
                (p.isWritable     () ? CAP::WRITE  : CAP::NONE) |
                (p.hasNotifySignal() ? CAP::NOTIFY : CAP::NONE) |
                (p.isConstant     () ? CAP::CONST  : CAP::NONE) |
                (p.isUser         () ? CAP::USER   : CAP::NONE)
            ),
            p.propertyIndex(),
            p.userType(),
            p.name(),
            p.notifySignalIndex()
        };
    }

    return mapper;
}

// Setting the parent ensure the memory will be cleaned when `o` is destroyed.
PropertyChangeReceiver::PropertyChangeReceiver(QObjectModel* m, InternalItem* i, QObject* p, int sigIdx)
    : QObject(p), item(i), model(m)
{
    // Handle removing the item from this
    if (i->m_IsObjectRoot)
        connect(this, &QObject::destroyed,
            this, &PropertyChangeReceiver::slotDestroyed);

    connect(m, &QObject::destroyed, this, &QObject::deleteLater);

    const auto notifySignal = p->metaObject()->method(sigIdx);

    static int changedId = metaObject()->indexOfSlot("changed()");

    const auto notifySlot = metaObject()->method(changedId);

    QObject::connect(p, notifySignal, this, notifySlot);
}

void PropertyChangeReceiver::changed()
{
    const QModelIndex idx = model->createIndex(item->m_Index, 0, item); //FIXME this doesn't support columns

    Q_EMIT model->dataChanged(idx, idx);
}

void PropertyChangeReceiver::slotDestroyed()
{
    //TODO emit the beginRemoveRows and delete the InternalItem
}

#include "qobjectmodel.moc"
