#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    QCoreApplication::setApplicationName("AcousticNode");
    ui_->setupUi(this);

    js_ = new QJSEngine();
    moduleMgr_ = new ModuleManager(js_);
    moduleMgr_->setModulesDir("C:\\Users\\valen\\Documents\\GitHub\\AcousticNode\\modules");
    moduleMgr_->loadModules();

    dbMgr_ = new DbManager();
    dbMgr_->addDb("C:\\Users\\valen\\Documents\\GitHub\\AcousticNode\\database.db", "Local DataBase");

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

    setWindowTitle("AcousticNode - new_project.node[*]");
    setWindowModified(true);

    connect(flowScene_, &FlowScene::nodeCreated, this, &MainWindow::nodeCreated);
    connect(flowScene_, &FlowScene::nodeDeleted, this, &MainWindow::nodeDeleted);
    connect(flowScene_, &FlowScene::nodeContextMenu, this, &MainWindow::nodeContextMenu);
    connect(flowScene_, &FlowScene::selectionChanged, this, &MainWindow::selectionChanged);
    connect(flowScene_, &FlowScene::nodeDoubleClicked, this, &MainWindow::nodeLocked);

    QSettings settings("config.ini", QSettings::IniFormat);
    qDebug() << settings.fileName();
    QStringList recentFilePaths = settings.value("recentProjects").toStringList();
    for (int i = 0; i < MAX_RECENT_FILES; ++i) {
        recentFileActs_[i] = new QAction(this);
        recentFileActs_[i]->setVisible(false);
        ui_->menuRecentProjects->addAction(recentFileActs_[i]);
        connect(recentFileActs_[i], &QAction::triggered, this, &MainWindow::openRecentProject);
    }
    updateRecentFileActions();

    connect(ui_->actionSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui_->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui_->actionNewProject, &QAction::triggered, this, &MainWindow::newProject);
    connect(ui_->actionOpenProject, &QAction::triggered, this, &MainWindow::openProject);
    connect(ui_->actionDBBrowse, &QAction::triggered, this, &MainWindow::browseDb);
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
    setWindowModified(true);
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node.nodeDataModel());
    selectedNode_ = &node;
    ui_->dockWidget->setWidget(nodeModel->getDockWidget());
    nodeModel->getDockWidget()->show();
}

void MainWindow::nodeDeleted(Node &node)
{
    setWindowModified(true);
    if (&node == selectedNode_) {
        selectedNode_ = nullptr;
        ui_->dockWidget->setWidget(ui_->defaultDock);
        return;
    }
    if (nodeDocks_.contains(node.id())) {
        nodeDocks_[node.id()]->close();
        nodeDocks_.remove(node.id());
    }
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
    if (node == selectedNode_) {
        return;
    }
    selectedNode_ = node;
    ui_->dockWidget->setWidget(nodeModel->getDockWidget());
    nodeModel->getDockWidget()->show();
}

void MainWindow::nodeContextMenu(Node &n, const QPointF &pos)
{
    Node* node = &n;
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node->nodeDataModel());
    QMenu* menu = new QMenu();
    QAction* action = menu->addAction("Edit in new block");
    connect(action, &QAction::triggered, [=]() {
        if (nodeDocks_.contains(node->id())) {
            nodeDocks_[node->id()]->setVisible(true);
            nodeDocks_[node->id()]->raise();
            return;
        }
        if (node == selectedNode_) {
            selectedNode_ = nullptr;
            ui_->dockWidget->setWidget(ui_->defaultDock);
        }
        NodeDockWidget* newDock = new NodeDockWidget(node->id(), nodeModel->caption());
        connect(newDock, &NodeDockWidget::closed, this, &MainWindow::dockClosed);
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

void MainWindow::dockClosed(QUuid nodeId)
{
    nodeDocks_.remove(nodeId);
}

void MainWindow::save()
{
    if (currentProject_.isEmpty()) {
        return saveAs();
    }
    QFile file(currentProject_);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(flowScene_->saveToMemory());
    }
    setWindowModified(false);
}

void MainWindow::saveAs()
{
    QString fileName =
      QFileDialog::getSaveFileName(nullptr,
                                   tr("Save As AcousticNode Project"),
                                   QDir::homePath(),
                                   tr("AcousticNode Projects (*.node)"));

    if (fileName.isEmpty()) {
        return;
    }
    if (!fileName.endsWith("node", Qt::CaseInsensitive)) {
        fileName += ".node";
    }
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(flowScene_->saveToMemory());
    }
    setCurrentFile(fileName);
    setWindowTitle(QString("AcousticNode - %1[*]").arg(currentProject_));
    setWindowModified(false);
}

void MainWindow::newProject()
{
    if (isWindowModified()) {
        QMessageBox msgBox;
        msgBox.setText("The project has been modified.");
        msgBox.setInformativeText("Do you want to save your work before closing the current Project ?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.exec();
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:
            save();
            break;
        case QMessageBox::Discard:
            // Don't Save was clicked
            break;
        case QMessageBox::Cancel:
            return;
        default:
            break;
      }
    }
    flowScene_->clearScene();
    currentProject_ = QString();
    setWindowTitle(QString("AcousticNode - new_project[*]").arg(currentProject_));
    setWindowModified(false);
}

void MainWindow::openProject()
{
    if (isWindowModified()) {
        QMessageBox msgBox;
        msgBox.setText("The project has been modified.");
        msgBox.setInformativeText("Do you want to save your work before closing the current Project ?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Save:
                save();
                break;
            case QMessageBox::Discard:
                // Don't Save was clicked
                break;
            case QMessageBox::Cancel:
                return;
            default:
                break;
        }
    }
    QString fileName =
      QFileDialog::getOpenFileName(nullptr,
                                   tr("Open AcousticNode Project"),
                                   QDir::homePath(),
                                   tr("AcousticNode Projects (*.node)"));

    if (!QFileInfo::exists(fileName)) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray wholeFile = file.readAll();

    flowScene_->clearScene();
    flowScene_->loadFromMemory(wholeFile);
    setCurrentFile(fileName);
    setWindowTitle(QString("AcousticNode - %1[*]").arg(currentProject_));
    setWindowModified(false);
}

void MainWindow::openRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString fileName = action->data().toString();
        if (!QFileInfo::exists(fileName)) {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray wholeFile = file.readAll();

        flowScene_->clearScene();
        flowScene_->loadFromMemory(wholeFile);
        setCurrentFile(fileName);
        setWindowTitle(QString("AcousticNode - %1[*]").arg(currentProject_));
        setWindowModified(false);
    }
}

void MainWindow::browseDb()
{
    dbMgr_->getSearchDialog()->open();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentProject_ = fileName;
    setWindowFilePath(currentProject_);

    QSettings settings("config.ini", QSettings::IniFormat);
    QStringList files = settings.value("recentProjects").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MAX_RECENT_FILES) {
        files.removeLast();
    }

    settings.setValue("recentProjects", files);

    updateRecentFileActions();
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("config.ini", QSettings::IniFormat);
    QStringList files = settings.value("recentProjects").toStringList();
    qDebug() << settings.fileName();
    qDebug() << files;
    int numRecentFiles = qMin(files.size(), MAX_RECENT_FILES);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("%1").arg(strippedName(files[i]));
        recentFileActs_[i]->setText(text);
        recentFileActs_[i]->setData(files[i]);
        recentFileActs_[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MAX_RECENT_FILES; ++j) {
        recentFileActs_[j]->setVisible(false);
    }
}
