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

#include <NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class SpectrumData : public NodeData
{
private:
    QList<double> values_;

public:
    SpectrumData(double val_63 = 0., double val_125 = 0.0, double val_250 = 0.0, double val_500 = 0.0,
                 double val_1k = 0.0, double val_2k = 0.0, double val_4k = 0.0, double val_8k = 0.0)
    {
        values_.append(val_63);
        values_.append(val_125);
        values_.append(val_250);
        values_.append(val_500);
        values_.append(val_1k);
        values_.append(val_2k);
        values_.append(val_4k);
        values_.append(val_8k);
    }

    ~SpectrumData() override {}

    NodeDataType type() const override
    {
        return NodeDataType {
            "spectrum",
            "Spectrum"
        };
    }

    double getValue(int freq) const
    {
        return values_[freq];
    }

    void setValue(int freq, double value)
    {
        values_[freq] = value;
    }
};

#endif // SPECTRUMDATA_H
