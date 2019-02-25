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
    std::shared_ptr<QTableWidget> tableWidget_;
    std::shared_ptr<SpectrumData> spectrumData_;
    std::shared_ptr<QBarSet> barSet_;

public:
    SpectrumModuleData(const QString& description);

    std::shared_ptr<QWidget> getWidget() const override;
    std::shared_ptr<NodeData> getNodeData() const override;
    void setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData) override;

    std::shared_ptr<QBarSet> getBarSet() const
    { return barSet_; }

    double getValue(int freq) const;
    void setValue(int freq, double value);

signals:

public slots:

    void cellChanged(int row, int freq);

};

#endif // SPECTRUMMODULEDATA_H
