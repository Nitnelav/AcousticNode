#include "scriptwrappermodel.h"

ScriptWrapperModel::ScriptWrapperModel(QJSEngine *engine, DbManager* db, QString path):
    js_(engine),
    db_(db),
    path_(path)
{
    dockWidget_ = new QWidget();
    moduleChart_ = new ModuleGraph();
    moduleChartView_ = new QChartView();

    module_ = js_->importModule(path_);
    QFileInfo fileInfo(path_);
    QDir dir(fileInfo.path());
    QStringList filters = {"*.png", "*.xpm", "*.jpg", "*.jpeg"};
    QFileInfoList pictureFiles = dir.entryInfoList(filters);

    if (!pictureFiles.empty()) {
        hasPicture_ = true;
        pixmap_ = QPixmap(pictureFiles.at(0).absoluteFilePath());
    }

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
    }
    if (!module_.hasProperty("parameters") || !module_.property("parameters").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "parameters");
    }
    if (!module_.hasProperty("outputs") || !module_.property("outputs").isArray()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "outputs");
    }
    if (!module_.hasProperty("calculate") || !module_.property("calculate").isCallable()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "calculate");
    }

    caption_ = module_.property("caption").toString();
    description_ = "";
    descriptionWidget_ = std::make_shared<QLabel>(description_);

    if (module_.hasProperty("description") && module_.property("description").isString()) {
        description_ = module_.property("description").toString();
    }

    inputArgs_ = js_->newObject();
    inputsDefinition_ = module_.property("inputs");
    numInputs_ = inputsDefinition_.property("length").toInt();
    bool atLeastOneInputRequired = false;
    for (int index = 0; index < numInputs_; ++index) {
        QJSValue element = inputsDefinition_.property(index);
        QString type = element.property("type").toString();
        if (type == "spectrum") {
            auto spectrumModuleData = std::make_shared<SpectrumModuleData>(element);
            inputs_.append(spectrumModuleData);
        } else if (type == "int") {
            inputs_.append(std::make_shared<IntegerModuleData>(element));
        } else if (type == "bool") {
            inputs_.append(std::make_shared<BooleanModuleData>(element));
        } else if (type == "float") {
            inputs_.append(std::make_shared<FloatModuleData>(element));
        } else {
            inputs_.append(nullptr);
        }
        if (inputs_[index]->required()) {
            atLeastOneInputRequired = true;
        }
    }

    parameterArgs_ = js_->newObject();
    parametersDefinition_ = module_.property("parameters");
    numParameters_= parametersDefinition_.property("length").toInt();
    for (int index = 0; index < numParameters_; ++index) {
        QJSValue element = parametersDefinition_.property(index);
        QString type = element.property("type").toString();
        if (type == "spectrum") {
            auto spectrumModuleData = std::make_shared<SpectrumModuleData>(element);
            auto spectrumData = std::make_shared<SpectrumData>();
            spectrumModuleData->setNodeData(spectrumData);
            if (element.hasProperty("default") && element.property("default").isArray()) {
                QJSValue defaultSpectrum = element.property("default");
                if (defaultSpectrum.property("length").toInt() >= 8) {
                    for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                        spectrumModuleData->setValue(freq, defaultSpectrum.property(freq).toNumber());
                    }
                }
            }
            connect(spectrumModuleData.get(), &SpectrumModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
            parameters_.append(spectrumModuleData);
            moduleChart_->appendSpectrumData(spectrumModuleData);
            QJSValue array = js_->newArray(8);
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                array.setProperty(freq, spectrumModuleData->getValue(freq));
            }
            parameterArgs_.setProperty(spectrumModuleData->id(), array);
        } else if (type == "int") {
            auto integerModuleData = std::make_shared<IntegerModuleData>(element);
            auto integerData = std::make_shared<IntegerData>();
            integerModuleData->setNodeData(integerData);
            if (element.hasProperty("default") && element.property("default").isNumber()) {
                integerModuleData->setValue(element.property("default").toInt());
            }
            connect(integerModuleData.get(), &IntegerModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
            parameters_.append(integerModuleData);
            parameterArgs_.setProperty(integerModuleData->id(), integerData->number());
        } else if (type == "bool") {
            auto booleanModuleData = std::make_shared<BooleanModuleData>(element);
            auto booleanData = std::make_shared<BooleanData>();
            booleanModuleData->setNodeData(booleanData);
            if (element.hasProperty("default") && element.property("default").isBool()) {
                booleanModuleData->setValue(element.property("default").toBool());
            }
            connect(booleanModuleData.get(), &BooleanModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
            parameters_.append(booleanModuleData);
            parameterArgs_.setProperty(booleanModuleData->id(), booleanData->boolean());
        } else if (type == "float") {
            auto floatModuleData = std::make_shared<FloatModuleData>(element);
            auto floatData = std::make_shared<FloatData>();
            floatModuleData->setNodeData(floatData);
            if (element.hasProperty("default") && element.property("default").isNumber()) {
                floatModuleData->setValue(element.property("default").toNumber());
            }
            connect(floatModuleData.get(), &FloatModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
            parameters_.append(floatModuleData);
            parameterArgs_.setProperty(floatModuleData->id(), floatData->number());
        } else if (type == "choice") {
            auto choiceModuleData = std::make_shared<ChoiceModuleData>(element);
            auto choiceData = std::make_shared<ChoiceData>();
            choiceModuleData->setNodeData(choiceData);
            if (element.hasProperty("default")) {
                if (element.property("default").isNumber()) {
                    choiceModuleData->setIndex(element.property("default").toInt());
                }
                if (element.property("default").isString()) {
                    QJSValue list = element.property("choices");
                    int length = list.property("length").toInt();
                    for (int i = 0; i < length; i++) {
                        if (list.property(i).toString() == element.property("default").toString()) {
                            choiceModuleData->setIndex(i);
                        }
                    }
                }
            }
            connect(choiceModuleData.get(), &ChoiceModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
            parameters_.append(choiceModuleData);
            parameterArgs_.setProperty(choiceModuleData->id(), choiceData->string());
        } else {
            parameters_.append(nullptr);
        }
    }

    outputsDefinition_ = module_.property("outputs");
    numOutputs_ = outputsDefinition_.property("length").toInt();
    for (int index = 0; index < numOutputs_; ++index) {
        QJSValue element = outputsDefinition_.property(index);
        QString type = element.property("type").toString();
        if (type == "spectrum") {
            auto spectrumData = std::make_shared<SpectrumModuleData>(element);
            outputs_.append(spectrumData);
        } else if (type == "int") {
            outputs_.append(std::make_shared<IntegerModuleData>(element));
        } else if (type == "bool") {
            outputs_.append(std::make_shared<BooleanModuleData>(element));
        } else if (type == "float") {
            outputs_.append(std::make_shared<FloatModuleData>(element));
        } else {
            outputs_.append(nullptr);
        }
    }

    setupDockWidget();

    if (atLeastOneInputRequired) {
        validationState_ = NodeValidationState::Warning;
        validationMessage_ = QString("Missing inputs");
    } else {
        calculate();
    }
}

ScriptWrapperModel::~ScriptWrapperModel()
{
    delete dockWidget_;
}

void ScriptWrapperModel::setCaption(const QString &caption)
{
     caption_ = caption;
}

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
    if (portType == PortType::In) {
        if (index >= numInputs_) {
            return "";
        }
        return inputs_[index]->caption();
    }
    else if (portType == PortType::Out) {
        if (index >= numOutputs_) {
            return "";
        }
        return outputs_[index]->caption();
    }
    else {
        return "";
    }
}

unsigned int ScriptWrapperModel::nPorts(QtNodes::PortType portType) const
{
    if (portType == PortType::In) {
        return numInputs_;
    }
    else if (portType == PortType::Out) {
        return numOutputs_;
    }
    else {
        return 0;
    }
}

QtNodes::NodeDataType ScriptWrapperModel::dataType(QtNodes::PortType portType, QtNodes::PortIndex index) const
{
    QString typeName, caption;
    if (portType == PortType::In) {
        if (index >= numInputs_) {
            return NodeDataType();
        }
        typeName =  inputs_[index]->typeName();
        caption =  inputs_[index]->caption();
    }
    else if (portType == PortType::Out) {
        if (index >= numOutputs_) {
            return NodeDataType();
        }
        typeName =  outputs_[index]->typeName();
        caption =  outputs_[index]->caption();
    }
    else {
        return NodeDataType();
    }
    return NodeDataType { typeName, caption};
}

void ScriptWrapperModel::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex index)
{
    QString id = inputs_[index]->id();
    QString type = inputs_[index]->type();
    bool required = inputs_[index]->required();

    if (!data) {
        inputs_[index]->setNodeData(nullptr);
        if (type == "spectrum") {
            auto inputModuleData = std::dynamic_pointer_cast<SpectrumModuleData>(inputs_[index]);
            moduleChart_->removeSpectrumData(inputModuleData);
        }
        if (required) {
            for (int outIndex = 0; outIndex < numOutputs_; ++outIndex) {
                outputs_[outIndex]->setNodeData(nullptr);
                Q_EMIT dataUpdated(outIndex);
                auto ouput = std::dynamic_pointer_cast<SpectrumModuleData>(outputs_[outIndex]);
                moduleChart_->removeSpectrumData(ouput);
            }
            validationState_ = NodeValidationState::Warning;
            validationMessage_ = QString("Missing or invalid inputs");
        } else {
            inputArgs_.setProperty(id, QJSValue::UndefinedValue);
            calculate();
        }
        return;
    }

    if (type == "spectrum") {
        auto inputData = std::dynamic_pointer_cast<SpectrumData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<SpectrumModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        moduleChart_->appendSpectrumData(inputModuleData);
        QJSValue array = js_->newArray(8);
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
            array.setProperty(freq, inputData->getValue(freq));
        }
        inputArgs_.setProperty(id, array);
    } else if (type == "int") {
        auto inputData = std::dynamic_pointer_cast<IntegerData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<IntegerModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        inputArgs_.setProperty(id, inputData->number());
    } else if (type == "bool") {
        auto inputData = std::dynamic_pointer_cast<BooleanData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<BooleanModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        inputArgs_.setProperty(id, inputData->boolean());
    } else if (type == "float") {
        auto inputData = std::dynamic_pointer_cast<FloatData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<FloatModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        inputArgs_.setProperty(id, inputData->number());
    } else {
        return;
    }

    validationState_ = NodeValidationState::Valid;
    validationMessage_ = QString("");

    calculate();
}

