#include "booleanmoduledata.h"

BooleanModuleData::BooleanModuleData(const QJSValue &element):
    ModuleData (element)
{
    checkBox_ = new QCheckBox();
    checkBox_->setText("");

    if (element.hasProperty("default")) {
        default_ = element.property("default").toBool();
    }

    connect(checkBox_, &QCheckBox::toggled, this, &BooleanModuleData::toggled);
}

QWidget *BooleanModuleData::getWidget() const
{
    return checkBox_;
}

std::shared_ptr<QtNodes::NodeData> BooleanModuleData::getNodeData() const
{
    return boolData_;
}

void BooleanModuleData::setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData)
{
    boolData_ = std::static_pointer_cast<BooleanData>(nodeData);
    if (boolData_) {
        checkBox_->setChecked(boolData_->boolean());
    } else {
        checkBox_->setChecked(default_);
    }
}

bool BooleanModuleData::getValue() const
{
    if (!boolData_) {
        return default_;
    }
    return boolData_->boolean();
}

void BooleanModuleData::setValue(const bool &checked)
{
    if (boolData_) {
        boolData_->setBoolean(checked);
        checkBox_->setChecked(checked);
    }
}

void BooleanModuleData::toggled()
{
    if (boolData_) {
        boolData_->setBoolean(checkBox_->isChecked());
    }

    Q_EMIT widgetDataChanged();
}
