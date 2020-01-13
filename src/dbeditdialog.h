#ifndef DBEDITDIALOG_H
#define DBEDITDIALOG_H

#include <QVBoxLayout>
#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QToolButton>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSqlRecord>
#include <QSqlField>
#include <QStringList>

class DbManager;

class DbEditDialog : public QDialog
{
    Q_OBJECT

private:
    DbManager* dbManager_;

    QString recordId_;
    QStringList freqNames_;

    QComboBox* tableNameCombo_;
    QComboBox* dbNameCombo_;

    QLineEdit* nameWidget_;
    QTableWidget* spectrumWidget_;
    QPlainTextEdit* commentWidget_;

public:
    DbEditDialog(DbManager* mgr);
    DbEditDialog(DbManager* mgr, QSqlRecord rec);
};

#endif // DBEDITDIALOG_H