void ScriptWrapperModel::setupDockWidget()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QTabWidget* tabWidget = new QTabWidget();

    QFrame* infoFrame = new QFrame();
    QFormLayout* infoLayout = new QFormLayout();
    infoLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
    infoFrame->setLayout(infoLayout);
    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setText(caption_);
    infoLayout->addRow("Name", nameEdit);
    QPlainTextEdit* descriptionEdit = new QPlainTextEdit();
    descriptionEdit->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    descriptionEdit->setPlainText(description_);
    infoLayout->addRow("Description", descriptionEdit);

    connect(nameEdit, &QLineEdit::textEdited, this, &ScriptWrapperModel::setCaption);
    connect(descriptionEdit, &QPlainTextEdit::textChanged, [=]() {
        setDescription(descriptionEdit->toPlainText());
    });

    if (hasPicture_) {
        QLabel* picture = new QLabel();

        picture->setBackgroundRole(QPalette::Base);
        picture->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
        picture->setPixmap(pixmap_);

        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setAlignment(Qt::AlignCenter);
        scrollArea->setWidget(picture);
        scrollArea->setWidgetResizable(false);
        scrollArea->setVisible(true);

        picture->setScaledContents(true);
        scrollArea->adjustSize();

        infoLayout->addRow("Picture", scrollArea);
    }


    QFrame* dataFrame = new QFrame();
    QVBoxLayout* dataLayout = new QVBoxLayout();
    dataFrame->setLayout(dataLayout);

    if (numInputs_ > 0) {
        QGroupBox* inputGroup = new QGroupBox("Inputs");
        QFormLayout* inputLayout = new QFormLayout();
        inputLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
        inputGroup->setLayout(inputLayout);
        dataLayout->addWidget(inputGroup);
        for (int index = 0; index < numInputs_; ++index) {
            QWidget* widget = inputs_[index]->getWidget();
            inputLayout->addRow(inputs_[index]->description(), widget);
            widget->setDisabled(true);
            widget->show();
        }
    }

    if (numParameters_ > 0) {
        QGroupBox* parameterGroup = new QGroupBox("Parameters");
        QFormLayout* parameterLayout = new QFormLayout();
        parameterLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
        parameterGroup->setLayout(parameterLayout);
        dataLayout->addWidget(parameterGroup);
        for (int index = 0; index < numParameters_; ++index) {
            QWidget* widget = parameters_[index]->getWidget();
            parameterLayout->addRow(parameters_[index]->description(), widget);
            widget->show();
            if (parameters_[index]->type() == "spectrum") {
                SpectrumModuleData* specrtumModuleData = static_cast<SpectrumModuleData*>(parameters_[index].get());
                connect(specrtumModuleData, &SpectrumModuleData::contextMenuRequested, this, &ScriptWrapperModel::spectrumContextMenu);
            }
        }
    }

    if (numOutputs_ > 0) {
        QGroupBox* outputGroup = new QGroupBox("Outputs");
        QFormLayout* outputLayout = new QFormLayout();
        outputLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
        outputGroup->setLayout(outputLayout);
        dataLayout->addWidget(outputGroup);
        for (int index = 0; index < numOutputs_; ++index) {
            QWidget* widget = outputs_[index]->getWidget();
            outputLayout->addRow(outputs_[index]->description(), widget);
            widget->setDisabled(true);
            widget->show();
        }
    }

    dataLayout->addStretch(1);

    moduleChartView_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    moduleChartView_->setContextMenuPolicy(Qt::CustomContextMenu);
    moduleChartView_->setChart(moduleChart_);
    connect(moduleChartView_, &QChartView::customContextMenuRequested, this, &ScriptWrapperModel::graphContextMenu);

    tabWidget->addTab(infoFrame, "Info");
    tabWidget->addTab(dataFrame, "Parameters");
    tabWidget->addTab(moduleChartView_, "Graph");

    mainLayout->addWidget(tabWidget);
    dockWidget_->setLayout(mainLayout);
}

