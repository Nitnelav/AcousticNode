#include "dbeditdialog.h"

#include "dbmanager.h"

DbEditDialog::DbEditDialog(DbManager* mgr):
    dbManager_(mgr)
{
    freqNames_ << "63Hz" << "125Hz" << "250Hz" << "500Hz" << "1kHz" << "2kHz" << "4kHz" << "8kHz";

    setWindowTitle("Edit Element");
    setSizeGripEnabled(true);
    setModal(true);
    setBaseSize(700, 400);
    resize(700, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    QWidget* topWidget = new QWidget();
    QHBoxLayout* topLayout = new QHBoxLayout();
    topWidget->setLayout(topLayout);

    QLabel* dbNameLabel = new QLabel("Database");
    dbNameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    dbNameCombo_ = new QComboBox();
    for (QString dbName: dbManager_->dbList()) {
        dbNameCombo_->addItem(dbName);
    }
    dbNameCombo_->setCurrentText(dbManager_->getCurrentDbName());
    dbNameCombo_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QLabel* tableNameLabel = new QLabel("Table");
    tableNameLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    tableNameCombo_ = new QComboBox();
    for (QString tableName: dbManager_->tableList()) {
        tableNameCombo_->addItem(tableName);
    }
    tableNameCombo_->setCurrentText(dbManager_->getCurrentTable());
    tableNameCombo_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    tableNameCombo_->setEditable(true);

    topLayout->addWidget(dbNameLabel);
    topLayout->addWidget(dbNameCombo_);
    topLayout->addSpacing(20);
    topLayout->addWidget(tableNameLabel);
    topLayout->addWidget(tableNameCombo_);

    mainLayout->addWidget(topWidget);

    QGroupBox* formWidget = new QGroupBox();
    QFormLayout* form = new QFormLayout();
    formWidget->setLayout(form);
    form->setRowWrapPolicy(QFormLayout::WrapAllRows);

    nameWidget_ = new QLineEdit();

    form->addRow("name", nameWidget_);

    spectrumWidget_ = new QTableWidget(1, 8);
    spectrumWidget_->setHorizontalHeaderLabels(freqNames_);
    spectrumWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    spectrumWidget_->verticalHeader()->setStretchLastSection(true);
    spectrumWidget_->verticalHeader()->hide();
    spectrumWidget_->resizeColumnsToContents();
    spectrumWidget_->setMaximumHeight(70);
    spectrumWidget_->setRowCount(1);
    for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq)
    {
        spectrumWidget_->setItem(0, freq, new QTableWidgetItem());
    }

    form->addRow("Spectrum", spectrumWidget_);

    commentWidget_ = new QPlainTextEdit();

    form->addRow("comment", commentWidget_);

    mainLayout->addWidget(formWidget);

    QDialogButtonBox* dlgBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(dlgBtn);

    setLayout(mainLayout);

    connect(dlgBtn, &QDialogButtonBox::accepted, [=]() {
        QStringList spectrum;
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq)
        {
            spectrum << spectrumWidget_->item(0, freq)->text();
        }

        if (!recordId_.isEmpty()) {
            dbManager_->updateElement(dbNameCombo_->currentText(),
                                      tableNameCombo_->currentText(),
                                      recordId_,
                                      nameWidget_->text(),
                                      spectrum,
                                      commentWidget_->toPlainText());
        } else {
            dbManager_->addElement(dbNameCombo_->currentText(),
                                   tableNameCombo_->currentText(),
                                   nameWidget_->text(),
                                   spectrum,
                                   commentWidget_->toPlainText());
        }
        close();
    });
    connect(dlgBtn, &QDialogButtonBox::rejected, [=]() {
        close();
    });
}

DbEditDialog::DbEditDialog(DbManager *mgr, QSqlRecord rec):
    DbEditDialog(mgr)
{
    recordId_ = rec.value("id").toString();

    dbNameCombo_->setEnabled(false);
    tableNameCombo_->setEnabled(false);
    nameWidget_->setText(rec.value("name").toString());

    for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
        QString value = rec.value(freqNames_.at(freq)).toString();
        spectrumWidget_->item(0, freq)->setText(value);
    }

    commentWidget_->setPlainText(rec.field("comment").value().toString());
}


