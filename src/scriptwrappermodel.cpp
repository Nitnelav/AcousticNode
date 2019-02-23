#include "scriptwrappermodel.h"

ScriptWrapperModel::ScriptWrapperModel(QJSEngine* engine, QString path):
    js_(engine),
    path_(path)
{
    moduleGraph_ = std::make_shared<ModuleGraph>();
    module_ = js_->importModule(path_);

    if (module_.isError()) {
        throw ScriptLoadError(path.toLatin1());
    }
    if (!module_.hasProperty("name") || !module_.property("name").isString()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "name");
    }
    caption_ = module_.property("name").toString();
    description_ = "";
    descriptionWidget_ = std::make_shared<QLabel>(description_);

    if (!module_.hasProperty("caption") || !module_.property("caption").isString()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "caption");
    }
    if (!module_.hasProperty("inputs") || !module_.property("inputs").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "inputs");
    } else {
        inputsDefinition_ = module_.property("inputs");
        numInputs_ = inputsDefinition_.property("length").toInt();
        for (int index = 0; index < numInputs_; ++index) {
            QJSValue element = inputsDefinition_.property(index);
            QString type = element.property("type").toString();
            QString description = element.property("description").toString();
            if (type == "Lw") {
                auto spectrumData = std::make_shared<SpectrumModuleData>(description);
                inputs_.append(spectrumData);
                moduleGraph_->appendSpectrumData(spectrumData);
            } else {
                inputs_.append(nullptr);
            }
        }
        inputArgs_ = js_->newArray(numInputs_);
        if (numInputs_ > 0) {
            validationState_ = NodeValidationState::Warning;
            validationMessage_ = QString("Missing inputs");
        }
    }
    if (!module_.hasProperty("parameters") || !module_.property("parameters").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "parameters");
    } else {
        parametersDefinition_ = module_.property("parameters");
        numParameters_= parametersDefinition_.property("length").toInt();
        parameterArgs_ = js_->newArray(numParameters_);
        for (int index = 0; index < numParameters_; ++index) {
            QJSValue element = parametersDefinition_.property(index);
            QString type = element.property("type").toString();
            QString description = element.property("description").toString();
            if (type == "Lw") {
                auto spectrumData = std::make_shared<SpectrumModuleData>(description);
                auto spectrumNodeData = std::make_shared<SpectrumData>(SPECTRUM_TYPE_LW);
                spectrumData->setNodeData(spectrumNodeData);
                connect(spectrumData.get(), &SpectrumModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
                parameters_.append(spectrumData);
                moduleGraph_->appendSpectrumData(spectrumData);
                QJSValue array = js_->newArray(8);
                for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                    array.setProperty(freq, spectrumData->getValue(freq));
                }
                parameterArgs_.setProperty(index, array);
            } else {
                parameters_.append(nullptr);
            }
        }
    }
    if (!module_.hasProperty("outputs") || !module_.property("outputs").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "outputs");
    } else {
        outputsDefinition_ = module_.property("outputs");
        numOutputs_ = outputsDefinition_.property("length").toInt();
        for (int index = 0; index < numOutputs_; ++index) {
            QJSValue element = outputsDefinition_.property(index);
            QString type = element.property("type").toString();
            QString description = element.property("description").toString();
            if (type == "Lw") {
                auto spectrumData = std::make_shared<SpectrumModuleData>(description);
                outputs_.append(spectrumData);
                moduleGraph_->appendSpectrumData(spectrumData);
            } else {
                outputs_.append(nullptr);
            }
        }
    }
    if (!module_.hasProperty("calculate") || !module_.property("calculate").isCallable()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "calculate");
    }

    if (numInputs_ == 0) {
        calculate();
    }
}

//ScriptWrapperModel::~ScriptWrapperModel()
//{
//    delete moduleGraph_;
//}

QString ScriptWrapperModel::caption() const
{
    return caption_;
}

void ScriptWrapperModel::setDescription(const QString &description)
{
    description_ = description;
    descriptionWidget_->setText(description_);
}

QString ScriptWrapperModel::name() const
{
    return module_.property("name").toString();
}

QString ScriptWrapperModel::portCaption(QtNodes::PortType portType, QtNodes::PortIndex index) const
{
    QString arrayProperty;
    if (portType == PortType::In) {
        arrayProperty = "inputs";
    }
    else if (portType == PortType::Out) {
        arrayProperty = "outputs";
    }
    else {
        return "";
    }
    QJSValue array = module_.property(arrayProperty);
    int arrayLength = array.property("length").toInt();
    if (index >= arrayLength)
        throw;
    QJSValue element = array.property(index);
    if (element.isError() || !element.hasProperty("type"))
        throw;
    return element.property("type").toString();
}

unsigned int ScriptWrapperModel::nPorts(QtNodes::PortType portType) const
{
    QString arrayProperty;
    if (portType == PortType::In) {
        arrayProperty = "inputs";
    }
    else if (portType == PortType::Out) {
        arrayProperty = "outputs";
    }
    else {
        return 0;
    }
    QJSValue array = module_.property(arrayProperty);
    return array.property("length").toInt();
}

