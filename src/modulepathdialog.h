#ifndef MODULEPATHDIALOG_H
#define MODULEPATHDIALOG_H

#include <QVBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QSpacerItem>
#include <QSettings>

#include "modulemanager.h"

class ModulePathDialog : public QDialog
{
    Q_OBJECT

private:
    ModuleManager* moduleManager_;
    QVBoxLayout* scrollLayout_;
    QList<QLineEdit*> paths_;

    void addPathWidget(QString path = "");

public:
    ModulePathDialog(ModuleManager* moduleManager);
};

#endif // MODULEPATHDIALOG_H
