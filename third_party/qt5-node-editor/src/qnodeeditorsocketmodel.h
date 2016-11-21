#pragma once

#include "qtypecoloriserproxy.h"
#include <QtCore/QIdentityProxyModel>

#include <graphicsnodesocket.hpp>

class GraphicsNode;
class QReactiveProxyModel;
class GraphicsNodeScene;
class GraphicsDirectedEdge;

class QNodeEditorSocketModelPrivate;

//TODO move to a subclass
class QNodeEditorEdgeModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit QNodeEditorEdgeModel(QNodeEditorSocketModelPrivate* parent = Q_NULLPTR); //TODO make private
    virtual ~QNodeEditorEdgeModel();

//     virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual QVariant data(const QModelIndex& idx, int role) const override;

    bool canConnect(const QModelIndex& idx1, const QModelIndex& idx2) const;
    bool connectSocket(const QModelIndex& idx1, const QModelIndex& idx2);

    GraphicsNodeScene* scene() const;

    QNodeEditorSocketModel* socketModel() const;

private:
    QNodeEditorSocketModelPrivate* d_ptr;
};

/**
 * Keeper/factory for the nodes and socket objects. This is the only place
 * allowed to create and destroy them.
 *
 * It might seems a little abusive to use a proxy for that, but trying to keep
 * consistency across all 4 "view" classes proved to be challenging and
 * messy. Having a central entity to do it ensure a clear and simple ownership
 * pyramid for each objects.
 *
 * TODO once the model refactoring is done, turn into a private class
 */
class QNodeEditorSocketModel : public QTypeColoriserProxy
{
    Q_OBJECT
public:

    explicit QNodeEditorSocketModel(
        QReactiveProxyModel* rmodel,
        GraphicsNodeScene* scene
    );

    virtual ~QNodeEditorSocketModel();

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual void setSourceModel(QAbstractItemModel *sourceModel) override;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &idx) const override;

    int sourceSocketCount(const QModelIndex& idx) const;
    int sinkSocketCount(const QModelIndex& idx) const;

    QNodeEditorEdgeModel* edgeModel() const;

    GraphicsNodeScene* scene() const;

    GraphicsNode*       getNode(const QModelIndex& idx, bool recursive = false);
    GraphicsNodeSocket* getSourceSocket(const QModelIndex& idx);
    GraphicsNodeSocket* getSinkSocket(const QModelIndex& idx);

    GraphicsDirectedEdge* getSourceEdge(const QModelIndex& idx);
    GraphicsDirectedEdge* getSinkEdge(const QModelIndex& idx);

    Q_INVOKABLE QAbstractItemModel *sinkSocketModel(const QModelIndex& node) const;
    Q_INVOKABLE QAbstractItemModel *sourceSocketModel(const QModelIndex& node) const;

    //TODO in later iterations of the API, add partial connections to the QNodeEditorEdgeModel
    GraphicsDirectedEdge* initiateConnectionFromSource(const QModelIndex& index, const QPointF& point);
    GraphicsDirectedEdge* initiateConnectionFromSink(const QModelIndex& index, const QPointF& point);

private:
    QNodeEditorSocketModelPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QNodeEditorSocketModel)
};
