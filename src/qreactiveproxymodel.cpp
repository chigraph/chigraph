#include "qreactiveproxymodel.h"

#include <QtCore/QAbstractTableModel>
#include <QtCore/QAbstractProxyModel>
#include <QtCore/QMimeData>
#include <QtCore/QDebug>

#include "qmodeldatalistdecoder.h"

#if QT_VERSION < 0x050700
//Q_FOREACH is deprecated and Qt CoW containers are detached on C++11 for loops
template<typename T>
const T& qAsConst(const T& v)
{
    return const_cast<const T&>(v);
}
#endif

class ConnectedIndicesModel : public QAbstractTableModel
{
    friend class QReactiveProxyModel;
    friend class QReactiveProxyModelPrivate;
public:
    ConnectedIndicesModel(QObject* parent, QReactiveProxyModelPrivate* d);

    virtual QVariant data(const QModelIndex& idx, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual int rowCount(const QModelIndex& parent = {}) const override;
    virtual int columnCount(const QModelIndex& parent = {}) const override;

private:
    QReactiveProxyModelPrivate* d_ptr;
};

struct ConnectionHolder
{
    int index;

    QPersistentModelIndex source;
    QPersistentModelIndex destination;
    void* sourceIP;
    void* destinationIP;

    bool isValid() const {
        return source.isValid() && destination.isValid();
    }

    bool isUsed() const {
        return source.isValid() || destination.isValid();
    }
};

class QReactiveProxyModelPrivate : public QObject
{
public:
    const QString MIME_TYPE = QStringLiteral("qt-model/reactive-connection");
    QVector<int> m_lConnectedRoles;
    ConnectedIndicesModel* m_pConnectionModel;
    QHash<const QMimeData*, QPersistentModelIndex> m_hDraggedIndexCache;
    QVector<ConnectionHolder*> m_lConnections;

    QAbstractProxyModel* m_pCurrentProxy {nullptr};

    bool m_HasExtraRole[5] {false, false, false, false, false};
    int  m_ExtraRole   [5] {0,     0,     0,     0,     0    };

    // In case dataChanged() contains a single QModelIndex, use this fast path
    // to avoid doing a query on each connections or QModelIndex
    QHash<void*, ConnectionHolder*> m_hDirectMapping;

    //Helper
    void clear();
    bool synchronize(const QModelIndex& source, const QModelIndex& destination) const;
    ConnectionHolder* newConnection();

    void notifyConnect(const QModelIndex& source, const QModelIndex& destination) const;
    void notifyDisconnect(const QModelIndex& source, const QModelIndex& destination) const;
    void notifyConnect(const ConnectionHolder* conn) const;
    void notifyDisconnect(const ConnectionHolder* conn) const;

    QReactiveProxyModel* q_ptr;
public Q_SLOTS:
    void slotMimeDestroyed();
    void slotDataChanged(const QModelIndex& tl, const QModelIndex& br);
};

QReactiveProxyModel::QReactiveProxyModel(QObject* parent) : QIdentityProxyModel(parent),
    d_ptr(new QReactiveProxyModelPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->m_pConnectionModel = new ConnectedIndicesModel(this, d_ptr);

    connect(this, &QAbstractItemModel::dataChanged,
        d_ptr, &QReactiveProxyModelPrivate::slotDataChanged);
}

ConnectedIndicesModel::ConnectedIndicesModel(QObject* parent, QReactiveProxyModelPrivate* d)
    : QAbstractTableModel(parent), d_ptr(d)
{
    
}

QReactiveProxyModel::~QReactiveProxyModel()
{
    delete d_ptr;
}

QMimeData* QReactiveProxyModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.size() != 1)
        return Q_NULLPTR;

    const auto idx = indexes.first();

    if (!idx.isValid())
        return Q_NULLPTR;

    // Will also have whatever the source had and hopefully the
    // `application/x-qabstractitemmodeldatalist` MIME type from
    // QAbstractItemModel::mimeData()`
    auto md = QIdentityProxyModel::mimeData(indexes);

    connect(md, &QObject::destroyed, d_ptr, &QReactiveProxyModelPrivate::slotMimeDestroyed);

    d_ptr->m_hDraggedIndexCache[md] = idx;

    md->setData(d_ptr->MIME_TYPE, "valid");

    return md;
}

