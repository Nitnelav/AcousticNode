#ifndef SPECTRUMDATA_H
#define SPECTRUMDATA_H

#define FREQ_63Hz 0
#define FREQ_125Hz 1
#define FREQ_250Hz 2
#define FREQ_500Hz 3
#define FREQ_1kHz 4
#define FREQ_2kHz 5
#define FREQ_4kHz 6
#define FREQ_8kHz 7

#define SPECTRUM_TYPE_ALPHA 0
#define SPECTRUM_TYPE_R 1
#define SPECTRUM_TYPE_D 2
#define SPECTRUM_TYPE_LW 3
#define SPECTRUM_TYPE_LP 4

#include <QObject>
#include <QTableWidget>
#include <QHeaderView>

#include <NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class SpectrumData : public QObject, public NodeData
{
    Q_OBJECT

private:
    QList<float> values_;
    int type_;

    QString descritpion_;
    QTableWidget* tableWidget_;

public:
    SpectrumData(int type, QString descritpion, float val_63 = -99.0, float val_125 = -99.0, float val_250 = -99.0, float val_500 = -99.0, float val_1k = -99.0, float val_2k = -99.0, float val_4k = -99.0, float val_8k = -99.0):
        type_(type), descritpion_(descritpion)
    {
        values_.append(val_63);
        values_.append(val_125);
        values_.append(val_250);
        values_.append(val_500);
        values_.append(val_1k);
        values_.append(val_2k);
        values_.append(val_4k);
        values_.append(val_8k);

        tableWidget_ = new QTableWidget(1, 8);
        QStringList headers = QStringList() << "63Hz" << "125Hz" << "250Hz" << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz";
        tableWidget_->setHorizontalHeaderLabels(headers);
        tableWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableWidget_->verticalHeader()->setStretchLastSection(true);
        tableWidget_->verticalHeader()->hide();
        tableWidget_->resizeColumnsToContents();
        tableWidget_->setMaximumHeight(70);

        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq)
        {
            tableWidget_->setItem(0, freq, new QTableWidgetItem(QString::number(values_[freq])));
        }

        connect(tableWidget_, &QTableWidget::cellChanged, this, &SpectrumData::cellChanged);
    }

    NodeDataType type() const override
    {
        switch (type_) {
        case SPECTRUM_TYPE_ALPHA:
            return NodeDataType {
                "AlphaSpectrum",
                "Alpha Spectrum"
            };
        case SPECTRUM_TYPE_R:
            return NodeDataType {
                "RSpectrum",
                "R Spectrum"
            };
        case SPECTRUM_TYPE_D:
            return NodeDataType {
                "DSpectrum",
                "D Spectrum"
            };
        case SPECTRUM_TYPE_LW:
            return NodeDataType {
                "LwSpectrum",
                "Lw Spectrum"
            };
        case SPECTRUM_TYPE_LP:
            return NodeDataType {
                "LpSpectrum",
                "Lp Spectrum"
            };
        default:
            return NodeDataType {
                "Unknown",
                "Unknown"
            };
        }

    }

    float getValue(int freq) const
    {
        return values_[freq];
    }

    void setValue(int freq, float value)
    {
        values_[freq] = value;
        tableWidget_->item(0, freq)->setText(QString::number(value));
    }

    QTableWidget* getTableWidget()
    { return tableWidget_; }

private Q_SLOTS:

    void cellChanged(int row, int freq) {
        float value = tableWidget_->item(row, freq)->text().toFloat();
        values_[freq] = value;
        Q_EMIT widgetDataChanged();
    }

Q_SIGNALS:

    void widgetDataChanged();
};

#endif // SPECTRUMDATA_H
