#include "choicemoduledata.h"

ChoiceModuleData::ChoiceModuleData(const QJSValue& element):
    ModuleData (element)
{
    if (element.property("choices").isArray()) {
        QJSValue list = element.property("choices");
        int length = list.property("length").toInt();
        for (int i = 0; i < length; i++) {
            choiceList_.append(list.property(i).toString());
        }
    }

    if (element.hasProperty("default")) {
        if (element.property("default").isNumber()) {
            default_ = element.property("default").toInt();
        }
        if (element.property("default").isString()) {
            QString string = element.property("default").toString();
            if (int index = choiceList_.indexOf(string) >= 0) {
                default_ = index;
            }
        }
    }

    comboBox_ = new QComboBox();
    comboBox_->insertItems(0, choiceList_);

    connect<void(QComboBox::*)(int), void(ChoiceModuleData::*)(int)>
            (comboBox_, &QComboBox::currentIndexChanged, this, &ChoiceModuleData::currentIndexChanged);
}

QWidget *ChoiceModuleData::getWidget() const
{
    return comboBox_;
}

std::shared_ptr<QtNodes::NodeData> ChoiceModuleData::getNodeData() const
{
    return choiceData_;
}

void ChoiceModuleData::setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData)
{
    choiceData_ = std::static_pointer_cast<ChoiceData>(nodeData);
    if (choiceData_) {
        comboBox_->setCurrentIndex(choiceData_->index());
    } else {
        comboBox_->setCurrentIndex(default_);
    }
}

int ChoiceModuleData::getIndex() const
{
    if (!choiceData_) {
        return default_;
    }
    return choiceData_->index();
}

QString ChoiceModuleData::getString() const
{
    if (!choiceData_) {
        return choiceList_[default_];
    }
    if (choiceData_->string() == "") {
        return choiceList_[default_];
    }
    return choiceData_->string();
}

void ChoiceModuleData::setIndex(const int &index)
{
    if (choiceData_ && index != choiceData_->index()) {
        choiceData_->setIndexAndString(index, choiceList_.at(index));
        comboBox_->setCurrentIndex(index);
    }
}

void ChoiceModuleData::currentIndexChanged(int index)
{
    if (choiceData_ && index != choiceData_->index()) {
        choiceData_->setIndexAndString(index, choiceList_.at(index));
        Q_EMIT widgetDataChanged();
    }
}
