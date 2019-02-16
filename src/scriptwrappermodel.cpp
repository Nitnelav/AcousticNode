#include "scriptwrappermodel.h"

ScriptWrapperModel::ScriptWrapperModel(QJSEngine* engine, QString path):
    js_(engine),
    path_(path)
{
    module_ = js_->importModule(path_);
    if (module_.isError()) {
        throw ScriptLoadError(path.toLatin1());
    }
    if (!module_.hasProperty("name") || !module_.property("name").isString()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "name");
    }
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
            inputs_.append(nullptr);
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
            parameters_.append(nullptr);
        }
    }
    if (!module_.hasProperty("outputs") || !module_.property("outputs").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "outputs");
    } else {
        outputsDefinition_ = module_.property("outputs");
        numOutputs_ = outputsDefinition_.property("length").toInt();
        for (int index = 0; index < numOutputs_; ++index) {
            outputs_.append(nullptr);
        }
    }
    if (!module_.hasProperty("calculate") || !module_.property("calculate").isCallable()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "calculate");
    }

    setupParameters();

    if (numInputs_ == 0) {
        calculate();
    }
}

ScriptWrapperModel::~ScriptWrapperModel()
{

}

QString ScriptWrapperModel::caption() const
{
    return module_.property("caption").toString();
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
        return SpectrumData(SPECTRUM_TYPE_LW, description).type();
    }
    return NodeDataType { "Unknown", "Unknown" };
}

void ScriptWrapperModel::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex index)
{
    if (!data) {
        for (int index = 0; index < numOutputs_; ++index) {
            outputs_[index].reset();
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
        inputs_[index] = inputData;
        QJSValue array = js_->newArray(8);
        for (int i = 0; i < 8; ++i) {
            array.setProperty(i, inputData->getValue(i));
        }
        inputArgs_.setProperty(index, array);
    } else {
        return;
    }

    for (int i = 0; i < numInputs_; i++) {
        if (!inputs_[i]) {
            validationState_ = NodeValidationState::Warning;
            validationMessage_ = QString("Missing or invalid inputs");
            return;
        }
    }
    validationState_ = NodeValidationState::Valid;
    validationMessage_ = QString("");

    calculate();
}

void ScriptWrapperModel::setupParameters()
{
    for (int index = 0; index < numParameters_; ++index) {
        QJSValue element = parametersDefinition_.property(index);
        if (element.isError() || !element.hasProperty("type"))
            throw;
        QString type = element.property("type").toString();
        QString description = element.property("description").toString();
        if (type == "Lw") {
            std::shared_ptr<SpectrumData> spectrumData(new SpectrumData(SPECTRUM_TYPE_LW, description));
            parameters_[index] = spectrumData;
            connect(spectrumData.get(), &SpectrumData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
            QJSValue array = js_->newArray(8);
            for (int i = 0; i < 8; ++i) {
                array.setProperty(i, spectrumData->getValue(i));
            }
            parameterArgs_.setProperty(index, array);
        }
    }
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

    for (int i = 0; i < numOutputs_; i++) {
        QString type = outputsDefinition_.property(i).property("type").toString();
        QString description = outputsDefinition_.property(i).property("description").toString();
        if (type == "Lw") {
            if (!result.property(i).isArray()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %s is not an array (Lw Spectrum)");
                return;
            }
            if (result.property(i).property("length").toInt() != 8) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %s must be an array of size 8 (Lw Spectrum)");
                return;
            }
            std::shared_ptr<SpectrumData> spectrumData(new SpectrumData(SPECTRUM_TYPE_LW, description));
            for (int freq = 0; freq < 8; ++freq) {
                spectrumData->setValue(freq, result.property(i).property(freq).toNumber());
            }
            outputs_[i] = spectrumData;
            Q_EMIT dataUpdated(i);
        }
    }
}

std::shared_ptr<QtNodes::NodeData> ScriptWrapperModel::outData(QtNodes::PortIndex index)
{
    return outputs_.at(index);
}

QtNodes::NodeValidationState ScriptWrapperModel::validationState() const
{
    return validationState_;
}

QString ScriptWrapperModel::validationMessage() const
{
    return validationMessage_;
}

std::shared_ptr<QtNodes::NodeData> ScriptWrapperModel::getInputData(int index) const
{
    return inputs_[index];
}

std::shared_ptr<QtNodes::NodeData> ScriptWrapperModel::getParameterData(int index) const
{
    return parameters_[index];
}

std::shared_ptr<QtNodes::NodeData> ScriptWrapperModel::getOutputData(int index) const
{
    return outputs_[index];
}

void ScriptWrapperModel::parameterChanged()
{
    for (int index = 0; index < numParameters_; ++index) {
        QJSValue element = parametersDefinition_.property(index);
        QString type = element.property("type").toString();
        QString description = element.property("description").toString();
        auto spectrumData = std::dynamic_pointer_cast<SpectrumData>(parameters_[index]);
        if (type == "Lw") {
            for (int freq = 0; freq < 8; ++freq) {
                parameterArgs_.property(index).setProperty(freq, spectrumData->getValue(freq));
            }
        }
    }
    calculate();
}
