#include "nodedockwidget.h"

NodeDockWidget::NodeDockWidget():
    QDockWidget(nullptr)
{
}

NodeDockWidget::NodeDockWidget(const QUuid& id):
    QDockWidget(nullptr),
    nodeId_(id)
{
}

NodeDockWidget::NodeDockWidget(const QUuid& id, const QString &title):
    QDockWidget(title, nullptr),
    nodeId_(id)
{
}

QUuid NodeDockWidget::nodeId() const
{
    return nodeId_;
}

void NodeDockWidget::setNodeId(const QUuid &nodeId)
{
    nodeId_ = nodeId;
}

QSize NodeDockWidget::sizeHint() const
{
    return QSize(400, 200);
}

void NodeDockWidget::closeEvent(QCloseEvent *event)
{
    QWidget* child = widget();
    setWidget(nullptr);
    child->setParent(nullptr);
    QDockWidget::closeEvent(event);
    emit closed(nodeId_);
    delete this;
}

