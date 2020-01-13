#include "dbmanager.h"

#include "dbeditdialog.h"
#include "dbpathdialog.h"

#include <QDebug>
#include <QSqlError>

DbManager::DbManager() : QObject(nullptr),
    searchDialog_(new QDialog),
    searchDialogUi_(new Ui::DbSearchDialog)
{
    searchDialogUi_->setupUi(searchDialog_);
    searchDialogUi_->tableView->resizeColumnsToContents();
    searchDialogUi_->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(searchDialogUi_->dbListCombo, &QComboBox::currentTextChanged, this, &DbManager::changeDb);
    connect(searchDialogUi_->dbTableCombo, &QComboBox::currentTextChanged, this, &DbManager::changeTable);
    connect(searchDialogUi_->searchFilter, &QLineEdit::textChanged, this, &DbManager::changeFilter);
    connect(searchDialogUi_->tableView, &QTableView::customContextMenuRequested, this, &DbManager::tableContextMenu);
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

QStringList DbManager::tableList() const
{
    QSqlDatabase db = QSqlDatabase::database(currentDbName_);
    return db.tables();
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
        if (QSqlDatabase::connectionNames().size() > 0) {
            changeDb(QSqlDatabase::connectionNames().at(0));
        } else {
            currentDbName_ = "";
        }
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

QDialog *DbManager::openSearchDialog(bool buttons)
{
    if (buttons) {
        searchDialogUi_->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    } else {
        searchDialogUi_->buttonBox->setStandardButtons(QDialogButtonBox::Close);
    }
    searchDialog_->open();
    return searchDialog_;
}

void DbManager::setFromDb(std::shared_ptr<SpectrumModuleData> module)
{
    searchDialogUi_->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(searchDialogUi_->buttonBox, &QDialogButtonBox::clicked, this, [=](QAbstractButton* button) {
        auto role = searchDialogUi_->buttonBox->buttonRole(button);
        switch (role) {
        case (QDialogButtonBox::ButtonRole::AcceptRole) :
        {
            int row = searchDialogUi_->tableView->selectionModel()->selectedRows()[0].row();
            auto sqlModel = static_cast<QSqlQueryModel*>(searchDialogUi_->tableView->model());
            QSqlRecord record = sqlModel->record(row);
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++) {
                double value = record.value(freq+1).toString().replace(",", ".").toDouble();
                module->setValue(freq, value);
            }
            break;
        }
        case (QDialogButtonBox::ButtonRole::ApplyRole) :
        {
            int row = searchDialogUi_->tableView->selectionModel()->selectedRows()[0].row();
            auto sqlModel = static_cast<QSqlQueryModel*>(searchDialogUi_->tableView->model());
            QSqlRecord record = sqlModel->record(row);
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++) {
                double value = record.value(freq+1).toString().replace(",", ".").toDouble();
                module->setValue(freq, value);
            }
            break;
        }
        default:
            break;
        }
    });
    searchDialog_->exec();
    disconnect(searchDialogUi_->buttonBox, &QDialogButtonBox::clicked, this, nullptr);
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
    currentTable_ = tableName;
    QSqlDatabase db = QSqlDatabase::database(currentDbName_);
    searchDialogUi_->tableView->model()->deleteLater();
    QSqlQueryModel* model = new QSqlQueryModel();
    QString query = QString("SELECT id, name, val63Hz as '63Hz', val125Hz as '125Hz', val250Hz as '250Hz', val500Hz as '500Hz', val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment FROM %1");
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
                SELECT id, name, val63Hz as '63Hz', val125Hz as '125Hz', val250Hz as '250Hz', val500Hz as '500Hz', val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment
                FROM %1
                WHERE name LIKE '%%2%'
            )").arg(searchDialogUi_->dbTableCombo->currentText()).arg(filter);
    } else {
        query = QString("SELECT id, name, val63Hz as '63Hz', val125Hz as '125Hz', val250Hz as '250Hz', val500Hz as '500Hz', val1kHz as '1kHz', val2kHz as '2kHz', val4kHz as '4kHz', val8kHz as '8kHz', comment FROM %1")
                .arg(searchDialogUi_->dbTableCombo->currentText());
    }
    model->setQuery(query, db);
    searchDialogUi_->tableView->setModel(model);
    searchDialogUi_->tableView->resizeColumnsToContents();
}

