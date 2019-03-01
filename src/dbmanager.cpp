#include "dbmanager.h"

#include <QDebug>

DbManager::DbManager() : QObject(nullptr),
    searchDialog_(new QDialog),
    searchDialogUi_(new Ui::DbSearchDialog)
{
    searchDialogUi_->setupUi(searchDialog_);
    searchDialogUi_->tableView->resizeColumnsToContents();

    connect(searchDialogUi_->dbListCombo, &QComboBox::currentTextChanged, this, &DbManager::changeDb);
    connect(searchDialogUi_->dbTableCombo, &QComboBox::currentTextChanged, this, &DbManager::changeTable);
    connect(searchDialogUi_->searchFilter, &QLineEdit::textChanged, this, &DbManager::changeFilter);
}

DbManager::~DbManager()
{
    delete searchDialogUi_;
    delete searchDialog_;
}

QStringList DbManager::dbList() const
{
    return QSqlDatabase::connectionNames();
}

void DbManager::addDb(const QString &filePath, const QString &name)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(filePath);

    if (!db.open()) {
        QSqlDatabase::removeDatabase(name);
        return;
    }
    searchDialogUi_->dbListCombo->clear();
    foreach (auto dbName, dbList()) {
        searchDialogUi_->dbListCombo->addItem(dbName);
    }

    changeDb(name);
}

void DbManager::removeDb(const QString &name)
{
    QSqlDatabase::removeDatabase(name);

    if (currentDbName_ == name) {
        changeDb(QSqlDatabase::connectionNames().at(0));
    }
    searchDialogUi_->dbListCombo->clear();
    foreach (auto dbName, dbList()) {
        searchDialogUi_->dbListCombo->addItem(dbName);
    }
}

QDialog *DbManager::getSearchDialog()
{
    return searchDialog_;
}

void DbManager::changeDb(const QString &dbName)
{
    currentDbName_ = dbName;
    QSqlDatabase db = QSqlDatabase::database(currentDbName_);
    searchDialogUi_->dbTableCombo->clear();
    foreach (auto table, db.tables()) {
        QSqlRecord rec = db.record(table);
        // val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment
        if (!rec.contains("name") || !rec.contains("val63Hz") || !rec.contains("val125Hz") ||
            !rec.contains("val250Hz") || !rec.contains("val500Hz") || !rec.contains("val1kHz") ||
            !rec.contains("val2kHz") || !rec.contains("val4kHz") || !rec.contains("val8kHz") ||
            !rec.contains("comment")
            )
        {
            continue;
        }
        searchDialogUi_->dbTableCombo->addItem(table);
    }
    changeTable(searchDialogUi_->dbTableCombo->currentText());
}

void DbManager::changeTable(const QString &tableName)
{
    QSqlDatabase db = QSqlDatabase::database(currentDbName_);
    searchDialogUi_->tableView->model()->deleteLater();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString query = QString("SELECT name, val63Hz as '63Hz', val125Hz as '125Hz', val250Hz as '250Hz', val500Hz as '500Hz', val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment FROM %1");
    model->setQuery(query.arg(tableName), db);
    searchDialogUi_->tableView->setModel(model);
    searchDialogUi_->tableView->resizeColumnsToContents();
}

void DbManager::changeFilter(const QString &filter)
{
    QSqlDatabase db = QSqlDatabase::database(currentDbName_);
    searchDialogUi_->tableView->model()->deleteLater();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString query;

    if (!filter.isEmpty()) {
        query = QString(
            R"(
                SELECT name, val63Hz as '63Hz', val125Hz as '125Hz', val250Hz as '250Hz', val500Hz as '500Hz', val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment
                FROM %1
                WHERE name LIKE '%%2%'
            )").arg(searchDialogUi_->dbTableCombo->currentText()).arg(filter);
    } else {
        query = QString("SELECT name, val63Hz as '63Hz', val125Hz as '125Hz', val250Hz as '250Hz', val500Hz as '500Hz', val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment FROM %1")
                .arg(searchDialogUi_->dbTableCombo->currentText());
    }
    model->setQuery(query, db);
    searchDialogUi_->tableView->setModel(model);
    searchDialogUi_->tableView->resizeColumnsToContents();
}