bool QReactiveProxyModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    const auto idx = index(row, column, parent);

    if ((!data) || (!idx.isValid()) || !(action & supportedDropActions()))
        return QIdentityProxyModel::canDropMimeData(data, action, row, column, parent);

    if (!data->hasFormat(d_ptr->MIME_TYPE))
        return QIdentityProxyModel::canDropMimeData(data, action, row, column, parent);

    // Try to decode `application/x-qabstractitemmodeldatalist`
    QModelDataListDecoder decoder(data);

    return decoder.canConvert(idx.data(Qt::EditRole).userType());
}

bool QReactiveProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action)

    if ((!data) || !data->hasFormat(d_ptr->MIME_TYPE))
        return QIdentityProxyModel::dropMimeData(data, action, row, column, parent);

    const auto srcIdx  = d_ptr->m_hDraggedIndexCache[data];
    const auto destIdx = index(row, column, parent);

    if ((!srcIdx.isValid()) || !destIdx.isValid())
        return QIdentityProxyModel::dropMimeData(data, action, row, column, parent);

    connectIndices(srcIdx, destIdx);

    return true;
}

QStringList QReactiveProxyModel::mimeTypes() const
{
    static QStringList ret {d_ptr->MIME_TYPE};

    return ret;
}

Qt::DropActions QReactiveProxyModel::supportedDropActions() const
{
    return QIdentityProxyModel::supportedDropActions() | Qt::LinkAction;
}

Qt::DropActions QReactiveProxyModel::supportedDragActions() const
{
    return QIdentityProxyModel::supportedDropActions() | Qt::LinkAction;
}

void QReactiveProxyModel::setSourceModel(QAbstractItemModel *sm)
{
    if (sm == sourceModel())
        return;

    d_ptr->clear();

    QIdentityProxyModel::setSourceModel(sm);

}

Qt::ItemFlags QReactiveProxyModel::flags(const QModelIndex &idx) const
{
    return mapToSource(idx).flags();
}

void QReactiveProxyModel::addConnectedRole(int role)
{
    if (d_ptr->m_lConnectedRoles.indexOf(role) == -1)
        d_ptr->m_lConnectedRoles << role;
}

QVector<int> QReactiveProxyModel::connectedRoles() const
{
    return d_ptr->m_lConnectedRoles;
}

QAbstractProxyModel* QReactiveProxyModel::currentProxy() const
{
    return d_ptr->m_pCurrentProxy;
}

/**
 *  Allow to set a proxy the connection model will listen to in order to be
 * notified of item changes.
 *
 * This is useful to avoid having to expose the internal mapping. However, it
 * isn't the prettyest workaround ever.
 */
void QReactiveProxyModel::setCurrentProxy(QAbstractProxyModel* proxy)
{
    auto cur = d_ptr->m_pCurrentProxy ? d_ptr->m_pCurrentProxy : this;

    disconnect(cur, &QAbstractItemModel::dataChanged,
        d_ptr, &QReactiveProxyModelPrivate::slotDataChanged);

    d_ptr->m_pCurrentProxy = proxy;

    cur = d_ptr->m_pCurrentProxy ? d_ptr->m_pCurrentProxy : this;

    connect(cur, &QAbstractItemModel::dataChanged,
        d_ptr, &QReactiveProxyModelPrivate::slotDataChanged);
}

QAbstractItemModel* QReactiveProxyModel::connectionsModel() const
{
    return d_ptr->m_pConnectionModel;
}

int QReactiveProxyModel::extraRole(ExtraRoles type) const
{
    return d_ptr->m_ExtraRole[static_cast<int>(type)];
}