void ScriptWrapperModel::calculate()
{    
    for (int index = 0; index < numInputs_; index++) {
        if (inputs_[index]->required() && !inputs_[index]->getNodeData()) {
            validationState_ = NodeValidationState::Warning;
            validationMessage_ = QString("Missing or invalid inputs");
            return;
        }
    }

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
    if (!result.isObject()) {
        validationState_ = NodeValidationState::Error;
        validationMessage_ = QString("Script output is not an object");
        return;
    }

    for (int index = 0; index < numOutputs_; index++) {
        QString type = outputs_[index]->type();
        QString id = outputs_[index]->id();
        if (!result.hasProperty(id)) {
            outputs_[index]->setNodeData(nullptr);
            Q_EMIT dataUpdated(index);
            continue;
        }
        if (type == "spectrum") {
            if (!result.property(id).isArray()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output '%1' is not an array (spectrum)").arg(id);
                return;
            }
            if (result.property(id).property("length").toInt() != 8) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output '%1' must be an array of size 8 (spectrum)").arg(id);
                return;
            }
            std::shared_ptr<SpectrumData> outputData;
            auto ouput = std::dynamic_pointer_cast<SpectrumModuleData>(outputs_[index]);
            if (ouput->getNodeData()) {
                outputData = std::static_pointer_cast<SpectrumData>(ouput->getNodeData());
            } else {
                outputData = std::make_shared<SpectrumData>();
                ouput->setNodeData(outputData);
                moduleChart_->appendSpectrumData(ouput);
            }
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                ouput->setValue(freq, result.property(id).property(freq).toNumber());
            }
        } else if (type == "int") {
            if (!result.property(id).isNumber()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output '%1' is not an number (int)").arg(id);
                return;
            }
            std::shared_ptr<IntegerData> outputData;
            auto ouput = std::dynamic_pointer_cast<IntegerModuleData>(outputs_[index]);
            if (ouput->getNodeData()) {
                outputData = std::static_pointer_cast<IntegerData>(ouput->getNodeData());
            } else {
                outputData = std::make_shared<IntegerData>();
                ouput->setNodeData(outputData);
            }
            ouput->setValue(result.property(id).toNumber());
        } else if (type == "bool") {
            if (!result.property(id).isBool()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output '%1' is not an boolean (bool)").arg(id);
                return;
            }
            std::shared_ptr<BooleanData> outputData;
            auto ouput = std::dynamic_pointer_cast<BooleanModuleData>(outputs_[index]);
            if (ouput->getNodeData()) {
                outputData = std::static_pointer_cast<BooleanData>(ouput->getNodeData());
            } else {
                outputData = std::make_shared<BooleanData>();
                ouput->setNodeData(outputData);
            }
            ouput->setValue(result.property(id).toBool());
        } else if (type == "float") {
            if (!result.property(id).isNumber()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output '%1' is not an number (float)").arg(id);
                return;
            }
            std::shared_ptr<FloatData> outputData;
            auto ouput = std::dynamic_pointer_cast<FloatModuleData>(outputs_[index]);
            if (ouput->getNodeData()) {
                outputData = std::static_pointer_cast<FloatData>(ouput->getNodeData());
            } else {
                outputData = std::make_shared<FloatData>();
                ouput->setNodeData(outputData);
            }
            ouput->setValue(result.property(id).toNumber());
        }
        Q_EMIT dataUpdated(index);
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

