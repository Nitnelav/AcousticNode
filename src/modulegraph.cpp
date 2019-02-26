#include "modulegraph.h"

ModuleGraph::ModuleGraph():
    QChart(nullptr, nullptr)
{
    barSeries_ = new QBarSeries();
    addSeries(barSeries_);
    setAnimationOptions(QChart::NoAnimation);
    setTheme(QChart::ChartThemeLight);
    layout()->setContentsMargins(0, 0, 0, 0);
    setBackgroundRoundness(0);

    QStringList categories;
    categories << "63" << "125" << "250" << "500" << "1k" << "2k" << "4k" << "8k";
    freqAxis_ = new QBarCategoryAxis();
    freqAxis_->append(categories);
    freqAxis_->setTitleText("Hz");
    addAxis(freqAxis_, Qt::AlignBottom);
    barSeries_->attachAxis(freqAxis_);

    levelAxis_ = new QValueAxis();
    levelAxis_->setTickInterval(10.0);
    levelAxis_->setTickType(QValueAxis::TicksDynamic);
    levelAxis_->setTitleText("dB");
    addAxis(levelAxis_, Qt::AlignLeft);
    levelAxis_->setRange(0.0, 5.0);
    levelAxis_->setTickInterval(1.0);
    barSeries_->attachAxis(levelAxis_);
}

void ModuleGraph::appendSpectrumData(std::shared_ptr<SpectrumModuleData> data)
{
    barSeries_->append(data->getBarSet());
    barSetList_.append(data->getBarSet());
    connect(data->getBarSet(), &QBarSet::valueChanged, this, &ModuleGraph::valueChanged);
}

void ModuleGraph::removeSpectrumData(std::shared_ptr<SpectrumModuleData> data)
{
}

bool ModuleGraph::isVisible(QBarSet *set)
{
    return barSeries_->barSets().contains(set);
}

void ModuleGraph::showBarSet(QBarSet *set)
{
    if (!barSeries_->barSets().contains(set)) {
       barSeries_->append(set);
    }
}

void ModuleGraph::hideBarSet(QBarSet *set)
{
    if (barSeries_->barSets().contains(set)) {
       barSeries_->take(set);
    }
}



void ModuleGraph::valueChanged(int)
{
    double max = 0;
    double min = 0;
    foreach(auto serie, barSeries_->barSets()) {
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
            double value = serie->at(freq);
            if (value > max) {
                max = value;
            }
            if (value < min) {
                min = value;
            }
        }
    }
    min = min == 0.0 ? 0.0 : min - double(5 + int(min) % 5);
    max = max == 0.0 ? 0.0 : max + double(5 - int(max) % 5);
    if (min == 0.0 && max == 0.0) {
        levelAxis_->setRange(0.0, 5.0);
    }
    levelAxis_->setRange(min, max);
    int range = int(max - min);
    double interval = 1.0;
    if (range > 10) {
        interval = 2.0;
    }
    if (range > 20) {
        interval = 5.0;
    }
    if (range > 50) {
        interval = 10.0;
    }
    if (range > 100) {
        interval = 20.0;
    }
    if (range > 200) {
        interval = 40.0;
    }
    levelAxis_->setTickInterval(interval);
}
