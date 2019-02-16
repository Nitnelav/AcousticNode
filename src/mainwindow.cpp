#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    js_ = new QJSEngine();
    moduleMgr_ = new ModuleManager(js_);
    moduleMgr_->setModulesDir("C:\\Users\\valen\\Documents\\GitHub\\AcousticNode\\modules");
    moduleMgr_->loadModules();

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    flowScene_ = new FlowScene(this);
    flowScene_->setRegistry(moduleMgr_->getModuleRegistry());
    ui_->flowView->setScene(flowScene_);

//    parameterDock_ = new QDockWidget("Parameters", this);
//    parameterDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//    parameterDock_->setMinimumWidth(400);
//    parameterDock_->setFeatures(QDockWidget::DockWidgetMovable);
//    parameterWidget_ = new QWidget(this);
//    parameterWidget_->setMinimumWidth(400);
//    parametersLayout_ = new QFormLayout();
//    parametersLayout_->setRowWrapPolicy(QFormLayout::WrapAllRows);
//    parameterWidget_->setLayout(parametersLayout_);
//    parameterDock_->setWidget(parameterWidget_);
//    addDockWidget(Qt::LeftDockWidgetArea, parameterDock_);
    parameterWidget_ = ui_->parameterFormWidget;
    parametersLayout_ = static_cast<QFormLayout*>(ui_->parameterFormWidget->layout());
    connect(flowScene_, &FlowScene::selectionChanged, this, &MainWindow::selectionChanged);
    connect(flowScene_, &FlowScene::nodeDoubleClicked, this, &MainWindow::nodeSelected);
}

MainWindow::~MainWindow()
{
    delete moduleMgr_;
    delete js_;
    delete ui_;
}

void MainWindow::nodeSelected(Node &node)
{
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node.nodeDataModel());
    auto nodeData = std::dynamic_pointer_cast<SpectrumData>(nodeModel->outData(0));
    qDebug() << "dble click";
    if (nodeData) {
        qDebug() << nodeData->getValue(0);
    }
}

void MainWindow::selectionChanged()
{
    QLayoutItem *item = nullptr;
    while ((item = parametersLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    auto nodes = flowScene_->selectedNodes();
    if (nodes.size() == 0) {
        // clear selection
        qDebug() << "empty selection";
        return;
    }
    qDebug() << nodes.size() << "nodes selected";
    auto node = nodes[nodes.size() -1];
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node->nodeDataModel());
    for (int index = 0; index < nodeModel->numInputs(); ++index) {
        if (!nodeModel->getInputData(index))
            continue;
        NodeDataType type = nodeModel->getInputData(index)->type();
        if (type.id == "LwSpectrum") {
            auto input = std::dynamic_pointer_cast<SpectrumData>(nodeModel->getInputData(index));
            parametersLayout_->addRow(input->description(), input->getTableWidget());
            input->getTableWidget()->setDisabled(true);
            input->getTableWidget()->show();
        }
    }
    for (int index = 0; index < nodeModel->numParameters(); ++index) {
        NodeDataType type = nodeModel->getParameterData(index)->type();
        if (type.id == "LwSpectrum") {
            auto parameter = std::dynamic_pointer_cast<SpectrumData>(nodeModel->getParameterData(index));
            parametersLayout_->addRow(parameter->description(), parameter->getTableWidget());
            parameter->getTableWidget()->show();
        }
    }
    for (int index = 0; index < nodeModel->numOutputs(); ++index) {
        if (!nodeModel->getOutputData(index))
            continue;
        NodeDataType type = nodeModel->getOutputData(index)->type();
        if (type.id == "LwSpectrum") {
            auto output = std::dynamic_pointer_cast<SpectrumData>(nodeModel->getOutputData(index));
            parametersLayout_->addRow(output->description(), output->getTableWidget());
            output->getTableWidget()->setDisabled(true);
            output->getTableWidget()->show();
        }
    }
    auto nodeData = std::dynamic_pointer_cast<SpectrumData>(nodeModel->outData(0));
    if (nodeData) {
        qDebug() << "last selected node value : " << nodeData->getValue(0);
    }
}

