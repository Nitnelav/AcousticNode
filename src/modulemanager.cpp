#include "modulemanager.h"

#include <QDebug>

ModuleManager::ModuleManager(QJSEngine *js, DbManager* db):
    js_(js),
    db_(db)
{

}

void ModuleManager::moduleValidator(QDir &dir)
{
    QFileInfo moduleFile(dir.filePath("module.mjs"));

    if (!moduleFile.exists()) {
        throw ModuleError(dir.path().toLatin1());
    }
    QString path = moduleFile.absoluteFilePath();
    QJSValue module = js_->importModule(path);
    if (module.isError()) {
        qDebug()
                << "Uncaught exception at line"
                << module.property("lineNumber").toInt()
                << ":" << module.toString();
        throw ScriptLoadError(path.toLatin1());
    }
    if (!module.hasProperty("name") || !module.property("name").isString()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "name");
    }
    if (!module.hasProperty("caption") || !module.property("caption").isString()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "caption");
    }
    if (!module.hasProperty("inputs") || !module.property("inputs").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "inputs");
    }
    if (!module.hasProperty("parameters") || !module.property("parameters").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "parameters");
    }
    if (!module.hasProperty("outputs") || !module.property("outputs").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "outputs");
    }
    if (!module.hasProperty("calculate") || !module.property("calculate").isCallable()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "calculate");
    }
}

std::shared_ptr<DataModelRegistry> ModuleManager::getModuleRegistry()
{
    auto ret = std::make_shared<DataModelRegistry>();
    QJSEngine* js = js_;
    DbManager* db = db_;

    QMapIterator<QString, QStringList> it(validModules_);
    while (it.hasNext()) {
        it.next();
        QString category = it.key();
        for (QString path: it.value()) {
            auto creator = [js, db, path]() {
                return std::make_unique<ScriptWrapperModel>(js, db, path);
            };
            ret->registerModel<ScriptWrapperModel>(std::move(creator), category);
        }
    }
    return ret;
}

void ModuleManager::loadModules()
{
    validModules_.clear();

    for (QDir modulesDir: modulesDirList_) {
        QStringList rootDirList = modulesDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        for (QString rootDir: rootDirList) {
            QDir root = QDir(modulesDir); // copy before cd
            root.cd(rootDir);
            QString category = root.dirName();
            QStringList modules = root.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
            for (QString module: modules) {
                QDir dir = QDir(root); // copy before cd
                dir.cd(module);
                try {
                    moduleValidator(dir);
                    validModules_[category].append(dir.filePath("module.mjs"));
                } catch (ModuleError& e){
                    QString message(e.what());
                    qDebug() << dir << ";" << message.toLatin1();
                }
            }
        }
    }
}

QList<QDir> ModuleManager::getModulesDirList() const
{
    return modulesDirList_;
}

void ModuleManager::addModulesDir(const QString &path)
{
    modulesDirList_.append(QDir(path));
}

void ModuleManager::clearModulesDir()
{
    modulesDirList_.clear();
}
