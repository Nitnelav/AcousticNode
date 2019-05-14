#-------------------------------------------------
#
# Project created by QtCreator 2019-02-11T13:38:31
#
#-------------------------------------------------

QT       += core gui qml opengl charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AcousticNode
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

DEFINES += NODE_EDITOR_STATIC

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/scriptwrappermodel.cpp \
    externals/nodeeditor/src/Connection.cpp \
    externals/nodeeditor/src/ConnectionBlurEffect.cpp \
    externals/nodeeditor/src/ConnectionGeometry.cpp \
    externals/nodeeditor/src/ConnectionGraphicsObject.cpp \
    externals/nodeeditor/src/ConnectionPainter.cpp \
    externals/nodeeditor/src/ConnectionState.cpp \
    externals/nodeeditor/src/ConnectionStyle.cpp \
    externals/nodeeditor/src/DataModelRegistry.cpp \
    externals/nodeeditor/src/FlowScene.cpp \
    externals/nodeeditor/src/FlowView.cpp \
    externals/nodeeditor/src/FlowViewStyle.cpp \
    externals/nodeeditor/src/Node.cpp \
    externals/nodeeditor/src/NodeConnectionInteraction.cpp \
    externals/nodeeditor/src/NodeDataModel.cpp \
    externals/nodeeditor/src/NodeGeometry.cpp \
    externals/nodeeditor/src/NodeGraphicsObject.cpp \
    externals/nodeeditor/src/NodePainter.cpp \
    externals/nodeeditor/src/NodeState.cpp \
    externals/nodeeditor/src/NodeStyle.cpp \
    externals/nodeeditor/src/Properties.cpp \
    externals/nodeeditor/src/StyleCollection.cpp \
    src/modulemanager.cpp \
    src/modulegraph.cpp \
    src/moduledata/moduledata.cpp \
    src/moduledata/spectrummoduledata.cpp \
    src/moduledata/booleanmoduledata.cpp \
    src/moduledata/floatmoduledata.cpp \
    src/moduledata/integermoduledata.cpp \
    src/nodedockwidget.cpp \
    src/dbmanager.cpp \
    src/moduledata/choicemoduledata.cpp

HEADERS += \
    src/mainwindow.h \
    src/scriptwrappermodel.h \
    externals/nodeeditor/include/nodes/internal/Compiler.hpp \
    externals/nodeeditor/include/nodes/internal/Connection.hpp \
    externals/nodeeditor/include/nodes/internal/ConnectionGeometry.hpp \
    externals/nodeeditor/include/nodes/internal/ConnectionGraphicsObject.hpp \
    externals/nodeeditor/include/nodes/internal/ConnectionState.hpp \
    externals/nodeeditor/include/nodes/internal/ConnectionStyle.hpp \
    externals/nodeeditor/include/nodes/internal/DataModelRegistry.hpp \
    externals/nodeeditor/include/nodes/internal/Export.hpp \
    externals/nodeeditor/include/nodes/internal/FlowScene.hpp \
    externals/nodeeditor/include/nodes/internal/FlowView.hpp \
    externals/nodeeditor/include/nodes/internal/FlowViewStyle.hpp \
    externals/nodeeditor/include/nodes/internal/memory.hpp \
    externals/nodeeditor/include/nodes/internal/Node.hpp \
    externals/nodeeditor/include/nodes/internal/NodeData.hpp \
    externals/nodeeditor/include/nodes/internal/NodeDataModel.hpp \
    externals/nodeeditor/include/nodes/internal/NodeGeometry.hpp \
    externals/nodeeditor/include/nodes/internal/NodeGraphicsObject.hpp \
    externals/nodeeditor/include/nodes/internal/NodePainterDelegate.hpp \
    externals/nodeeditor/include/nodes/internal/NodeState.hpp \
    externals/nodeeditor/include/nodes/internal/NodeStyle.hpp \
    externals/nodeeditor/include/nodes/internal/OperatingSystem.hpp \
    externals/nodeeditor/include/nodes/internal/PortType.hpp \
    externals/nodeeditor/include/nodes/internal/QStringStdHash.hpp \
    externals/nodeeditor/include/nodes/internal/QUuidStdHash.hpp \
    externals/nodeeditor/include/nodes/internal/Serializable.hpp \
    externals/nodeeditor/include/nodes/internal/Style.hpp \
    externals/nodeeditor/include/nodes/internal/TypeConverter.hpp \
    externals/nodeeditor/include/nodes/Connection \
    externals/nodeeditor/include/nodes/ConnectionStyle \
    externals/nodeeditor/include/nodes/DataModelRegistry \
    externals/nodeeditor/include/nodes/FlowScene \
    externals/nodeeditor/include/nodes/FlowView \
    externals/nodeeditor/include/nodes/FlowViewStyle \
    externals/nodeeditor/include/nodes/Node \
    externals/nodeeditor/include/nodes/NodeData \
    externals/nodeeditor/include/nodes/NodeDataModel \
    externals/nodeeditor/include/nodes/NodeGeometry \
    externals/nodeeditor/include/nodes/NodePainterDelegate \
    externals/nodeeditor/include/nodes/NodeState \
    externals/nodeeditor/include/nodes/NodeStyle \
    externals/nodeeditor/include/nodes/TypeConverter \
    externals/nodeeditor/src/ConnectionBlurEffect.hpp \
    externals/nodeeditor/src/ConnectionPainter.hpp \
    externals/nodeeditor/src/NodeConnectionInteraction.hpp \
    externals/nodeeditor/src/NodePainter.hpp \
    externals/nodeeditor/src/Properties.hpp \
    externals/nodeeditor/src/StyleCollection.hpp \
    src/modulemanager.h \
    src/moduleexceptions.h \
    src/moduledata/spectrumdata.h \
    src/moduledata/booleandata.h \
    src/moduledata/integerdata.h \
    src/moduledata/floatdata.h \
    src/modulegraph.h \
    src/moduledata/moduledata.h \
    src/moduledata/spectrummoduledata.h \
    src/moduledata/booleanmoduledata.h \
    src/moduledata/floatmoduledata.h \
    src/moduledata/integermoduledata.h \
    src/nodedockwidget.h \
    src/dbmanager.h \
    src/moduledata/choicemoduledata.h \
    src/moduledata/choicedata.h

FORMS += \
        src/mainwindow.ui \
    src/dbsearch.ui

RESOURCES += \
    resources/resources.qrc

#LIBS += -lnodesdebug
#LIBS += -LC:\Users\valen\Documents\GitHub\AcousticNode\externals\nodeeditor\Mingw64bit\lib

INCLUDEPATH += .\externals\nodeeditor\include\nodes
INCLUDEPATH += .\externals\nodeeditor\include\nodes\internal

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    modules/* \
