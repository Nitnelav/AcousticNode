#include "floatmoduledata.h"

FloatModuleData::FloatModuleData(const QJSValue &element):
    ModuleData (element)
{
    spinBox_ = new QDoubleSpinBox();
    spinBox_->setRange(-DBL_MAX, DBL_MAX);

    if (element.hasProperty("default")) {
        default_ = element.property("default").toNumber();
    }

    connect<void(QDoubleSpinBox::*)(double), void(FloatModuleData::*)(double)>
            (spinBox_, &QDoubleSpinBox::valueChanged, this, &FloatModuleData::valueChanged);
}

QWidget *FloatModuleData::getWidget() const
{
    return spinBox_;
}

std::shared_ptr<QtNodes::NodeData> FloatModuleData::getNodeData() const
{
    return floatData_;
}

void FloatModuleData::setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData)
{
    floatData_ = std::static_pointer_cast<FloatData>(nodeData);
    if (floatData_) {
        spinBox_->setValue(floatData_->number());
    } else {
        spinBox_->setValue(default_);
    }
}

double FloatModuleData::getValue() const
{
    if (!floatData_) {
        return default_;
    }
    return floatData_->number();
}

void FloatModuleData::setValue(const double &number)
{
    if (floatData_ && number != floatData_->number()) {
        floatData_->setNumber(number);
        spinBox_->setValue(number);
    }
}

void FloatModuleData::valueChanged(double number)
{
    if (floatData_ && number != floatData_->number()) {
        floatData_->setNumber(number);
        Q_EMIT widgetDataChanged();
    }
}
