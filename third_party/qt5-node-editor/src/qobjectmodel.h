#pragma once

#include <QtCore/QAbstractItemModel>

class QObjectModelPrivate;

/**
 * TODO not fully implemented yet
 * Automatically a list of QObject into a model.
 * 
 * It supports:
 *
 *  * Properties ("USER=true" ones are enabled by default)
 *  * Signal (disabled by default)
 *  * Slots (disabled by default)
 *
 * It also supports to dispositions:
 *
 * * Horizontal (each property has its columns, the object name is used for the
 *   vertical header)
 * * Vertical (all properties are added on new rows, mixing the objects)
 *
 * It also supports introspection filters to control the content of the model
 */
class QObjectModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class PropertyChangeReceiver; // for createIndex()
public:
    enum Capabilities : char {
        NONE   = 0 << 0,
        READ   = 1 << 0,
        WRITE  = 1 << 1,
        NOTIFY = 1 << 2,
        CONST  = 1 << 3,
        USER   = 1 << 4,
    };

    enum Role {
        ValueRole = Qt::UserRole+1,
        PropertyIdRole,
        PropertyNameRole,
        CapabilitiesRole,
        MetaTypeRole,
        TypeNameRole,
    };

    explicit QObjectModel(QObject* parent = Q_NULLPTR);
    QObjectModel(const QList<QObject*> objs, Qt::Orientation = Qt::Horizontal, int displayRole = Qt::DisplayRole, QObject* parent = Q_NULLPTR);
    virtual ~QObjectModel();

    virtual QVariant data(const QModelIndex& idx, int role) const override;
    virtual int rowCount(const QModelIndex& parent = {}) const override;
    virtual int columnCount(const QModelIndex& parent = {}) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent ={}) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &idx) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual QHash<int, QByteArray> roleNames() const override;
    virtual QModelIndex parent(const QModelIndex& idx) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /** In heterogeneous mode, this model take all the properties and mix
     * them. When this is disabled (default), only the common properties are
     * added to the model (to preserve the table columns consistency)
     **/
    bool heterogeneous() const;
    void setHeterogeneous(bool value); //TODO

    /**
     * By default, this models enable "setData" when the property is writable.
     */
    bool isReadOnly() const; //TODO
    void setReadOnly(bool value);

    /// By default, the value is used, but it can be configured to use something else
    int displayRole() const;
    void setDisplayRole(int role);

    /// Display as a list or a table
    Qt::Orientation oridentation() const;
    void setOrientation(Qt::Orientation o);

    /**
     * Add objects to be displayed in the model.
     */
    void addObject(QObject* obj);
    void addObjects(const QVector<QObject*>& objs);
    void addObjects(const QList<QObject*>& objs);

private:
    QObjectModelPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QObjectModel)
};
