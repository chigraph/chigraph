#include "qnodeview.h"

#include <QtCore/QDebug>

#include "graphicsnode.hpp"

#include "qreactiveproxymodel.h"
#include "graphicsnodescene.hpp"
#include "graphicsnodesocket.hpp"

#include "qnodeeditorsocketmodel.h"

class QNodeViewPrivate final : public QObject
{
public:
    explicit QNodeViewPrivate(QObject* p) : QObject(p) {}

    QReactiveProxyModel     m_Proxy   {this     };
    QAbstractItemModel*     m_pModel  {Q_NULLPTR};
    QVector<GraphicsNode*>  m_lNodes  {         };
    GraphicsNodeScene       m_Scene   {this     };
    QNodeEditorSocketModel* m_pFactory{Q_NULLPTR};
};

QNodeView::QNodeView(QWidget* parent) : GraphicsNodeView(parent),
    d_ptr(new QNodeViewPrivate(this))
{
    d_ptr->m_pFactory = new QNodeEditorSocketModel(&d_ptr->m_Proxy, &d_ptr->m_Scene);

    m_pModel = d_ptr->m_pFactory; //HACK to remove

    d_ptr->m_Scene.setSceneRect(-32000, -32000, 64000, 64000);
    setScene(&d_ptr->m_Scene);
}

QNodeView::~QNodeView()
{
    delete d_ptr;
}

GraphicsNodeScene* QNodeView::scene() const
{
    return &d_ptr->m_Scene;
}

void QNodeView::setModel(QAbstractItemModel* m)
{
    d_ptr->m_pModel = m;
    d_ptr->m_Proxy.setSourceModel(m);
}

GraphicsNode* QNodeView::getNode(const QModelIndex& idx) const
{
    if (!idx.isValid())
        return Q_NULLPTR;

    const auto factoryIdx = d_ptr->m_pFactory->mapFromSource(
        d_ptr->m_Proxy.mapFromSource(idx)
    );

    Q_ASSERT(factoryIdx.isValid());

    return d_ptr->m_pFactory->getNode(factoryIdx);
}

QReactiveProxyModel* QNodeView::reactiveModel() const
{
    return &d_ptr->m_Proxy;
}

QAbstractItemModel *QNodeView::sinkSocketModel(const QModelIndex& node) const
{
    return d_ptr->m_pFactory->sinkSocketModel(node);
}

QAbstractItemModel *QNodeView::sourceSocketModel(const QModelIndex& node) const
{
    return d_ptr->m_pFactory->sourceSocketModel(node);
}

QAbstractItemModel* QNodeView::edgeModel() const
{
    return d_ptr->m_pFactory->edgeModel();
}
