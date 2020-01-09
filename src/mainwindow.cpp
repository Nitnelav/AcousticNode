#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define INIT_SETTINGS 0

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    QCoreApplication::setApplicationName("AcousticNode");
    ui_->setupUi(this);

    int size = 0;
    QSettings settings("config.ini", QSettings::IniFormat);

#if INIT_SETTINGS
    settings.beginWriteArray("databases");
    settings.setArrayIndex(0);
    settings.setValue("name", "Local DataBase");
    settings.setValue("path", "C:\\Users\\valen\\Documents\\GitHub\\AcousticNode\\database.db");
//    settings.setValue("path", "/home/valoo/Projects/AcousticNode/database.db");
    settings.endArray();

    settings.beginWriteArray("modules");
    settings.setArrayIndex(0);
//    settings.setValue("dir", "/home/valoo/Projects/AcousticNode/modules");
    settings.setValue("dir", "C:\\Users\\valen\\Documents\\GitHub\\AcousticNode\\modules");
    settings.endArray();
#endif

    dbMgr_ = new DbManager();

    size = settings.beginReadArray("databases");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        QString path = settings.value("path").toString();
        if (!QFile::exists(path)) {
            continue;
        }
        dbMgr_->addDb(path, name);
    }
    settings.endArray();

    js_ = new QJSEngine();
    js_->installExtensions(QJSEngine::ConsoleExtension);
    moduleMgr_ = new ModuleManager(js_, dbMgr_);

    size = settings.beginReadArray("modules");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString dir = settings.value("dir").toString();
        if (!QFile::exists(dir)) {
            continue;
        }
        moduleMgr_->addModulesDir(dir);
    }
    settings.endArray();
    moduleMgr_->loadModules();

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    flowScene_ = new FlowScene();
    flowScene_->setRegistry(moduleMgr_->getModuleRegistry());
    ui_->flowView->setScene(flowScene_);

    setStyles();

    setWindowTitle("AcousticNode - new_project.node[*]");
    setWindowModified(true);

    connect(flowScene_, &FlowScene::nodeCreated, this, &MainWindow::nodeCreated);
    connect(flowScene_, &FlowScene::nodeDeleted, this, &MainWindow::nodeDeleted);
    connect(flowScene_, &FlowScene::nodeContextMenu, this, &MainWindow::nodeContextMenu);
    connect(flowScene_, &FlowScene::groupContextMenu, this, &MainWindow::groupContextMenu);
    connect(flowScene_, &FlowScene::selectionChanged, this, &MainWindow::selectionChanged);
    connect(flowScene_, &FlowScene::nodeDoubleClicked, this, &MainWindow::nodeLocked);

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
    connect(ui_->actionExcel, &QAction::triggered, this, &MainWindow::exportAs);
    connect(ui_->actionNewProject, &QAction::triggered, this, &MainWindow::newProject);
    connect(ui_->actionOpenProject, &QAction::triggered, this, &MainWindow::openProject);
    connect(ui_->actionDBBrowse, &QAction::triggered, this, &MainWindow::browseDb);
    connect(ui_->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui_->actionAddNode, &QAction::triggered, this, &MainWindow::nodeMenu);
    connect(ui_->actionDBSetPaths, &QAction::triggered, [=]() {
        DbPathDialog* dlg = new DbPathDialog(dbMgr_);
        dlg->exec();
    });
    connect(ui_->actionModulesSetPaths, &QAction::triggered, [=]() {
        ModulePathDialog* dlg = new ModulePathDialog(moduleMgr_);
        dlg->exec();
        moduleMgr_->loadModules();
        flowScene_->setRegistry(moduleMgr_->getModuleRegistry());
    });
}

MainWindow::~MainWindow()
{
    delete flowScene_;
    delete moduleMgr_;
    delete js_;
    delete ui_;
}

