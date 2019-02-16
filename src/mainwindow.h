#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJSEngine>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFormLayout>
#include <QDockWidget>

#include <Node>
#include <NodeData>
#include <FlowScene>
#include <FlowView>
#include <ConnectionStyle>
#include <DataModelRegistry>

#include "scriptwrappermodel.h"
#include "modulemanager.h"

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

    FlowScene* flowScene_;
    QDockWidget* parameterDock_;
    QDockWidget* _bddDock;
    QDockWidget* _spectrumDock;

    QFormLayout* parametersLayout_;

    QWidget* parameterWidget_;
    QTabWidget* _bddWidget;
    QWidget* _spectrumWidget;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void nodeSelected(Node &node);
    void selectionChanged();

};

#endif // MAINWINDOW_H
