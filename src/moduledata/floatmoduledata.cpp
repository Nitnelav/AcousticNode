#include "floatmoduledata.h"

FloatModuleData::FloatModuleData(const QString &description):
    ModuleData (description)
{
    spinBox_ = std::make_shared<QDoubleSpinBox>();
    spinBox_->setRange(-DBL_MAX, DBL_MAX);

    connect<void(QDoubleSpinBox::*)(double), void(FloatModuleData::*)(double)>
            (spinBox_.get(), &QDoubleSpinBox::valueChanged, this, &FloatModuleData::valueChanged);
}

std::shared_ptr<QWidget> FloatModuleData::getWidget() const
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
        spinBox_->setDisabled(false);
    } else {
        spinBox_->setValue(0.0);
        spinBox_->setDisabled(true);
    }
}

double FloatModuleData::getValue() const
{
    if (!floatData_) {
        return 0.0;
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
    if (number != floatData_->number()) {
        floatData_->setNumber(number);
        Q_EMIT widgetDataChanged();
    }
}
