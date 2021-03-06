#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAX_RECENT_FILES 5

#include <QMainWindow>
#include <QJSEngine>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFormLayout>
#include <QDockWidget>
#include <QMap>
#include <QUuid>
#include <QSettings>
#include <QFileDialog>
#include <QtCharts/QChart>
#include <QMessageBox>
#include <QColorDialog>
#include <QWidgetAction>
#include <QTreeWidget>
#include <QAction>
#include <QInputDialog>

#include <Node>
#include <NodeData>
#include <NodeGroup>
#include <NodeStyle>
#include <FlowScene>
#include <FlowView>
#include <ConnectionStyle>
#include <DataModelRegistry>

#include "nodedockwidget.h"
#include "scriptwrappermodel.h"
#include "modulemanager.h"
#include "dbmanager.h"
#include "dbpathdialog.h"
#include "dbeditdialog.h"
#include "modulepathdialog.h"
#include "modulegraph.h"
#include "excelexporter.h"

using namespace QtCharts;

using QtNodes::DataModelRegistry;
using QtNodes::Node;
using QtNodes::NodeStyle;
using QtNodes::FlowScene;
using QtNodes::NodeGroup;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui_;

    QJSEngine* js_;
    ModuleManager* moduleMgr_;
    DbManager* dbMgr_;

    QStringList nodeNames_;

    Node* selectedNode_;
    Node* lockedNode_;

    FlowScene* flowScene_;
    QMap<QUuid, NodeDockWidget*> nodeDocks_;

    QString currentProject_;
    QAction* recentFileActs_[MAX_RECENT_FILES];

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();

    QString strippedName(const QString &fullFileName)
    {
        return QFileInfo(fullFileName).fileName();
    }

    QColor normalNodeColor_;
    QColor dockedNodeColor_;

    void setStyles();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    void closeEvent(QCloseEvent *event);

private slots:
    void nodeLocked(Node &node);
    void nodeCreated(Node &node);
    void nodeDeleted(Node &node);
    void selectionChanged();

    void nodeContextMenu(Node& n, const QPointF& pos);
    void groupContextMenu(NodeGroup& g, const QPointF& pos);

    void dockClosed(QUuid nodeId);

    bool save(); // true = successful
    bool saveAs(); // true = successful
    bool exportAs();
    void newProject();
    void openProject();
    void openRecentProject();

    void nodeMenu();

    void browseDb();
};

#endif // MAINWINDOW_H
