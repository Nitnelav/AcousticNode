#include "excelexporter.h"

ExcelExporter::ExcelExporter()
{

}

bool ExcelExporter::exportScene(QtNodes::FlowScene *scene, QString fileName)
{
    QStringList freq_strings = {"63Hz", "125Hz", "250Hz", "500Hz", "1kHz", "2kHz", "4kHz", "8kHz"};

    QXlsx::Document xlsx;

    QByteArray data = scene->saveToMemory();

    QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

    int nodeIndex = 0;
    int row = 1, col = 1;

    for (QJsonValueRef node : nodesJsonArray)
    {
        QJsonObject nodeRoot = node.toObject();
        QJsonObject nodeModel = nodeRoot["model"].toObject();
        QUuid id(nodeRoot["id"].toString());

        xlsx.addSheet(id.toString(QUuid::WithoutBraces));
//        xlsx.addSheet(QString::number(nodeIndex) + "_" + nodeModel["name"].toString());

        Worksheet *sheet = xlsx.currentWorksheet();

        row = 1;
        col = 1;
        sheet->write(CellReference(row, col), "id");
        col++;
        sheet->write(CellReference(row, col), id.toString(QUuid::WithoutBraces));

        row++;
        col = 1;
        sheet->write(CellReference(row, col), "name");
        col++;
        sheet->write(CellReference(row, col), nodeModel["name"].toString());

        row++;
        col = 1;
        sheet->write(CellReference(row, col), "description");
        col++;
        sheet->write(CellReference(row, col), nodeModel["description"].toString());

        for (QString group: {"inputs", "parameters", "readonly_outputs", "outputs"}) {

            row += 2;
            col = 1;
            sheet->write(CellReference(row, col), group);
            row += 1;

            QJsonObject elements = nodeModel[group].toObject();

            for (QString key: elements.keys()) {
                QJsonObject element = elements[key].toObject();
                QString type = element["type"].toString();

                row += 1;
                col = 2;
                sheet->write(CellReference(row, col), key);
                col++;
                sheet->write(CellReference(row, col), element["description"].toString());

                row += 1;
                col = 2;
                sheet->write(CellReference(row, col), "data");
                row += 1;
                sheet->write(CellReference(row, col), type);
                col += 1;
                if (type == "spectrum") {
                    for (QString freq: freq_strings) {
                        sheet->write(CellReference(row, col), type);
                    }
                    for (int freq = 0; freq <= 7; freq++) {
                        row -= 1;
                        sheet->write(CellReference(row, col + freq), freq_strings[freq]);
                        row += 1;
                        sheet->write(CellReference(row, col + freq), element["data"].toArray()[freq].toDouble());
                    }
                } else {
                    sheet->write(CellReference(row, col), element["data"].toVariant());
                }
                row += 1;
            }
        }

        nodeIndex++;
    }

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

    for (QJsonValueRef connection : connectionJsonArray)
    {
        QJsonObject connectionData = connection.toObject();
    }

    return xlsx.saveAs(fileName);
}
