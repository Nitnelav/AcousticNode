#ifndef MODULEDATA_H
#define MODULEDATA_H

#include <memory>

#include <QObject>
#include <QWidget>

#include <NodeData>

using QtNodes::NodeData;

class ModuleData : public QObject
{
    Q_OBJECT

protected:
    QString description_;

public:
    ModuleData(const QString& description);

    virtual std::shared_ptr<QWidget> getWidget() const = 0;
    virtual std::shared_ptr<NodeData> getNodeData() const = 0;
    virtual void setNodeData(const std::shared_ptr<NodeData> &nodeData) = 0;

    QString description() const
    { return description_; }

signals:
    void widgetDataChanged();

public slots:
};

#endif // MODULEDATA_H
