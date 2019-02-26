#ifndef FLOATMODULEDATA_H
#define FLOATMODULEDATA_H

#include "float.h"

#include <QDoubleSpinBox>

#include "moduledata.h"
#include "floatdata.h"

class FloatModuleData : public ModuleData
{
    Q_OBJECT

private:
    QDoubleSpinBox* spinBox_;
    std::shared_ptr<FloatData> floatData_;

public:
    FloatModuleData(const QString& description);

    QWidget* getWidget() const override;
    std::shared_ptr<NodeData> getNodeData() const override;
    void setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData) override;

    double getValue() const;
    void setValue(const double& number);

signals:

public slots:

    void valueChanged(double value);
};

#endif // FLOATMODULEDATA_H