void MainWindow::closeEvent(QCloseEvent *event)
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
                if (save()) {
                    event->accept();
                } else {
                    event->ignore();
                }
                break;
            case QMessageBox::Discard:
                event->accept();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
            default:
                break;
        }
    } else {
        event->accept();
    }
}

void MainWindow::nodeLocked(Node &node)
{
//    node.nodeDataModel()->setNodeStyle(style);
}

void MainWindow::nodeCreated(Node &node)
{
    setWindowModified(true);
    ScriptWrapperModel* nodeModel = static_cast<ScriptWrapperModel*>(node.nodeDataModel());
    selectedNode_ = &node;
    ui_->dockWidget->setWidget(nodeModel->getDockWidget());
    nodeModel->getDockWidget()->show();
    connect(nodeModel, &ScriptWrapperModel::captionChanged, [&](){
        node.nodeGraphicsObject().setFocus();
        node.nodeGraphicsObject().clearFocus();
    });
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
        NodeStyle style = nodeModel->nodeStyle();
        style.NormalBoundaryColor = dockedNodeColor_;
        nodeModel->setNodeStyle(style);
        nodeDocks_[node->id()] = newDock;
    });
    menu->popup(ui_->flowView->mapToGlobal(ui_->flowView->mapFromScene(pos)));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void MainWindow::groupContextMenu(NodeGroup &g, const QPointF &pos)
{
    NodeGroup* group = &g;
    QMenu* menu = new QMenu();
    QAction* colorAction = menu->addAction("Set color ...");
    colorAction->setIcon(QIcon(":/_icons/color-painting-palette.png"));
    connect(colorAction, &QAction::triggered, [=]() {
        QColor color = QColorDialog::getColor();
        group->setColor(color);
    });

    QAction* renameAction = menu->addAction("Set name ...");
    renameAction->setIcon(QIcon(":/_icons/pencil-write.png"));
    connect(renameAction, &QAction::triggered, [=]() {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Rename Group"),
                                             tr("Group name:"), QLineEdit::Normal,
                                             group->title(), &ok);
        if (ok && !text.isEmpty())
            group->setTitle(text);
    });

    menu->addSeparator();

    QAction* moduleAction = menu->addAction("Add module ...");
    moduleAction->setIcon(QIcon(":/_icons/module-add.png"));
    moduleAction->setProperty("position", pos);
    connect(moduleAction, &QAction::triggered, this, &MainWindow::nodeMenu);

    menu->addSeparator();

    QAction* deleteAction = menu->addAction("Delete");
    deleteAction->setIcon(QIcon(":/_icons/bin.png"));
    connect(deleteAction, &QAction::triggered, [&]() {
        flowScene_->removeGroup(g);
    });
    menu->popup(ui_->flowView->mapToGlobal(ui_->flowView->mapFromScene(pos)));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void MainWindow::dockClosed(QUuid nodeId)
{
    flowScene_->clearSelection();
    nodeDocks_.remove(nodeId);
    auto nodes = flowScene_->allNodes();
    flowScene_->iterateOverNodes([=](Node* node) {
        if (node->id() == nodeId) {
            NodeStyle style = node->nodeDataModel()->nodeStyle();
            style.NormalBoundaryColor = normalNodeColor_;
            node->nodeDataModel()->setNodeStyle(style);
            node->nodeGraphicsObject().update();
        }
    });
}

bool MainWindow::save() // true = successful
{
    if (currentProject_.isEmpty()) {
        return saveAs();
    }
    QFile file(currentProject_);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(flowScene_->saveToMemory());
    }
    setWindowModified(false);
    return true;
}

