#pragma once

#include <QtCore/QIdentityProxyModel>

class QTypeColoriserProxyPrivate;

/**
 * Set the background and foreground color based on a role QMetaType.
 */
class QTypeColoriserProxy : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit QTypeColoriserProxy(QObject* parent);
    virtual ~QTypeColoriserProxy();

    virtual QVariant data(const QModelIndex& idx, int role) const override;

    int baseRole() const;
    void setBaseRole(int role);

    void setForegroundRole(quint32 typeId, const QVariant& value);
    void setBackgroundRole(quint32 typeId, const QVariant& value);

    template<typename T>
    void setForegroundRole(const QVariant& value) {
        setForegroundRole(qMetaTypeId<T>(), value);
    }

    template<typename T>
    void setBackgroundRole(const QVariant& value) {
        setBackgroundRole(qMetaTypeId<T>(), value);
    }

private:
    QTypeColoriserProxyPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QTypeColoriserProxy)
};