QtNodes::NodeDataType ScriptWrapperModel::dataType(QtNodes::PortType portType, QtNodes::PortIndex index) const
{
    QString arrayProperty;
    if (portType == PortType::In) {
        arrayProperty = "inputs";
    }
    else if (portType == PortType::Out) {
        arrayProperty = "outputs";
    }
    else {
        return NodeDataType();
    }
    QJSValue array = module_.property(arrayProperty);
    int arrayLength = array.property("length").toInt();
    if (index >= arrayLength)
        throw;
    QJSValue element = array.property(index);
    if (element.isError() || !element.hasProperty("type"))
        throw;
    QString type = element.property("type").toString();
    QString description = element.property("description").toString();
    if (type == "Lw") {
//        return SpectrumData(SPECTRUM_TYPE_LW, description).type();
        return NodeDataType { "Lw", "Lw"};
    }
    return NodeDataType { "Unknown", "Unknown" };
}

void ScriptWrapperModel::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex index)
{
    if (!data) {
        for (int index = 0; index < numOutputs_; ++index) {
            outputs_[index]->setNodeData(nullptr);
            inputs_[index]->setNodeData(nullptr);
            Q_EMIT dataUpdated(index);
        }
        validationState_ = NodeValidationState::Warning;
        validationMessage_ = QString("Missing or invalid inputs");
        return;
    }

    QJSValue element = inputsDefinition_.property(index);
    QString type = element.property("type").toString();

    if (type == "Lw") {
        auto inputData = std::dynamic_pointer_cast<SpectrumData>(data);
        auto input = std::dynamic_pointer_cast<SpectrumModuleData>(inputs_[index]);
        input->setNodeData(inputData);
        QJSValue array = js_->newArray(8);
        for (int i = 0; i < 8; ++i) {
            array.setProperty(i, inputData->getValue(i));
        }
        inputArgs_.setProperty(index, array);
    } else {
        return;
    }

    for (int i = 0; i < numInputs_; i++) {
        if (!inputs_[i]->getNodeData()) {
            validationState_ = NodeValidationState::Warning;
            validationMessage_ = QString("Missing or invalid inputs");
            return;
        }
    }
    validationState_ = NodeValidationState::Valid;
    validationMessage_ = QString("");

    calculate();
}

void ScriptWrapperModel::calculate()
{

    QJSValue function = module_.property("calculate");
    QJSValueList args;
    args.append(inputArgs_);
    args.append(parameterArgs_);
    QJSValue result = function.call(args);

    if (result.isError()) {
        validationState_ = NodeValidationState::Error;
        validationMessage_ = QString("Script Error");
        return;
    }
    if (!result.isArray()) {
        validationState_ = NodeValidationState::Error;
        validationMessage_ = QString("Script output is not an array");
        return;
    }
    if (result.property("length").toInt() != numOutputs_) {
        validationState_ = NodeValidationState::Error;
        validationMessage_ = QString("Script output array has the wrong size");
        return;
    }

    for (int index = 0; index < numOutputs_; index++) {
        QString type = outputsDefinition_.property(index).property("type").toString();
        QString description = outputsDefinition_.property(index).property("description").toString();
        if (type == "Lw") {
            if (!result.property(index).isArray()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %s is not an array (Lw Spectrum)");
                return;
            }
            if (result.property(index).property("length").toInt() != 8) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %s must be an array of size 8 (Lw Spectrum)");
                return;
            }
            std::shared_ptr<SpectrumData> spectrumData;
            auto ouput = std::dynamic_pointer_cast<SpectrumModuleData>(outputs_[index]);
            if (ouput->getNodeData()) {
                spectrumData = std::static_pointer_cast<SpectrumData>(ouput->getNodeData());
            } else {
                spectrumData = std::make_shared<SpectrumData>(SPECTRUM_TYPE_LW);
                ouput->setNodeData(spectrumData);
            }
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                ouput->setValue(freq, result.property(index).property(freq).toNumber());
            }
            Q_EMIT dataUpdated(index);
        }
    }
    js_->collectGarbage();
}

std::shared_ptr<QtNodes::NodeData> ScriptWrapperModel::outData(QtNodes::PortIndex index)
{
    return outputs_.at(index)->getNodeData();
}

QtNodes::NodeValidationState ScriptWrapperModel::validationState() const
{
    return validationState_;
}

QString ScriptWrapperModel::validationMessage() const
{
    return validationMessage_;
}

QWidget *ScriptWrapperModel::embeddedWidget()
{
    return nullptr;
}

std::shared_ptr<ModuleData> ScriptWrapperModel::getInputData(int index) const
{
    return inputs_[index];
}

std::shared_ptr<ModuleData> ScriptWrapperModel::getParameterData(int index) const
{
    return parameters_[index];
}

std::shared_ptr<ModuleData> ScriptWrapperModel::getOutputData(int index) const
{
    return outputs_[index];
}

void ScriptWrapperModel::parameterChanged()
{
    for (int index = 0; index < numParameters_; ++index) {
        QJSValue element = parametersDefinition_.property(index);
        QString type = element.property("type").toString();
        QString description = element.property("description").toString();
        if (type == "Lw") {
            auto spectrumData = std::static_pointer_cast<SpectrumModuleData>(parameters_[index]);
            for (int freq = 0; freq < 8; ++freq) {
                parameterArgs_.property(index).setProperty(freq, spectrumData->getValue(freq));
            }
        }
    }
    for (int i = 0; i < numInputs_; i++) {
        if (!inputs_[i]->getNodeData()) {
            return;
        }
    }
    calculate();
}