void QReactiveProxyModel::setExtraRole(ExtraRoles type, int role)
{
    d_ptr->m_HasExtraRole[static_cast<int>(type)] = true;
    d_ptr->m_ExtraRole   [static_cast<int>(type)] = role;
}

ConnectionHolder* QReactiveProxyModelPrivate::newConnection()
{
    if (m_lConnections.isEmpty() || m_lConnections.last()->isUsed()) {
        const int id = m_lConnections.size();

        auto conn = new ConnectionHolder { id, {}, {}, {}, {}, };

        // Register the connection
        //m_pConnectionModel->beginInsertRows({}, id, id); //FIXME conflict with rowCount
        m_lConnections << conn;
        //m_pConnectionModel->endInsertRows();
    }

    return m_lConnections.last();
}

bool QReactiveProxyModel::connectIndices(const QModelIndex& srcIdx, const QModelIndex& destIdx)
{
    if (!(srcIdx.isValid() && destIdx.isValid()))
        return false;

    if (srcIdx.model() != this || destIdx.model() != this) {
        qWarning() << "Trying to connect QModelIndex from the wrong model";
        return false;
    }

    //TODO check if its already connected
    //TODO check if there is a partial connection that can be re-used

    auto conn = d_ptr->newConnection();

    conn->source        = srcIdx;
    conn->destination   = destIdx;
    conn->sourceIP      = srcIdx.internalPointer();
    conn->destinationIP = destIdx.internalPointer();

    // Check if there is an internal pointer or UID, models can work without
    // them, better not assume they exist. It is also not possible to assume
    // they are valid or unique. This is checked when it is retrieved.
    if (srcIdx.internalPointer())
        d_ptr->m_hDirectMapping[ srcIdx.internalPointer () ] = conn; //FIXME this limits the number of connections to 1

    if (destIdx.internalPointer())
        d_ptr->m_hDirectMapping[ destIdx.internalPointer() ] = conn; //FIXME this limits the number of connections to 1

    // Sync the current source value into the sink
    d_ptr->synchronize(srcIdx, destIdx);

    d_ptr->notifyConnect(srcIdx, destIdx);

    return true;
}

bool QReactiveProxyModel::areConnected(const QModelIndex& source, const QModelIndex& destination) const
{
    Q_UNUSED(source)
    Q_UNUSED(destination)
    return false; //TODO
}

QList<QModelIndex> QReactiveProxyModel::sendTo(const QModelIndex& source) const
{
    Q_UNUSED(source)
    return {}; //TODO
}

QList<QModelIndex> QReactiveProxyModel::receiveFrom(const QModelIndex& destination) const
{
    Q_UNUSED(destination)
    return {}; //TODO
}


