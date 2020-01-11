#ifndef SCRIPTWRAPPERMODEL_H
#define SCRIPTWRAPPERMODEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QDir>
#include <QException>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>
#include <QChartView>
#include <QMenu>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QSplitter>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <NodeDataModel>
#include <NodeData>
#include <NodeGeometry>
#include <NodeStyle>
#include <NodePainterDelegate>

#include "dbmanager.h"
#include "moduleexceptions.h"
#include "modulegraph.h"
#include "moduledata/moduledata.h"
#include "moduledata/spectrummoduledata.h"
#include "moduledata/floatmoduledata.h"
#include "moduledata/integermoduledata.h"
#include "moduledata/booleanmoduledata.h"
#include "moduledata/choicemoduledata.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeStyle;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;
using QtNodes::NodePainterDelegate;

class ScriptWrapperModel : public NodeDataModel
{
    Q_OBJECT

private:
    QJSEngine* js_;
    DbManager* db_;
    QString path_;
    QJSValue module_;

    QString caption_;
    QString description_;
    std::shared_ptr<QLabel> descriptionWidget_;

    QList<std::shared_ptr<ModuleData> > inputs_;
    QList<std::shared_ptr<ModuleData> > parameters_;
    QList<std::shared_ptr<ModuleData> > roOutputs_;
    QList<std::shared_ptr<ModuleData> > outputs_;

    ModuleGraph* moduleChart_;
    QChartView* moduleChartView_;

    QJSValue inputsDefinition_;
    int numInputs_;
    QJSValue inputArgs_;

    QJSValue parametersDefinition_;
    int numParameters_;
    QJSValue parameterArgs_;

    QJSValue roOutputsDefinition_;
    int numRoOutputs_;

    QJSValue outputsDefinition_;
    int numOutputs_;

    NodeValidationState validationState_ = NodeValidationState::Valid;
    QString validationMessage_;

    QWidget* dockWidget_;

    bool hasPicture_ = false;
    QPixmap pixmap_;

    void setupDockWidget();
    void calculate();

public:
    ScriptWrapperModel(QJSEngine *engine, DbManager* db, QString path);
    virtual ~ScriptWrapperModel() override;

    void setCaption(const QString& caption);
    /// Caption is used in GUI
    QString caption() const override;

    void setDescription(const QString& description);
    QString description() const { return description_; }

    /// Name makes this model unique
    QString name() const override;

    /// Port caption is used in GUI to label individual ports
    virtual QString portCaption(PortType portType, PortIndex index) const override;

    /// nPorts defines the number of ports for input and output types
    unsigned int nPorts(PortType portType) const override;

    /// dataType defines the type (Lw, Lp, ...) of ports for input and output types
    NodeDataType dataType(PortType portType, PortIndex index) const override;

    /// outData is the pointer to the resulting output data for the given ouput port
    std::shared_ptr<NodeData> outData(PortIndex index) override;

    /// setInData is the pointer to the input data for the given input port, it triggers the calculation
    void setInData(std::shared_ptr<NodeData> data, PortIndex index) override;

    NodeValidationState validationState() const override;

    QString validationMessage() const override;

    QWidget * embeddedWidget() override;

    bool resizable() const override { return false; }

    QJsonObject save() const override;
    void restore(QJsonObject const & json) override;

    int numInputs() const { return numInputs_; }
    int numParameters() const { return numParameters_; }
    int numOutputs() const { return numOutputs_; }

    std::shared_ptr<ModuleData> getInputData(int index) const;
    std::shared_ptr<ModuleData> getParameterData(int index) const;
    std::shared_ptr<ModuleData> getOutputData(int index) const;

    ModuleGraph* getModuleGraph() const { return moduleChart_; }
    QChartView* getModuleChartView() const { return moduleChartView_; }
    QWidget *getDockWidget() const { return dockWidget_; }

private slots:

    void parameterChanged();
    void graphContextMenu(const QPoint &pos);
    void toggleBarSet();
    void toggleNR();
    void spectrumContextMenu(const QPoint &pos, SpectrumModuleData* spectrumModuleData);
    void setFromDb();

signals:
    void captionChanged();

};

#endif // SCRIPTWRAPPERMODEL_H