QJsonObject ScriptWrapperModel::save() const
{
    QJsonObject modelJson;

    modelJson["name"] = name();
    modelJson["description"] = description();

    QJsonObject inputsJson;

    for (int index = 0; index < numInputs_; ++index) {
        QJsonObject input;
        QString type = inputs_[index]->type();
        QString id = inputs_[index]->id();
        input["type"] = type;
        input["typeName"] = inputs_[index]->typeName();
        input["description"] = inputs_[index]->description();
        if (type == "spectrum") {
            QJsonArray array;
            auto inputData = std::static_pointer_cast<SpectrumModuleData>(inputs_[index]);
            for (int freq = 0; freq < 8; ++freq) {
                array.append(inputData->getValue(freq));
            }
            input["data"] = array;
        } else if (type == "int") {
            auto inputData = std::static_pointer_cast<IntegerModuleData>(inputs_[index]);
            input["data"] = inputData->getValue();
        } else if (type == "bool") {
            auto inputData = std::static_pointer_cast<BooleanModuleData>(inputs_[index]);
            input["data"] = inputData->getValue();
        } else if (type == "float") {
            auto inputData = std::static_pointer_cast<FloatModuleData>(inputs_[index]);
            input["data"] = inputData->getValue();
        } else if (type == "choice") {
            auto inputData = std::static_pointer_cast<ChoiceModuleData>(inputs_[index]);
            input["data"] = inputData->getString();
        }
        inputsJson[id] = input;
    }

    modelJson["inputs"] = inputsJson;

    QJsonObject parametersJson;

    for (int index = 0; index < numParameters_; ++index) {
        QJsonObject parameter;
        QString type = parameters_[index]->type();
        QString id = parameters_[index]->id();
        parameter["type"] = type;
        parameter["typeName"] = parameters_[index]->typeName();
        parameter["description"] = parameters_[index]->description();
        if (type == "spectrum") {
            QJsonArray array;
            auto parameterData = std::static_pointer_cast<SpectrumModuleData>(parameters_[index]);
            for (int freq = 0; freq < 8; ++freq) {
                array.append(parameterData->getValue(freq));
            }
            parameter["data"] = array;
        } else if (type == "int") {
            auto parameterData = std::static_pointer_cast<IntegerModuleData>(parameters_[index]);
            parameter["data"] = parameterData->getValue();
        } else if (type == "bool") {
            auto parameterData = std::static_pointer_cast<BooleanModuleData>(parameters_[index]);
            parameter["data"] = parameterData->getValue();
        } else if (type == "float") {
            auto parameterData = std::static_pointer_cast<FloatModuleData>(parameters_[index]);
            parameter["data"] = parameterData->getValue();
        } else if (type == "choice") {
            auto parameterData = std::static_pointer_cast<ChoiceModuleData>(parameters_[index]);
            parameter["data"] = parameterData->getString();
        }
        parametersJson[id] = parameter;
    }

    modelJson["parameters"] = parametersJson;

    QJsonObject outputsJson;

    for (int index = 0; index < numOutputs_; ++index) {
        QJsonObject output;
        QString type = outputs_[index]->type();
        QString id = outputs_[index]->id();
        output["type"] = type;
        output["typeName"] = outputs_[index]->typeName();
        output["description"] = outputs_[index]->description();
        if (type == "spectrum") {
            QJsonArray array;
            auto outputData = std::static_pointer_cast<SpectrumModuleData>(outputs_[index]);
            for (int freq = 0; freq < 8; ++freq) {
                array.append(outputData->getValue(freq));
            }
            output["data"] = array;
        } else if (type == "int") {
            auto outputData = std::static_pointer_cast<IntegerModuleData>(outputs_[index]);
            output["data"] = outputData->getValue();
        } else if (type == "bool") {
            auto outputData = std::static_pointer_cast<BooleanModuleData>(outputs_[index]);
            output["data"] = outputData->getValue();
        } else if (type == "float") {
            auto outputData = std::static_pointer_cast<FloatModuleData>(outputs_[index]);
            output["data"] = outputData->getValue();
        } else if (type == "choice") {
            auto outputData = std::static_pointer_cast<ChoiceModuleData>(outputs_[index]);
            output["data"] = outputData->getString();
        }
        outputsJson[id] = output;
    }

    modelJson["outputs"] = outputsJson;

    return modelJson;
}

