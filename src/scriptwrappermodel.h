#ifndef SCRIPTWRAPPERMODEL_H
#define SCRIPTWRAPPERMODEL_H

#include <QObject>
#include <QWidget>
#include <QException>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>

#include <NodeDataModel>
#include <NodeData>
#include <NodeGeometry>
#include <NodeStyle>
#include <NodePainterDelegate>

#include "moduleexceptions.h"
#include "nodedata/spectrumdata.h"
#include "nodedata/booleandata.h"
#include "nodedata/integerdata.h"
#include "nodedata/floatdata.h"

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

    QList<std::shared_ptr<NodeData> > inputs_;
    QList<std::shared_ptr<NodeData> > parameters_;
    QList<std::shared_ptr<NodeData> > outputs_;

    QList<QWidget*> inputWidgets_;
    QList<QWidget*> parameterWidgets_;
    QList<QWidget*> outputWidgets_;

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

    void setupParameters();
    void calculate();

public:
    ScriptWrapperModel(QJSEngine *engine, QString path);
    ~ScriptWrapperModel() override;

    /// Caption is used in GUI
    QString caption() const override;

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

    QWidget * embeddedWidget() override { return nullptr; }

    int numInputs() const { return numInputs_; }
    int numParameters() const { return numParameters_; }
    int numOutputs() const { return numOutputs_; }

    std::shared_ptr<NodeData> getInputData(int index) const;
    std::shared_ptr<NodeData> getParameterData(int index) const;
    std::shared_ptr<NodeData> getOutputData(int index) const;

private slots:

    void parameterChanged();
};

#endif // SCRIPTWRAPPERMODEL_H