bool ConnectedIndicesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_ASSERT((!index.isValid()) || index.model() == this); //TODO remove

    if ((!index.isValid()) || index.model() != this || !value.canConvert<QModelIndex>())
        return false;

    // This model auto add rows
    if (index.row() == d_ptr->m_lConnections.size())
        d_ptr->newConnection();

    // First, given is a random QModelIndex, it might be in an anonymous proxy.
    // Usually, those are rejected, but here is would void some valid use cases.
    auto i = value.toModelIndex();

    #define M qobject_cast<const QAbstractProxyModel*>(i.model())
    while (i.model() != d_ptr->q_ptr && M && (i = M->mapToSource(i)).isValid());
    #undef M

    // `i` can be invalid (to disconnect)
    Q_ASSERT((!i.isValid()) || i.model() == d_ptr->q_ptr);

    const auto conn = d_ptr->m_lConnections[index.row()];

    const bool wasValid = conn->isValid();

    switch (role) {
        case QReactiveProxyModel::ConnectionsRoles::SOURCE_INDEX: // also DEST
            Q_ASSERT(index.column() != 1);
            if (index.column() == QReactiveProxyModel::ConnectionsColumns::SOURCE && i != conn->source) {
                d_ptr->m_hDirectMapping.remove(conn->sourceIP);

                if (wasValid)
                    Q_EMIT d_ptr->q_ptr->disconnected(conn->source, conn->destination);

                conn->source = i;

                if (i.isValid()) {
                    conn->sourceIP = i.internalPointer();
                    d_ptr->m_hDirectMapping[i.internalPointer()] = conn;
                }
                else
                    conn->sourceIP = nullptr;
                d_ptr->synchronize(conn->source, conn->destination);

                if (conn->isValid())
                    d_ptr->notifyConnect(conn->source, conn->destination);
            }
            else if (index.column() == QReactiveProxyModel::ConnectionsColumns::DESTINATION && i != conn->destination) {
                d_ptr->m_hDirectMapping.remove(conn->destinationIP);

                if (wasValid)
                    Q_EMIT d_ptr->q_ptr->disconnected(conn->source, conn->destination);

                conn->destination = i;

                if (i.isValid()) {
                    conn->destinationIP = i.internalPointer();
                    d_ptr->m_hDirectMapping[i.internalPointer()] = conn;
                }
                else
                    conn->destinationIP = nullptr;
                d_ptr->synchronize(conn->source, conn->destination);

                if (conn->isValid())
                    d_ptr->notifyConnect(conn->source, conn->destination);

                //FIXME this may require a beginInsertRows
            }

            Q_EMIT dataChanged(index, index);

            return true;
    }

    return false;
}

QVariant ConnectedIndicesModel::data(const QModelIndex& idx, int role) const
{
    if ((!idx.isValid()) || d_ptr->m_lConnections.size() <= idx.row())
        return {};

    const auto conn = d_ptr->m_lConnections[idx.row()];

    Q_ASSERT(conn);

    // custom role for this model
    switch(role) {
        case QReactiveProxyModel::ConnectionsRoles::IS_VALID:
            return conn->isValid();
        case QReactiveProxyModel::ConnectionsRoles::IS_USED:
            return conn->isUsed();
        case QReactiveProxyModel::ConnectionsRoles::SOURCE_INDEX:
            switch(idx.column()) {
                case QReactiveProxyModel::ConnectionsColumns::SOURCE:
                    return conn->source;
                case QReactiveProxyModel::ConnectionsColumns::DESTINATION:
                    return conn->destination;
            }
            break;
        case QReactiveProxyModel::ConnectionsRoles::UID:
            // Fallback on the Qt::DisplayRole is done on purpose
            switch(idx.column()) {
                case QReactiveProxyModel::ConnectionsColumns::SOURCE:
                    return conn->source.data(d_ptr->m_ExtraRole[
                        (int) QReactiveProxyModel::ExtraRoles::IdentifierRole
                    ]);
                case QReactiveProxyModel::ConnectionsColumns::DESTINATION:
                    return conn->destination.data(d_ptr->m_ExtraRole[
                        (int) QReactiveProxyModel::ExtraRoles::IdentifierRole
                    ]);
            }
            break;
    }

    // proxy to the source
    switch(idx.column()) {
        case QReactiveProxyModel::ConnectionsColumns::SOURCE:
            return conn->source.data(role);
        case QReactiveProxyModel::ConnectionsColumns::CONNECTION:
            return {}; //Eventually they will be named and have colors
        case QReactiveProxyModel::ConnectionsColumns::DESTINATION:
            return conn->destination.data(role);
    }

    return {};
}

int ConnectedIndicesModel::rowCount(const QModelIndex& parent) const
{
    const bool needNew = (!d_ptr->m_lConnections.size()) ||
        d_ptr->m_lConnections.last()->isValid();

    return parent.isValid() ? 0 : d_ptr->m_lConnections.size() + (needNew?1:0);
}

int ConnectedIndicesModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 3;
}

void QReactiveProxyModelPrivate::clear()
{
    while (!m_lConnections.isEmpty()) {
        notifyDisconnect(m_lConnections.first());
        delete m_lConnections.first();
    }
}

