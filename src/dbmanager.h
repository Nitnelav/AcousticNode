#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQueryModel>

#include "ui_dbsearch.h"

class DbManager : public QObject
{
    Q_OBJECT

private:
    QString currentDbName_;
    QDialog* searchDialog_;
    Ui::DbSearchDialog* searchDialogUi_;

public:
    DbManager();
    ~DbManager();

    QStringList dbList() const;
    void addDb(const QString &filePath, const QString &name);
    void removeDb(const QString &name);

    QDialog* getSearchDialog();

public slots:
    void changeDb(const QString& dbName);
    void changeTable(const QString &tableName);
    void changeFilter(const QString &filter);
};

#endif // DBMANAGER_H
