#include "moduledata.h"

ModuleData::ModuleData(const QJSValue& element) : QObject(nullptr)
{    
    id_ = element.property("id").toString();
    type_ = element.property("type").toString();
    typeName_ = element.property("typeName").toString();
    description_ = element.property("description").toString();
    if (element.hasProperty("required")) {
        required_ = element.property("required").toBool();
    } else {
        required_ = true;
    }
}

QString ModuleData::caption() const
{
    if (required_) {
        return typeName_;
    } else {
        return typeName_ + '*';
    }
}
