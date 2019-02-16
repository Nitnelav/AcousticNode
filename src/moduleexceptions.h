#ifndef MODULEEXCEPTIONS_H
#define MODULEEXCEPTIONS_H

#include <exception>

class ModuleError : public std::exception
{
private:
    const char * filePath_;
public:
    ModuleError(const char* filePath) : filePath_(filePath) {}
    const char* what() const noexcept {
        return "Module error";
    }
    const char *filePath() const {
        return filePath_;
    }
};

class ScriptLoadError : public ModuleError
{
public:
    ScriptLoadError(const char* filePath) : ModuleError(filePath) {}
    const char* what() const noexcept {
        return "Module script could not be loaded";
    }
};

class MissingOrBadPropertyError : public ModuleError
{
private:
    const char * property_;
public:
    MissingOrBadPropertyError(const char* filePath, const char* property):
        ModuleError(filePath), property_(property) {}
    const char* what() const noexcept {
        return "Module script is missing a property or property is the wrong type";
    }
    const char *property() const {
        return property_;
    }
};
#endif // MODULEEXCEPTIONS_H
