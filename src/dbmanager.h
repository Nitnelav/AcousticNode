#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <memory>

#include <QObject>
#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QModelIndex>

#include "ui_dbsearch.h"
#include "moduledata/spectrummoduledata.h"

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
    QDialog* openSearchDialog(bool buttons = false);
    void setFromDb(std::shared_ptr<SpectrumModuleData> module);

public slots:
    void changeDb(const QString& dbName);
    void changeTable(const QString &tableName);
    void changeFilter(const QString &filter);
};

#endif // DBMANAGER_H
