#ifndef NODEDOCKWIDGET_H
#define NODEDOCKWIDGET_H

#include <QDockWidget>
#include <QUuid>

class NodeDockWidget : public QDockWidget
{
    Q_OBJECT

private:
    QUuid nodeId_;

    void closeEvent(QCloseEvent *event) override;

public:
    NodeDockWidget();
    NodeDockWidget(const QUuid& id);
    NodeDockWidget(const QUuid& id, const QString& title);

    QUuid nodeId() const;
    void setNodeId(const QUuid &nodeId);


    QSize sizeHint() const override;


signals:
    void closed(QUuid nodeId);
};

#endif // NODEDOCKWIDGET_H
