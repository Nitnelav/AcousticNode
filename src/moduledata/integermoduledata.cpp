#include "integermoduledata.h"

IntegerModuleData::IntegerModuleData(const QString &description):
    ModuleData (description)
{
    spinBox_ = std::make_shared<QSpinBox>();
    spinBox_->setRange(-32768, 32767);

    connect<void(QSpinBox::*)(int), void(IntegerModuleData::*)(int)>
            (spinBox_.get(), &QSpinBox::valueChanged, this, &IntegerModuleData::valueChanged);
}

std::shared_ptr<QWidget> IntegerModuleData::getWidget() const
{
    return spinBox_;
}

std::shared_ptr<QtNodes::NodeData> IntegerModuleData::getNodeData() const
{
    return intData_;
}

void IntegerModuleData::setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData)
{
    intData_ = std::static_pointer_cast<IntegerData>(nodeData);
    if (intData_) {
        spinBox_->setValue(intData_->number());
        spinBox_->setDisabled(false);
    } else {
        spinBox_->setValue(0);
        spinBox_->setDisabled(true);
    }
}

int IntegerModuleData::getValue() const
{
    if (!intData_) {
        return 0;
    }
    return intData_->number();
}

void IntegerModuleData::setValue(const int &number)
{
    if (intData_ && number != intData_->number()) {
        intData_->setNumber(number);
        spinBox_->setValue(number);
    }
}

void IntegerModuleData::valueChanged(int number)
{
    if (number != intData_->number()) {
        intData_->setNumber(number);
        Q_EMIT widgetDataChanged();
    }
}
