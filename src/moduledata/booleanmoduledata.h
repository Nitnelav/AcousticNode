#ifndef BOOLEANMODULEDATA_H
#define BOOLEANMODULEDATA_H

#include <QCheckBox>

#include "moduledata.h"
#include "booleandata.h"

class BooleanModuleData : public ModuleData
{
    Q_OBJECT

private:

    QCheckBox* checkBox_;
    std::shared_ptr<BooleanData> boolData_;

public:
    BooleanModuleData(const QString& description);

    QWidget* getWidget() const override;
    std::shared_ptr<NodeData> getNodeData() const override;
    void setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData) override;

    bool getValue() const;
    void setValue(const bool& checked);

signals:

public slots:

    void toggled();
};

#endif // BOOLEANMODULEDATA_H