void DbManager::tableContextMenu(const QPoint &pos)
{
    int index = searchDialogUi_->tableView->rowAt(pos.y());
    if (index < 0) {
        return;
    }
    QSqlQueryModel* model = (QSqlQueryModel*) searchDialogUi_->tableView->model();
    QMenu* menu = new QMenu();
    QAction* action = menu->addAction("Edit...");
    connect(action, &QAction::triggered, [=]() {
        DbEditDialog* editDlg = new DbEditDialog(this, model->record(index));
        editDlg->open();
        connect(editDlg, &QDialog::finished, this, &DbManager::updateData);
    });

    menu->popup(searchDialogUi_->tableView->mapToGlobal(pos));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void DbManager::updateData()
{
    changeFilter(searchDialogUi_->searchFilter->text());
}

QString DbManager::getCurrentDbName() const
{
    return currentDbName_;
}

QString DbManager::getCurrentTable() const
{
    return currentTable_;
}

void DbManager::updateElement(QString db, QString table, QString id, QString name, QStringList spectrum, QString comment)
{
    if (!dbList().contains(db)) {
        return;
    }
    QSqlDatabase database = QSqlDatabase::database(currentDbName_);
    if (!database.tables().contains(table)) {
        return;
    }
    QString query = QString(R"(
                   UPDATE %1
                   SET name = '%2', val63Hz = %3, val125Hz = %4, val250Hz = %5, val500Hz = %6, val1kHz = %7, val2kHz = %8, val4kHz = %9, val8kHz = %10, comment = '%11'
                   WHERE id = %12
               )").arg(table);
    query = query.arg(name);
    for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++)
    {
        query = query.arg(spectrum[freq].replace(",", "."));
    }
    query = query.arg(comment);
    query = query.arg(id);

    QSqlQuery result = database.exec(query);
    if (!result.isActive()) {
        qDebug() << database.lastError();
        return;
    }
}

void DbManager::addElement(QString db, QString table, QString name, QStringList spectrum, QString comment)
{
    if (!dbList().contains(db)) {
        return;
    }
    QSqlDatabase database = QSqlDatabase::database(currentDbName_);
    if (!database.tables().contains(table)) {
        QString query = QString(R"(
                       CREATE TABLE %1 (
                       id       INTEGER       PRIMARY KEY,
                       name     VARCHAR (255),
                       val63Hz  DOUBLE,
                       val125Hz DOUBLE,
                       val250Hz DOUBLE,
                       val500Hz DOUBLE,
                       val1kHz  DOUBLE,
                       val2kHz  DOUBLE,
                       val4kHz  DOUBLE,
                       val8kHz  DOUBLE,
                       comment  VARCHAR (255)
                       );
                   )").arg(table);
        QSqlQuery result = database.exec(query);
        if (!result.isActive()) {
            qDebug() << database.lastError();
            return;
        }
    }
    QString query = QString(R"(
                   INSERT INTO %1 (name, val63Hz, val125Hz, val250Hz, val500Hz, val1kHz, val2kHz, val4kHz, val8kHz, comment)
                   VALUES ('%2', %3, %4, %5, %6, %7, %8, %9, %10, '%11')
               )").arg(table);
    query = query.arg(name);
    for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++)
    {
        query = query.arg(spectrum[freq].replace(",", "."));
    }
    query = query.arg(comment);

    QSqlQuery result = database.exec(query);
    if (!result.isActive()) {
        qDebug() << database.lastError();
        return;
    }
}
