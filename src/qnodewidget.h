#pragma once

#include "qnodeview.h"

#include <QtCore/QVariant>

class QNodeWidgetPrivate;

class QAbstractItemModel;
class GraphicsNode;

class QNodeWidget : public QNodeView
{
    Q_OBJECT
public:
    enum class ObjectFlags {
        NONE                  = 0,
        USER_PROPERTIES       = 1 << 0,
        DESIGNABLE_PROPERTIES = 1 << 1,
        CLASS_PROPERTIES      = 1 << 2,
        INHERITED_PROPERTIES  = 1 << 3,
        SIGNALS               = 1 << 4,
        SLOTS                 = 1 << 5,
        CLASS_METHODS         = 1 << 6,
        INHERITED_METHODS     = 1 << 7
    };

    explicit QNodeWidget(QWidget* parent = Q_NULLPTR);
    virtual ~QNodeWidget();

    GraphicsNode* addObject(
        QObject*       o,
        const QString& title = QString(),
        ObjectFlags    f     = ObjectFlags::NONE,
        const QVariant& uid  = {}
    );

    GraphicsNode* addModel(
        QAbstractItemModel* m,
        const QString&      title = QString(),
        const QVariant&     uid   = {}
    );

private:
    QNodeWidgetPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QNodeWidget);
};
