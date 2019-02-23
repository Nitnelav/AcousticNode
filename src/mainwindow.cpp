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

    leftInputsLayout_ = static_cast<QFormLayout*>(ui_->leftInputForm->layout());
    leftParametersLayout_ = static_cast<QFormLayout*>(ui_->leftParameterForm->layout());
    leftOutputsLayout_ = static_cast<QFormLayout*>(ui_->leftOutputForm->layout());

    rightInputsLayout_ = static_cast<QFormLayout*>(ui_->rightInputForm->layout());
    rightParametersLayout_ = static_cast<QFormLayout*>(ui_->rightParameterForm->layout());
    rightOutputsLayout_ = static_cast<QFormLayout*>(ui_->rightOutputForm->layout());

    QList<int> sizes;
    sizes << 300 << 300 << 0;
    ui_->splitter->setSizes(sizes);

    ui_->leftGraphChart->setContextMenuPolicy(Qt::CustomContextMenu);
    ui_->leftGraphChart->setChart(&defaultGraph_);

    connect(ui_->leftGraphChart, &QChartView::customContextMenuRequested, this, &MainWindow::leftGraphContextMenu);

    connect(flowScene_, &FlowScene::nodeDeleted, this, &MainWindow::nodeDeleted);
    connect(flowScene_, &FlowScene::selectionChanged, this, &MainWindow::selectionChanged);
    connect(flowScene_, &FlowScene::nodeDoubleClicked, this, &MainWindow::nodeLocked);
}

MainWindow::~MainWindow()
{
    delete moduleMgr_;
    delete js_;
    delete ui_;
}

