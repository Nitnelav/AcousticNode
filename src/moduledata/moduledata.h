#ifndef MODULEDATA_H
#define MODULEDATA_H

#include <memory>

#include <QJSValue>
#include <QObject>
#include <QWidget>

#include <NodeData>

using QtNodes::NodeData;

class ModuleData : public QObject
{
    Q_OBJECT

protected:
    QString id_;
    QString type_;
    QString typeName_;
    QString description_;
    bool required_;

public:
    ModuleData(const QJSValue& element);

    virtual QWidget* getWidget() const = 0;
    virtual std::shared_ptr<NodeData> getNodeData() const = 0;
    virtual void setNodeData(const std::shared_ptr<NodeData> &nodeData) = 0;

    QString description() const
    { return description_; }

    QString id() const
    { return id_; }

    QString type() const
    { return type_; }

    QString typeName() const
    { return typeName_; }

    bool required() const
    { return required_; }

    QString caption() const;

signals:
    void widgetDataChanged();

public slots:
};

#endif // MODULEDATA_H
