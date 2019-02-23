#ifndef MODULEGRAPH_H
#define MODULEGRAPH_H

#include <QObject>
#include <QGraphicsLayout>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>


#include "spectrummoduledata.h"

using namespace QtCharts;

class ModuleGraph : public QChart
{
    Q_OBJECT

private:
    QBarCategoryAxis* freqAxis_;
    QValueAxis* levelAxis_;

    QBarSeries* barSeries_;
    QList<QBarSet*> barSetList_;

public:
    ModuleGraph();

    void appendSpectrumData(std::shared_ptr<SpectrumModuleData> data);
    void removeSpectrumData(std::shared_ptr<SpectrumModuleData> data);

    bool isVisible(QBarSet* set);
    void showBarSet(QBarSet* set);
    void hideBarSet(QBarSet* set);


    QList<QBarSet *> barSetList() const{
        return barSetList_;
    }

signals:

public slots:
    void valueChanged(int);
};

#endif // MODULEGRAPH_H
