#ifndef SPECTRUMMODULEDATA_H
#define SPECTRUMMODULEDATA_H

#include <QTableWidget>
#include <QHeaderView>
#include <QtCharts/QBarSet>

#include "moduledata.h"
#include "spectrumdata.h"

using namespace QtCharts;

class SpectrumModuleData : public ModuleData
{
    Q_OBJECT

protected:
    QTableWidget* tableWidget_;
    QBarSet* barSet_;
    std::shared_ptr<SpectrumData> spectrumData_;

public:
    SpectrumModuleData(const QJSValue& element);

    QWidget* getWidget() const override;
    std::shared_ptr<NodeData> getNodeData() const override;
    void setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData) override;

    QBarSet* getBarSet() const
    { return barSet_; }

    double getValue(int freq) const;
    void setValue(int freq, double value);

signals:
    void contextMenuRequested(const QPoint& pos, SpectrumModuleData* spectrumModuleData);

public slots:

    void cellChanged(int row, int freq);
    void widgetContextMenuRequested(const QPoint& pos);

};

#endif // SPECTRUMMODULEDATA_H
