#ifndef CHOICEMODULEDATA_H
#define CHOICEMODULEDATA_H

#include <QComboBox>
#include <QJSValue>

#include "moduledata.h"
#include "choicedata.h"

class ChoiceModuleData : public ModuleData
{
    Q_OBJECT

private:
    QStringList choiceList_;

    QComboBox* comboBox_;
    std::shared_ptr<ChoiceData> choiceData_;

    int default_ = 0;

public:
    ChoiceModuleData(const QJSValue& element);

    QWidget* getWidget() const override;
    std::shared_ptr<NodeData> getNodeData() const override;
    void setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData) override;

    int getIndex() const;
    QString getString() const;

    void setIndex(const int& index);

signals:

public slots:

    void currentIndexChanged(int index);
};

#endif // CHOICEMODULEDATA_H
