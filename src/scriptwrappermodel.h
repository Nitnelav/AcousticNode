#ifndef SCRIPTWRAPPERMODEL_H
#define SCRIPTWRAPPERMODEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QException>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>
#include <QChartView>

#include <NodeDataModel>
#include <NodeData>
#include <NodeGeometry>
#include <NodeStyle>
#include <NodePainterDelegate>

#include "moduleexceptions.h"
#include "modulegraph.h"
#include "moduledata.h"
#include "spectrummoduledata.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class ScriptWrapperModel : public NodeDataModel
{
    Q_OBJECT

private:
    QJSEngine* js_;
    QString path_;
    QJSValue module_;

    QString caption_;
    QString description_;
    std::shared_ptr<QLabel> descriptionWidget_;

    QList<std::shared_ptr<ModuleData> > inputs_;
    QList<std::shared_ptr<ModuleData> > parameters_;
    QList<std::shared_ptr<ModuleData> > outputs_;

    std::shared_ptr<ModuleGraph> moduleGraph_;

    QJSValue inputsDefinition_;
    int numInputs_;
    QJSValue inputArgs_;

    QJSValue parametersDefinition_;
    int numParameters_;
    QJSValue parameterArgs_;

    QJSValue outputsDefinition_;
    int numOutputs_;

    NodeValidationState validationState_ = NodeValidationState::Valid;
    QString validationMessage_;

    void calculate();

public:
    ScriptWrapperModel(QJSEngine *engine, QString path);
    virtual ~ScriptWrapperModel() {}

    void setCaption(const QString& caption) { caption_ = caption; }
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

    bool resizable() const override { return true; }

    int numInputs() const { return numInputs_; }
    int numParameters() const { return numParameters_; }
    int numOutputs() const { return numOutputs_; }

    std::shared_ptr<ModuleData> getInputData(int index) const;
    std::shared_ptr<ModuleData> getParameterData(int index) const;
    std::shared_ptr<ModuleData> getOutputData(int index) const;

    ModuleGraph* getModuleGraph() const { return moduleGraph_.get(); }

private slots:

    void parameterChanged();
};

#endif // SCRIPTWRAPPERMODEL_H
