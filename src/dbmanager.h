#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <memory>

#include <QObject>
#include <QString>
#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QModelIndex>
#include <QMenu>

#include "ui_dbsearch.h"
#include "moduledata/spectrummoduledata.h"

class DbManager : public QObject
{
    Q_OBJECT

private:
    QString currentDbName_;
    QString currentTable_;
    QDialog* searchDialog_;
    Ui::DbSearchDialog* searchDialogUi_;

public:
    DbManager();
    ~DbManager();

    QStringList dbList() const;
    QStringList tableList() const;
    void addDb(const QString &filePath, const QString &name);
    void removeDb(const QString &name);

    QDialog* getSearchDialog();
    QDialog* openSearchDialog(bool buttons = false);
    void setFromDb(std::shared_ptr<SpectrumModuleData> module);

    QString getCurrentDbName() const;
    QString getCurrentTable() const;

    void updateElement(QString db, QString table, QString id, QString name, QStringList spectrum, QString comment);
    void addElement(QString db, QString table, QString name, QStringList spectrum, QString comment);

public slots:
    void changeDb(const QString& dbName);
    void changeTable(const QString &tableName);
    void changeFilter(const QString &filter);
    void tableContextMenu(const QPoint& pos);
    void updateData();
};

#endif // DBMANAGER_H
