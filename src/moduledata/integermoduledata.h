#ifndef INTEGERMODULEDATA_H
#define INTEGERMODULEDATA_H

#include <QSpinBox>

#include "moduledata.h"
#include "integerdata.h"

class IntegerModuleData : public ModuleData
{
    Q_OBJECT

private:
    QSpinBox* spinBox_;
    std::shared_ptr<IntegerData> intData_;
    int default_ = 0;

public:
    IntegerModuleData(const QJSValue& element);

    QWidget* getWidget() const override;
    std::shared_ptr<NodeData> getNodeData() const override;
    void setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData) override;

    int getValue() const;
    void setValue(const int& number);

signals:

public slots:

    void valueChanged(int number);
};
#endif // INTEGERMODULEDATA_H