void ScriptWrapperModel::restore(const QJsonObject &json)
{
    QJsonObject parametersJson = json["parameters"].toObject();

    for (int index = 0; index < numParameters_; ++index) {

        QString id = parameters_[index]->id();
        QString type = parameters_[index]->type();

        if (parametersJson[id].isUndefined()) {
            continue;
        }
        QJsonObject parameter = parametersJson[id].toObject();

        if (type != parameter["type"].toString()) {
            continue;
        }

        if (type == "spectrum") {
            QJsonArray array = parameter["data"].toArray();
            auto parameterData = std::static_pointer_cast<SpectrumModuleData>(parameters_[index]);
            for (int freq = 0; freq < 8; ++freq) {
                parameterData->setValue(freq, array[freq].toDouble());
            }
        } else if (type == "int") {
            auto parameterData = std::static_pointer_cast<IntegerModuleData>(parameters_[index]);
            parameterData->setValue(parameter["data"].toInt());
        } else if (type == "bool") {
            auto parameterData = std::static_pointer_cast<BooleanModuleData>(parameters_[index]);
            parameterData->setValue(parameter["data"].toBool());
        } else if (type == "float") {
            auto parameterData = std::static_pointer_cast<FloatModuleData>(parameters_[index]);
            parameterData->setValue(parameter["data"].toDouble());
        }
    }
    parameterChanged();
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
        QString id = parameters_[index]->id();
        QString type = parameters_[index]->type();
        if (type == "spectrum") {
            auto parameterData = std::static_pointer_cast<SpectrumModuleData>(parameters_[index]);
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                parameterArgs_.property(id).setProperty(freq, parameterData->getValue(freq));
            }
        } else if (type == "int") {
            auto parameterData = std::static_pointer_cast<IntegerModuleData>(parameters_[index]);
            parameterArgs_.setProperty(id, parameterData->getValue());
        } else if (type == "bool") {
            auto parameterData = std::static_pointer_cast<BooleanModuleData>(parameters_[index]);
            parameterArgs_.setProperty(id, parameterData->getValue());
        } else if (type == "float") {
            auto parameterData = std::static_pointer_cast<FloatModuleData>(parameters_[index]);
            parameterArgs_.setProperty(id, parameterData->getValue());
        } else if (type == "choice") {
            auto parameterData = std::static_pointer_cast<ChoiceModuleData>(parameters_[index]);
            parameterArgs_.setProperty(id, parameterData->getString());
        }
    }

    calculate();
}

