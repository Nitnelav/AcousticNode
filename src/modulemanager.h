#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <QJSEngine>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <DataModelRegistry>

#include "scriptwrappermodel.h"
#include "moduleexceptions.h"

using QtNodes::DataModelRegistry;

class ModuleManager
{

private:
    QJSEngine* js_;
    QDir modulesDir_;
    QStringList validModules_;

    void moduleValidator(QDir &dir);

public:
    ModuleManager(QJSEngine *js);

    std::shared_ptr<DataModelRegistry> getModuleRegistry();

    void loadModules();

    QDir getModulesDir() const;
    void setModulesDir(const QString &path);
};

#endif // MODULEMANAGER_H