void MainWindow::nodeLocked(Node &node)
{
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node.nodeDataModel());
    if (nodeModel == lockedNode_ || nodeModel == selectedNode_) {
        return;
    }
    QLayoutItem *item = nullptr;
    while ((item = rightInputsLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    while ((item = rightParametersLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    while ((item = rightOutputsLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    lockedNode_ = nullptr;
    ui_->rightGraphChart->setChart(&defaultGraph_);
    ui_->rightCaptionLineEdit->setText("");
    ui_->rightDescriptionTextEdit->setPlainText("");
    lockedNode_ = nodeModel;
    ui_->rightCaptionLineEdit->setDisabled(false);
    ui_->rightCaptionLineEdit->setText(nodeModel->caption());
    ui_->rightDescriptionTextEdit->setDisabled(false);
    ui_->rightDescriptionTextEdit->setPlainText(nodeModel->description());
    ui_->rightGraphChart->setChart(nodeModel->getModuleGraph());

    for (int index = 0; index < nodeModel->numInputs(); ++index) {
        if (!nodeModel->getInputData(index))
            continue;
        auto input = std::dynamic_pointer_cast<ModuleData>(nodeModel->getInputData(index));
        QWidget* widget = input->getWidget().get();
        rightInputsLayout_->addRow(input->description(), widget);
        widget->setDisabled(true);
        widget->show();
    }
    for (int index = 0; index < nodeModel->numParameters(); ++index) {
        auto parameter = std::dynamic_pointer_cast<ModuleData>(nodeModel->getParameterData(index));
        QWidget* widget = parameter->getWidget().get();
        rightParametersLayout_->addRow(parameter->description(), widget);
        widget->show();
    }
    for (int index = 0; index < nodeModel->numOutputs(); ++index) {
        if (!nodeModel->getOutputData(index))
            continue;
        auto output = std::dynamic_pointer_cast<ModuleData>(nodeModel->getOutputData(index));
        QWidget* widget = output->getWidget().get();
        rightOutputsLayout_->addRow(output->description(), widget);
        widget->setDisabled(true);
        widget->show();
    }
}

void MainWindow::nodeDeleted(QtNodes::Node &node)
{
    QLayoutItem *item = nullptr;
    while ((item = leftInputsLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    while ((item = leftParametersLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    while ((item = leftOutputsLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    selectedNode_ = nullptr;
    ui_->leftGraphChart->setChart(&defaultGraph_);
    ui_->leftCaptionLineEdit->setText("");
    ui_->leftDescriptionTextEdit->setPlainText("");
}

void MainWindow::selectionChanged()
{
    auto nodes = flowScene_->selectedNodes();
    if (nodes.size() == 0) {
        return;
    }
    auto node = nodes[nodes.size() -1];
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node->nodeDataModel());
    if (nodeModel == selectedNode_) {
        return;
    }
    QLayoutItem *item = nullptr;
    while ((item = leftInputsLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    while ((item = leftParametersLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    while ((item = leftOutputsLayout_->takeAt(0)) != nullptr) {
        item->widget()->hide();
    }
    selectedNode_ = nullptr;
    ui_->leftGraphChart->setChart(&defaultGraph_);
    ui_->leftCaptionLineEdit->setText("");
    ui_->leftDescriptionTextEdit->setPlainText("");
    selectedNode_ = nodeModel;
    ui_->leftCaptionLineEdit->setDisabled(false);
    ui_->leftCaptionLineEdit->setText(nodeModel->caption());
    ui_->leftDescriptionTextEdit->setDisabled(false);
    ui_->leftDescriptionTextEdit->setPlainText(nodeModel->description());
    ui_->leftGraphChart->setChart(nodeModel->getModuleGraph());

    for (int index = 0; index < nodeModel->numInputs(); ++index) {
        if (!nodeModel->getInputData(index))
            continue;
        auto input = std::dynamic_pointer_cast<ModuleData>(nodeModel->getInputData(index));
        QWidget* widget = input->getWidget().get();
        leftInputsLayout_->addRow(input->description(), widget);
        widget->setDisabled(true);
        widget->show();
    }
    for (int index = 0; index < nodeModel->numParameters(); ++index) {
        auto parameter = std::dynamic_pointer_cast<ModuleData>(nodeModel->getParameterData(index));
        QWidget* widget = parameter->getWidget().get();
        leftParametersLayout_->addRow(parameter->description(), widget);
        widget->show();
    }
    for (int index = 0; index < nodeModel->numOutputs(); ++index) {
        if (!nodeModel->getOutputData(index))
            continue;
        auto output = std::dynamic_pointer_cast<ModuleData>(nodeModel->getOutputData(index));
        QWidget* widget = output->getWidget().get();
        leftOutputsLayout_->addRow(output->description(), widget);
        widget->setDisabled(true);
        widget->show();
    }
}

void MainWindow::leftGraphContextMenu(QPoint pos)
{
    if (!selectedNode_) {
        return;
    }
    QMenu* menu = new QMenu(ui_->leftGraphChart);
    QList<QBarSet*> setList = selectedNode_->getModuleGraph()->barSetList();

    for (int index = 0; index < setList.size(); index++) {
        QBarSet* set = setList[index];
        QAction* action = menu->addAction(set->label());
        action->setCheckable(true);
        action->setChecked(selectedNode_->getModuleGraph()->isVisible(set));
        action->setProperty("barSetIndex", index);
        connect(action, &QAction::toggled, this, &MainWindow::toggleBarSet);
    }

    menu->popup(ui_->leftGraphChart->mapToGlobal(pos));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void MainWindow::toggleBarSet()
{
    if (!selectedNode_) {
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    bool checked = action->isChecked();
    int barSetIndex = action->property("barSetIndex").toInt();
    QBarSet* set = selectedNode_->getModuleGraph()->barSetList()[barSetIndex];
    if (checked) {
        selectedNode_->getModuleGraph()->showBarSet(set);
    } else {
        selectedNode_->getModuleGraph()->hideBarSet(set);
    }
}



void MainWindow::on_leftCaptionLineEdit_textEdited(const QString &caption)
{
    if (selectedNode_) {
        selectedNode_->setCaption(caption);
    }
}

void MainWindow::on_leftDescriptionTextEdit_textChanged()
{
    if (selectedNode_) {
        selectedNode_->setDescription(ui_->leftDescriptionTextEdit->toPlainText());
    }
}
