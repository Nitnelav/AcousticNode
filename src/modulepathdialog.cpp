#include "modulepathdialog.h"

ModulePathDialog::ModulePathDialog(ModuleManager* moduleManager):
    moduleManager_(moduleManager)
{
    setSizeGripEnabled(true);
    setModal(true);
    setBaseSize(500, 400);
    resize(500, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    QLabel* title = new QLabel("List of Databases (SQLite files)");
    mainLayout->addWidget(title);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    QWidget* scrollWidget = new QWidget();
    scrollLayout_ = new QVBoxLayout();

    QSettings settings("config.ini", QSettings::IniFormat);
    int size = settings.beginReadArray("modules");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString dir = settings.value("dir").toString();
        addPathWidget(dir);
    }
    settings.endArray();

    scrollLayout_->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

    scrollWidget->setLayout(scrollLayout_);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    QDialogButtonBox* dlgBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    QPushButton* addDB = dlgBtn->addButton("Add a new Modules Dir", QDialogButtonBox::ActionRole);
    mainLayout->addWidget(dlgBtn);

    setLayout(mainLayout);

    connect(addDB, &QPushButton::clicked, [=](){
        addPathWidget();
    });
    connect(dlgBtn, &QDialogButtonBox::accepted, [=]() {
        moduleManager_->clearModulesDir();
        QSettings settings("config.ini", QSettings::IniFormat);
        settings.remove("modules");
        settings.beginWriteArray("modules");
        int index = 0;
        for (QLineEdit* edit: paths_) {
            settings.setArrayIndex(index);
            QString dir = edit->text();
            settings.setValue("dir", dir);
            index++;
            if (!QFile::exists(dir)) {
                continue;
            }
            moduleManager_->addModulesDir(dir);
        }
        settings.endArray();
        close();
    });
    connect(dlgBtn, &QDialogButtonBox::rejected, [=]() {
        close();
    });
}

void ModulePathDialog::addPathWidget(QString path)
{
    QWidget* pathWidget = new QWidget();
    QHBoxLayout* pathLayout = new QHBoxLayout();

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
    paths_.append(pathEdit);

    connect(browseBtn, &QToolButton::clicked, [=](){
        QString path = QFileDialog::getExistingDirectory(this, "Select a Modules Directory");
        pathEdit->setText(path);
    });
    connect(deleteBtn, &QToolButton::clicked, [=](){
        paths_.removeOne(pathEdit);
        pathWidget->deleteLater();
    });
}