void QReactiveProxyModelPrivate::notifyConnect(const QModelIndex& source, const QModelIndex& destination) const
{
    typedef QReactiveProxyModel::ExtraRoles R;

    Q_EMIT q_ptr->connected(source, destination);

    if (m_HasExtraRole[(int)R::SourceConnectionNotificationRole]) {
        q_ptr->setData(source, destination, m_ExtraRole[(int)R::SourceConnectionNotificationRole]);
    }

    if (m_HasExtraRole[(int)R::DestinationConnectionNotificationRole])
        q_ptr->setData(source, source, m_ExtraRole[(int)R::DestinationConnectionNotificationRole]);

}

void QReactiveProxyModelPrivate::notifyDisconnect(const QModelIndex& source, const QModelIndex& destination) const
{
    typedef QReactiveProxyModel::ExtraRoles R;

    Q_EMIT q_ptr->disconnected(source, destination);

    if (m_HasExtraRole[(int)R::SourceDisconnectionNotificationRole])
        q_ptr->setData(source, destination, m_ExtraRole[(int)R::SourceDisconnectionNotificationRole]);

    if (m_HasExtraRole[(int)R::DestinationDisconnectionNotificationRole])
        q_ptr->setData(source, source, m_ExtraRole[(int)R::DestinationDisconnectionNotificationRole]);
}

void QReactiveProxyModelPrivate::notifyConnect(const ConnectionHolder* conn) const
{
    if (conn->isValid())
        notifyConnect(conn->source, conn->destination);
}

void QReactiveProxyModelPrivate::notifyDisconnect(const ConnectionHolder* conn) const
{
    if (conn->isValid())
        notifyDisconnect(conn->source, conn->destination);
}

bool QReactiveProxyModelPrivate::synchronize(const QModelIndex& s, const QModelIndex& d) const
{
    if ((!s.isValid()) || !d.isValid())
        return false;

    static const QVector<int> fallbackRole {Qt::DisplayRole};

    const auto roles = m_lConnectedRoles.size() ? &m_lConnectedRoles : &(fallbackRole);

    for (int role : qAsConst(*roles)) {
        const auto md = q_ptr->mimeData({s});

        q_ptr->canDropMimeData(
            md, Qt::LinkAction, d.row(), d.column(), d.parent()
        );
        q_ptr->setData(d, s.data(role) , role);
    }

    return true;
}

void QReactiveProxyModelPrivate::slotMimeDestroyed()
{
    // collect the garbage
    m_hDraggedIndexCache.remove(static_cast<QMimeData*>(QObject::sender()));
}

void QReactiveProxyModelPrivate::slotDataChanged(const QModelIndex& tl, const QModelIndex& br)
{
    if (!tl.isValid())
        return;

    // To avoid doing a foreach of the index matrix, this model ties to implement
    // some "hacky" optimizations to keep the overhead low. There is 3 scenarios:
    //
    //  1) There is only 1 changed item. Then use the internal pointers has hash
    //     keys.
    //  2) The top_left...bottom_right is smaller than the number of connected
    //     pairs. Then foreach the matrix
    //  3) The matrix is larger than the number of connections. Then foreach
    //     the connections. And use the `parent()` and `<=` `>=` operators.
    // Only 1 item changed
    if (tl == br) {
        if (const auto conn = m_hDirectMapping[tl.internalPointer()]) {
            if (synchronize(conn->source, conn->destination))
                Q_EMIT m_pConnectionModel->dataChanged(
                    m_pConnectionModel->index(conn->index, 0),
                    m_pConnectionModel->index(conn->index, 2)
                );
        }
    }
    else {
        //TODO make this faster...

        for (int i = tl.row(); i <= br.row(); i++)
            for (int j = tl.column(); j <= br.column(); j++) {
                const auto idx = tl.model()->index(i, j, tl.parent());
                slotDataChanged(idx, idx);
            }
    }

    //TODO implement scenario 3
}
