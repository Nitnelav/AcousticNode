#include "dbpathdialog.h"


DbPathDialog::DbPathDialog(DbManager* mgr):
    dbManager_(mgr)
{
    setSizeGripEnabled(true);
    setModal(true);
    setBaseSize(700, 400);
    resize(700, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    QLabel* title = new QLabel("List of Databases (SQLite files)");
    mainLayout->addWidget(title);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    QWidget* scrollWidget = new QWidget();
    scrollLayout_ = new QVBoxLayout();

    QSettings settings("config.ini", QSettings::IniFormat);
    int size = settings.beginReadArray("databases");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        QString path = settings.value("path").toString();
        addPathWidget(name, path);
    }
    settings.endArray();

    scrollLayout_->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

    scrollWidget->setLayout(scrollLayout_);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    QDialogButtonBox* dlgBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QPushButton* addDB = dlgBtn->addButton("Add a new DB", QDialogButtonBox::ActionRole);
    mainLayout->addWidget(dlgBtn);

    setLayout(mainLayout);

    connect(addDB, &QPushButton::clicked, [=](){
        addPathWidget();
    });
    connect(dlgBtn, &QDialogButtonBox::accepted, [=]() {
        for (QString db: dbManager_->dbList()) {
            dbManager_->removeDb(db);
        }
        QSettings settings("config.ini", QSettings::IniFormat);
        settings.remove("databases");
        settings.beginWriteArray("databases");
        int index = 0;
        for (DbPath* dbPath: paths_) {
            settings.setArrayIndex(index);
            QString name = dbPath->name->text();
            QString path = dbPath->path->text();
            settings.setValue("name", name);
            settings.setValue("path", path);
            index++;
            if (!QFile::exists(path)) {
                continue;
            }
            dbManager_->addDb(path, name);
        }
        settings.endArray();
        close();
    });
    connect(dlgBtn, &QDialogButtonBox::rejected, [=]() {
        close();
    });
}

void DbPathDialog::addPathWidget(QString name, QString path)
{
    QWidget* pathWidget = new QWidget();
    QHBoxLayout* pathLayout = new QHBoxLayout();

    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setFixedWidth(150);
    nameEdit->setPlaceholderText("Name");
    if (name.isEmpty()) {
        name = "New DB";
    }
    nameEdit->setText(name);
    pathLayout->addWidget(nameEdit);

    QLineEdit* pathEdit = new QLineEdit();
    pathEdit->setPlaceholderText("Path");
    pathEdit->setText(path);
    pathLayout->addWidget(pathEdit);

    QToolButton* browseBtn = new QToolButton();
    browseBtn->setText("...");
    pathLayout->addWidget(browseBtn);

    QToolButton* deleteBtn = new QToolButton();
    deleteBtn->setIcon(QIcon(":/icons/x.svg"));
    pathLayout->addWidget(deleteBtn);

    pathWidget->setLayout(pathLayout);
    scrollLayout_->insertWidget(paths_.size(), pathWidget);
    DbPath* dbPath = new DbPath();
    dbPath->name = nameEdit;
    dbPath->path = pathEdit;
    paths_.append(dbPath);

    connect(browseBtn, &QToolButton::clicked, [=](){
        QString path = QFileDialog::getOpenFileName(this, "Select a Database file");
        pathEdit->setText(path);
    });
    connect(deleteBtn, &QToolButton::clicked, [=](){
        paths_.removeOne(dbPath);
        delete dbPath;
        pathWidget->deleteLater();
    });
}
