#ifndef DBPATHDIALOG_H
#define DBPATHDIALOG_H

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

#include "dbmanager.h"

struct DbPath {
    QLineEdit* name;
    QLineEdit* path;
};

class DbPathDialog : public QDialog
{
    Q_OBJECT

private:
    DbManager* dbManager_;
    QVBoxLayout* scrollLayout_;
    QList<DbPath*> paths_;

    void addPathWidget(QString name = "", QString path = "");

public:
    DbPathDialog(DbManager *mgr);
};

#endif // DBPATHDIALOG_H
