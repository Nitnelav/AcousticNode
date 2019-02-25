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

    ConnectionStyle::setConnectionStyle(
    R"(
    {
      "ConnectionStyle": {
        "UseDataDefinedColors": true
      }
    }
    )");

    connect(flowScene_, &FlowScene::nodeCreated, this, &MainWindow::nodeCreated);
    connect(flowScene_, &FlowScene::nodeDeleted, this, &MainWindow::nodeDeleted);
    connect(flowScene_, &FlowScene::nodeContextMenu, this, &MainWindow::nodeContextMenu);
    connect(flowScene_, &FlowScene::selectionChanged, this, &MainWindow::selectionChanged);
    connect(flowScene_, &FlowScene::nodeDoubleClicked, this, &MainWindow::nodeLocked);
}

MainWindow::~MainWindow()
{
    delete flowScene_;
    delete moduleMgr_;
    delete js_;
    delete ui_;
}

void MainWindow::nodeLocked(Node &node)
{

}

void MainWindow::nodeCreated(Node &node)
{
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node.nodeDataModel());
    selectedNode_ = nodeModel;
    ui_->dockWidget->setWidget(nodeModel->getDockWidget());
    nodeModel->getDockWidget()->show();
}

void MainWindow::nodeDeleted(Node &node)
{
    selectedNode_ = nullptr;
    ui_->dockWidget->setWidget(ui_->defaultDock);
}

void MainWindow::selectionChanged()
{
    auto nodes = flowScene_->selectedNodes();
    if (nodes.size() == 0) {
        return;
    }
    auto node = nodes[nodes.size() -1];
    if (nodeDocks_.contains(node->id())) {
        nodeDocks_[node->id()]->raise();
        return;
    }
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node->nodeDataModel());
    if (nodeModel == selectedNode_) {
        return;
    }
    selectedNode_ = nodeModel;
    ui_->dockWidget->setWidget(nodeModel->getDockWidget());
    nodeModel->getDockWidget()->show();
}

void MainWindow::nodeContextMenu(Node &n, const QPointF &pos)
{
    Node* node = &n;
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node->nodeDataModel());
    QMenu* menu = new QMenu();
    QAction* action = menu->addAction("Edit in new block");
    if (nodeModel == selectedNode_) {
        action->setDisabled(true);
    }
    connect(action, &QAction::triggered, [=]() {
        if (nodeDocks_.contains(node->id())) {
            nodeDocks_[node->id()]->raise();
            return;
        }
        QDockWidget* newDock = new QDockWidget(nodeModel->caption());
        newDock->setWidget(nodeModel->getDockWidget());
        nodeModel->getDockWidget()->show();
        if (nodeDocks_.size() == 0) {
            addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, newDock);
        } else {
            tabifyDockWidget(nodeDocks_.last(), newDock);
        }
        nodeDocks_[node->id()] = newDock;
    });

    menu->popup(ui_->flowView->mapToGlobal(ui_->flowView->mapFromScene(pos)));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}
