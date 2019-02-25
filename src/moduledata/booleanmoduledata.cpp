#include "booleanmoduledata.h"

BooleanModuleData::BooleanModuleData(const QString &description):
    ModuleData (description)
{
    checkBox_ = std::make_shared<QCheckBox>();
    checkBox_->setText("");

    connect(checkBox_.get(), &QCheckBox::toggled, this, &BooleanModuleData::toggled);
}

std::shared_ptr<QWidget> BooleanModuleData::getWidget() const
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
        checkBox_->setDisabled(false);
    } else {
        checkBox_->setChecked(false);
        checkBox_->setDisabled(true);
    }
}

bool BooleanModuleData::getValue() const
{
    if (!boolData_) {
        return false;
    }
    return boolData_->boolean();
}

void BooleanModuleData::setValue(const bool &checked)
{
    if (boolData_) {
        boolData_->setBoolean(checked);
    }
}

void BooleanModuleData::toggled()
{
    boolData_->setBoolean(checkBox_->isChecked());

    Q_EMIT widgetDataChanged();
}
