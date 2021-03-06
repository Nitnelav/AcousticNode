#include "spectrummoduledata.h"

SpectrumModuleData::SpectrumModuleData(const QJSValue &element) : ModuleData (element)
{
    tableWidget_ = new QTableWidget(1, 8);
    barSet_ = new QBarSet(description_);
    QStringList headers = QStringList() << "63Hz" << "125Hz" << "250Hz" << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz";
    tableWidget_->setHorizontalHeaderLabels(headers);
    tableWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget_->verticalHeader()->setStretchLastSection(true);
    tableWidget_->verticalHeader()->hide();
    tableWidget_->resizeColumnsToContents();
    tableWidget_->setMaximumHeight(70);
    tableWidget_->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tableWidget_, &QTableWidget::cellChanged, this, &SpectrumModuleData::cellChanged);
    connect(tableWidget_, &QTableWidget::customContextMenuRequested, this, &SpectrumModuleData::widgetContextMenuRequested);

}

QWidget* SpectrumModuleData::getWidget() const
{
    return tableWidget_;
}

std::shared_ptr<QtNodes::NodeData> SpectrumModuleData::getNodeData() const
{
    return spectrumData_;
}

void SpectrumModuleData::setNodeData(const std::shared_ptr<QtNodes::NodeData> &nodeData)
{
    spectrumData_ = std::static_pointer_cast<SpectrumData>(nodeData);
    barSet_->remove(0, 8);
    if (spectrumData_) {
        tableWidget_->setRowCount(1);
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq)
        {
            tableWidget_->setItem(0, freq, new QTableWidgetItem(QString::number(spectrumData_->getValue(freq))));
            barSet_->append(spectrumData_->getValue(freq));
        }
    } else {
        tableWidget_->removeRow(0);
    }
}

double SpectrumModuleData::getValue(int freq) const
{
    if (!spectrumData_) {
        return -99.0; // should not happen
    }
    return spectrumData_->getValue(freq);
}

void SpectrumModuleData::setValue(int freq, double value)
{
    if (spectrumData_ && value != spectrumData_->getValue(freq)) {
        spectrumData_->setValue(freq, value);
        barSet_->replace(freq, value);
        tableWidget_->item(0, freq)->setText(QString::number(value));
        Q_EMIT widgetDataChanged();
    }
}
void SpectrumModuleData::cellChanged(int row, int freq)
{
    bool ok;
    double value = tableWidget_->item(row, freq)->text().toDouble(&ok);
    if (!ok || value > 1000) {
        tableWidget_->item(row, freq)->setText(QString::number(spectrumData_->getValue(freq)));
        return;
    }
    if (value != spectrumData_->getValue(freq)) {
        spectrumData_->setValue(freq, value);
        barSet_->replace(freq, value);
        Q_EMIT widgetDataChanged();
    }
}

void SpectrumModuleData::widgetContextMenuRequested(const QPoint &pos)
{
    Q_EMIT contextMenuRequested(pos, this);
}
