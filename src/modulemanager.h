#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QJSEngine>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QMap>
#include <QMapIterator>
#include <DataModelRegistry>

#include "dbmanager.h"
#include "scriptwrappermodel.h"
#include "moduleexceptions.h"

using QtNodes::DataModelRegistry;

class ModuleManager
{

private:
    QJSEngine* js_;
    DbManager* db_;
    QDir modulesDir_;
    QList<QDir> modulesDirList_;
    QMap<QString, QStringList> validModules_; // category => modulesList
//    QStringList validModules_;

    void moduleValidator(QDir &dir);

public:
    ModuleManager(QJSEngine *js, DbManager *db);

    std::shared_ptr<DataModelRegistry> getModuleRegistry();

    void loadModules();

    QList<QDir> getModulesDirList() const;
    void addModulesDir(const QString &path);
    void clearModulesDir();

};

#endif // MODULEMANAGER_H