bool MainWindow::saveAs() // true = successful
{
    QString fileName =
      QFileDialog::getSaveFileName(nullptr,
                                   tr("Save As AcousticNode Project"),
                                   QDir::homePath(),
                                   tr("AcousticNode Projects (*.node)"));

    if (fileName.isEmpty()) {
        return false;
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

    return true;
}


bool MainWindow::exportAs() // true = successful
{
    QString fileName =
      QFileDialog::getSaveFileName(nullptr,
                                   tr("Export As Excel File"),
                                   QDir::homePath(),
                                   tr("Excel File (*.xlsx)"));

    if (fileName.isEmpty()) {
        return false;
    }
    if (!fileName.endsWith("xlsx", Qt::CaseInsensitive)) {
        fileName += ".xlsx";
    }

    return ExcelExporter::exportScene(flowScene_, fileName);
}

void MainWindow::newProject()
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
                if (save()) {
                    break;
                } else {
                    return;
                }
            case QMessageBox::Discard:
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
    dbMgr_->openSearchDialog(false);
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

void MainWindow::nodeMenu()
{
    QObject* sender = QObject::sender();

    QMenu* modelMenu = new QMenu();

    auto skipText = QStringLiteral("skip me");

    //Add filterbox to the context menu
    auto *txtBox = new QLineEdit(modelMenu);

    txtBox->setPlaceholderText(QStringLiteral("Filter"));
    txtBox->setClearButtonEnabled(true);

    auto *txtBoxAction = new QWidgetAction(modelMenu);
    txtBoxAction->setDefaultWidget(txtBox);

    modelMenu->addAction(txtBoxAction);

    //Add result treeview to the context menu
    auto *treeView = new QTreeWidget(modelMenu);
    treeView->header()->close();

    auto *treeViewAction = new QWidgetAction(modelMenu);
    treeViewAction->setDefaultWidget(treeView);

    modelMenu->addAction(treeViewAction);

    QMap<QString, QTreeWidgetItem*> topLevelItems;
    for (auto const &cat : flowScene_->registry().categories())
    {
      auto item = new QTreeWidgetItem(treeView);
      item->setText(0, cat);
      item->setData(0, Qt::UserRole, skipText);
      topLevelItems[cat] = item;
    }

    for (auto const &assoc : flowScene_->registry().registeredModelsCategoryAssociation())
    {
      auto parent = topLevelItems[assoc.second];
      auto item   = new QTreeWidgetItem(parent);
      item->setText(0, assoc.first);
      item->setData(0, Qt::UserRole, assoc.first);
    }

    treeView->expandAll();

    connect(treeView, &QTreeWidget::itemClicked, [&](QTreeWidgetItem *item, int)
    {
      QString modelName = item->data(0, Qt::UserRole).toString();

      if (modelName == skipText)
      {
        return;
      }

      auto type = flowScene_->registry().create(modelName);

      if (type)
      {
        auto& node = flowScene_->createNode(std::move(type));

        if (sender && sender->property("position").isValid()) {
            QPoint position = sender->property("position").toPoint();
            node.nodeGraphicsObject().setPos(position);
        }
        modelMenu->close();
        flowScene_->nodePlaced(node);
      }
      else
      {
        qDebug() << "Model not found";
      }
    });

    //Setup filtering
    connect(txtBox, &QLineEdit::textChanged, [&](const QString &text)
    {
      for (auto& topLvlItem : topLevelItems)
      {
        for (int i = 0; i < topLvlItem->childCount(); ++i)
        {
          auto child = topLvlItem->child(i);
          auto modelName = child->data(0, Qt::UserRole).toString();
          const bool match = (modelName.contains(text, Qt::CaseInsensitive));
          child->setHidden(!match);
        }
      }
    });

    // make sure the text box gets focus so the user doesn't have to click on it
    txtBox->setFocus();

    modelMenu->exec(QCursor::pos());
}

void MainWindow::setStyles()
{

    ConnectionStyle::setConnectionStyle(
    R"(
    {
      "ConnectionStyle": {
        "UseDataDefinedColors": true
      }
    }
    )");

    normalNodeColor_ = QColor("#FFA800");
    dockedNodeColor_ = QColor("#c13ed6").lighter(200); //#773C6D //#932d82

}