void ScriptWrapperModel::graphContextMenu(const QPoint &pos)
{
    QMenu* menu = new QMenu();
    QList<QBarSet*> setList = moduleChart_->barSetList();

    for (int index = 0; index < setList.size(); index++) {
        QBarSet* set = setList[index];
        if (set->count() == 0) {
            continue;
        }
        QAction* action = menu->addAction(set->label());
        action->setCheckable(true);
        action->setChecked(moduleChart_->isVisible(set));
        action->setProperty("barSetIndex", index);
        connect(action, &QAction::toggled, this, &ScriptWrapperModel::toggleBarSet);
    }
    menu->addSeparator();

    QAction* action = menu->addAction("Show NR curves");
    action->setCheckable(true);
    action->setChecked(moduleChart_->isNRVisible());
    connect(action, &QAction::toggled, this, &ScriptWrapperModel::toggleNR);

    menu->popup(moduleChartView_->mapToGlobal(pos));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void ScriptWrapperModel::toggleBarSet()
{
    QAction *action = qobject_cast<QAction *>(sender());
    bool checked = action->isChecked();
    int barSetIndex = action->property("barSetIndex").toInt();
    QBarSet* set = moduleChart_->barSetList()[barSetIndex];
    if (checked) {
        moduleChart_->showBarSet(set);
    } else {
        moduleChart_->hideBarSet(set);
    }
}

void ScriptWrapperModel::toggleNR()
{
    if (moduleChart_->isNRVisible()) {
        moduleChart_->hideNR();
    } else {
        moduleChart_->showNR();
    }
}

void ScriptWrapperModel::spectrumContextMenu(const QPoint &pos, SpectrumModuleData* spectrumModuleData)
{
    QMenu* menu = new QMenu();
    QAction* add_3_action = menu->addAction("add 3dB");
    connect(add_3_action, &QAction::triggered, [=]() {
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++) {
            spectrumModuleData->setValue(freq, spectrumModuleData->getValue(freq) + 3.0);
        }
    });
    QAction* add_6_action = menu->addAction("add 6dB");
    connect(add_6_action, &QAction::triggered, [=]() {
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++) {
            spectrumModuleData->setValue(freq, spectrumModuleData->getValue(freq) + 6.0);
        }
    });
    QAction* add_10_action = menu->addAction("add 10dB");
    connect(add_10_action, &QAction::triggered, [=]() {
        for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; freq++) {
            spectrumModuleData->setValue(freq, spectrumModuleData->getValue(freq) + 10.0);
        }
    });
    menu->addSeparator();
    QAction* action = menu->addAction("Set from database...");
    for (int index = 0; index < numInputs_; index++) {
        if (parameters_[index].get() == spectrumModuleData) {
            action->setProperty("parameterIndex", index);
        }
    }
    connect(action, &QAction::triggered, this, &ScriptWrapperModel::setFromDb);
    QPoint global = spectrumModuleData->getWidget()->mapToGlobal(pos);
    menu->popup(global);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
}

void ScriptWrapperModel::setFromDb()
{
    QAction *action = qobject_cast<QAction *>(sender());
    int parameterIndex = action->property("parameterIndex").toInt();
    auto spectrumModelData = std::static_pointer_cast<SpectrumModuleData>(parameters_[parameterIndex]);
    db_->setFromDb(spectrumModelData);
}
