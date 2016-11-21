#include "qtypecoloriserproxy.h"

#include <QtCore/QDebug>

class QTypeColoriserProxyPrivate
{
public:
    int m_Role {Qt::EditRole};

    QHash<quint32, QVariant> m_lBg;
    QHash<quint32, QVariant> m_lFg;
};

QTypeColoriserProxy::QTypeColoriserProxy(QObject* parent) : QIdentityProxyModel(parent),
    d_ptr(new QTypeColoriserProxyPrivate)
{
}

QVariant QTypeColoriserProxy::data(const QModelIndex& idx, int role) const
{
    if (!idx.isValid())
        return {};

    switch(role) {
        case Qt::BackgroundRole:
            return d_ptr->m_lBg[idx.data(d_ptr->m_Role).userType()];
        case Qt::ForegroundRole:
            return d_ptr->m_lFg[idx.data(d_ptr->m_Role).userType()];
    };

    return QIdentityProxyModel::data(idx, role);
}

QTypeColoriserProxy::~QTypeColoriserProxy()
{
    delete d_ptr;
}

int QTypeColoriserProxy::baseRole() const
{
    return d_ptr->m_Role;
}

void QTypeColoriserProxy::setBaseRole(int role)
{
    d_ptr->m_Role = role;
    Q_EMIT dataChanged(index(0,0), index(rowCount()-1, columnCount()-1));
}

void QTypeColoriserProxy::setForegroundRole(quint32 typeId, const QVariant& value)
{
    d_ptr->m_lFg[typeId] = value;
    Q_EMIT dataChanged(index(0,0), index(rowCount()-1, columnCount()-1));
}

void QTypeColoriserProxy::setBackgroundRole(quint32 typeId, const QVariant& value)
{
    d_ptr->m_lBg[typeId] = value;
    Q_EMIT dataChanged(index(0,0), index(rowCount()-1, columnCount()-1));
}
