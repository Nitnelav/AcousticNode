#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJSEngine>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFormLayout>
#include <QDockWidget>
#include <QtCharts/QChart>

#include <Node>
#include <NodeData>
#include <FlowScene>
#include <FlowView>
#include <ConnectionStyle>
#include <DataModelRegistry>

#include "scriptwrappermodel.h"
#include "modulemanager.h"
#include "modulegraph.h"

using namespace QtCharts;

using QtNodes::DataModelRegistry;
using QtNodes::Node;
using QtNodes::FlowScene;
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

    ScriptWrapperModel* selectedNode_;
    ScriptWrapperModel* lockedNode_;

    FlowScene* flowScene_;
    QDockWidget* parameterDock_;
    QDockWidget* _bddDock;
    QDockWidget* _spectrumDock;

    QFormLayout* leftInputsLayout_;
    QFormLayout* leftParametersLayout_;
    QFormLayout* leftOutputsLayout_;

    QFormLayout* rightInputsLayout_;
    QFormLayout* rightParametersLayout_;
    QFormLayout* rightOutputsLayout_;

    ModuleGraph defaultGraph_;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void nodeLocked(Node &node);
    void nodeDeleted(Node &node);
    void selectionChanged();

    void leftGraphContextMenu(QPoint pos);
    void toggleBarSet();

    void on_leftCaptionLineEdit_textEdited(const QString &caption);
    void on_leftDescriptionTextEdit_textChanged();


};

#endif // MAINWINDOW_H
