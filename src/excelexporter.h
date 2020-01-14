#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <FlowScene>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QUuid>

// [0] include QXlsx headers
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

using QtNodes::FlowScene;

using namespace QXlsx;

class ExcelExporter
{
public:
    ExcelExporter();

    static bool exportScene(FlowScene* scene, QString fileName);
};

#endif // EXCELEXPORTER_H
