#include "integermoduledata.h"

IntegerModuleData::IntegerModuleData(const QJSValue &element):
    ModuleData (element)
{
    spinBox_ = new QSpinBox();
    spinBox_->setRange(-32768, 32767);

    if (element.hasProperty("default")) {
        default_ = element.property("default").toInt();
    }

    connect<void(QSpinBox::*)(int), void(IntegerModuleData::*)(int)>
            (spinBox_, &QSpinBox::valueChanged, this, &IntegerModuleData::valueChanged);
}

QWidget *IntegerModuleData::getWidget() const
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
    } else {
        spinBox_->setValue(default_);
    }
}

int IntegerModuleData::getValue() const
{
    if (!intData_) {
        return default_;
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
    if (intData_ && number != intData_->number()) {
        intData_->setNumber(number);
        Q_EMIT widgetDataChanged();
    }
}
