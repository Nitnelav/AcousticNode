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

    flowScene_ = new FlowScene();
    flowScene_->setRegistry(moduleMgr_->getModuleRegistry());
    ui_->flowView->setScene(flowScene_);

    parameterDock_ = new QDockWidget("Parameters", this);
    parameterDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    parameterWidget_ = new QWidget(this);
    parametersLayout_ = new QFormLayout();
    parametersLayout_->setRowWrapPolicy(QFormLayout::WrapAllRows);
    parameterWidget_->setLayout(parametersLayout_);
    parameterDock_->setWidget(parameterWidget_);
    addDockWidget(Qt::LeftDockWidgetArea, parameterDock_);

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
    auto nodes = flowScene_->selectedNodes();
    if (nodes.size() == 0) {
        // clear selection
        qDebug() << "empty selection";
        return;
    }
    qDebug() << nodes.size() << "nodes selected";
    auto node = nodes[nodes.size() -1];
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node->nodeDataModel());
    for (int index = 0; index < nodeModel->numParameters(); ++index) {
        NodeDataType type = nodeModel->getParameterData(index)->type();
        if (type.id == "LwSpectrum") {
            auto parameter = std::dynamic_pointer_cast<SpectrumData>(nodeModel->getParameterData(index));
            parametersLayout_->addWidget(parameter->getTableWidget());
        }
    }
    auto nodeData = std::dynamic_pointer_cast<SpectrumData>(nodeModel->outData(0));
    if (nodeData) {
        qDebug() << "last selected node value : " << nodeData->getValue(0);
    }
}
