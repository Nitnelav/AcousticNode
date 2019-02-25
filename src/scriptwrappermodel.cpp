#include "scriptwrappermodel.h"

ScriptWrapperModel::ScriptWrapperModel(QJSEngine* engine, QString path):
    js_(engine),
    path_(path)
{
    moduleChart_ = new ModuleGraph();
    moduleChartView_ = new QChartView();

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
            if (type == "spectrum") {
                auto spectrumData = std::make_shared<SpectrumModuleData>(description);
                inputs_.append(spectrumData);
                moduleChart_->appendSpectrumData(spectrumData);
            } else if (type == "int") {
                inputs_.append(std::make_shared<IntegerModuleData>(description));
            } else if (type == "bool") {
                inputs_.append(std::make_shared<BooleanModuleData>(description));
            } else if (type == "float") {
                inputs_.append(std::make_shared<FloatModuleData>(description));
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
            if (type == "spectrum") {
                auto spectrumModuleData = std::make_shared<SpectrumModuleData>(description);
                auto spectrumData = std::make_shared<SpectrumData>();
                spectrumModuleData->setNodeData(spectrumData);
                connect(spectrumModuleData.get(), &SpectrumModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
                parameters_.append(spectrumModuleData);
                moduleChart_->appendSpectrumData(spectrumModuleData);
                QJSValue array = js_->newArray(8);
                for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                    array.setProperty(freq, spectrumModuleData->getValue(freq));
                }
                parameterArgs_.setProperty(index, array);
            } else if (type == "int") {
                auto integerModuleData = std::make_shared<IntegerModuleData>(description);
                auto integerData = std::make_shared<IntegerData>();
                integerModuleData->setNodeData(integerData);
                connect(integerModuleData.get(), &IntegerModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
                parameters_.append(integerModuleData);
                parameterArgs_.setProperty(index, integerData->number());
            } else if (type == "bool") {
                auto booleanModuleData = std::make_shared<BooleanModuleData>(description);
                auto booleanData = std::make_shared<BooleanData>();
                booleanModuleData->setNodeData(booleanData);
                connect(booleanModuleData.get(), &BooleanModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
                parameters_.append(booleanModuleData);
                parameterArgs_.setProperty(index, booleanData->boolean());
            } else if (type == "float") {
                auto floatModuleData = std::make_shared<FloatModuleData>(description);
                auto floatData = std::make_shared<FloatData>();
                floatModuleData->setNodeData(floatData);
                connect(floatModuleData.get(), &FloatModuleData::widgetDataChanged, this, &ScriptWrapperModel::parameterChanged);
                parameters_.append(floatModuleData);
                parameterArgs_.setProperty(index, floatData->number());
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
            if (type == "spectrum") {
                auto spectrumData = std::make_shared<SpectrumModuleData>(description);
                outputs_.append(spectrumData);
                moduleChart_->appendSpectrumData(spectrumData);
            } else if (type == "int") {
                outputs_.append(std::make_shared<IntegerModuleData>(description));
            } else if (type == "bool") {
                outputs_.append(std::make_shared<BooleanModuleData>(description));
            } else if (type == "float") {
                outputs_.append(std::make_shared<FloatModuleData>(description));
            } else {
                outputs_.append(nullptr);
            }
        }
    }
    if (!module_.hasProperty("calculate") || !module_.property("calculate").isCallable()) {
        throw MissingOrBadPropertyError(path.toLatin1(), "calculate");
    }

    setupDockWidget();

    if (numInputs_ == 0) {
        calculate();
    }
}

ScriptWrapperModel::~ScriptWrapperModel()
{
//    delete moduleGraph_;
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
    if (element.isError() || !element.hasProperty("typeName"))
        throw;
    return element.property("typeName").toString();
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
    if (element.isError() || !element.hasProperty("typeName"))
        throw;
    QString typeName = element.property("typeName").toString();
    return NodeDataType { typeName, typeName};
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

    if (type == "spectrum") {
        auto inputData = std::dynamic_pointer_cast<SpectrumData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<SpectrumModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        QJSValue array = js_->newArray(8);
        for (int i = 0; i < 8; ++i) {
            array.setProperty(i, inputData->getValue(i));
        }
        inputArgs_.setProperty(index, array);
    } else if (type == "int") {
        auto inputData = std::dynamic_pointer_cast<IntegerData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<IntegerModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        inputArgs_.setProperty(index, inputData->number());
    } else if (type == "bool") {
        auto inputData = std::dynamic_pointer_cast<BooleanData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<BooleanModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        inputArgs_.setProperty(index, inputData->boolean());
    } else if (type == "float") {
        auto inputData = std::dynamic_pointer_cast<FloatData>(data);
        auto inputModuleData = std::dynamic_pointer_cast<FloatModuleData>(inputs_[index]);
        inputModuleData->setNodeData(inputData);
        inputArgs_.setProperty(index, inputData->number());
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


void ScriptWrapperModel::setupDockWidget()
{
    dockWidget_ = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout();

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
            QWidget* widget = inputs_[index]->getWidget().get();
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
            QWidget* widget = parameters_[index]->getWidget().get();
            parameterLayout->addRow(parameters_[index]->description(), widget);
            widget->show();
        }
    }

    if (numOutputs_ > 0) {
        QGroupBox* outputGroup = new QGroupBox("Outputs");
        QFormLayout* outputLayout = new QFormLayout();
        outputLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
        outputGroup->setLayout(outputLayout);
        dataLayout->addWidget(outputGroup);
        for (int index = 0; index < numOutputs_; ++index) {
            QWidget* widget = outputs_[index]->getWidget().get();
            outputLayout->addRow(outputs_[index]->description(), widget);
            widget->setDisabled(true);
            widget->show();
        }
    }

    moduleChartView_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    moduleChartView_->setContextMenuPolicy(Qt::CustomContextMenu);
    moduleChartView_->setChart(moduleChart_);
    connect(moduleChartView_, &QChartView::customContextMenuRequested, this, &ScriptWrapperModel::graphContextMenu);

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(infoFrame);
    splitter->addWidget(dataFrame);
    splitter->addWidget(moduleChartView_);
    splitter->setStretchFactor(1, 0);
    splitter->setStretchFactor(2, 2);

    mainLayout->addWidget(splitter);
    dockWidget_->setLayout(mainLayout);
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
        if (type == "spectrum") {
            if (!result.property(index).isArray()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %1 is not an array (Spectrum)").arg(index);
                return;
            }
            if (result.property(index).property("length").toInt() != 8) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %1 must be an array of size 8 (Spectrum)").arg(index);
                return;
            }
            std::shared_ptr<SpectrumData> outputData;
            auto ouput = std::dynamic_pointer_cast<SpectrumModuleData>(outputs_[index]);
            if (ouput->getNodeData()) {
                outputData = std::static_pointer_cast<SpectrumData>(ouput->getNodeData());
            } else {
                outputData = std::make_shared<SpectrumData>();
                ouput->setNodeData(outputData);
            }
            for (int freq = FREQ_63Hz; freq <= FREQ_8kHz; ++freq) {
                ouput->setValue(freq, result.property(index).property(freq).toNumber());
            }
        } else if (type == "int") {
            if (!result.property(index).isNumber()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %1 is not an number (Int)").arg(index);
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
            ouput->setValue(result.property(index).toNumber());
        } else if (type == "bool") {
            if (!result.property(index).isBool()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %1 is not an boolean (bool)").arg(index);
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
            ouput->setValue(result.property(index).toBool());
        } else if (type == "float") {
            if (!result.property(index).isNumber()) {
                validationState_ = NodeValidationState::Error;
                validationMessage_ = QString("Script output at index %1 is not an number (float)").arg(index);
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
            ouput->setValue(result.property(index).toNumber());
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
        if (type == "spectrum") {
            auto parameterData = std::static_pointer_cast<SpectrumModuleData>(parameters_[index]);
            for (int freq = 0; freq < 8; ++freq) {
                parameterArgs_.property(index).setProperty(freq, parameterData->getValue(freq));
            }
        } else if (type == "int") {
            auto parameterData = std::static_pointer_cast<IntegerModuleData>(parameters_[index]);
            parameterArgs_.setProperty(index, parameterData->getValue());
        } else if (type == "bool") {
            auto parameterData = std::static_pointer_cast<BooleanModuleData>(parameters_[index]);
            parameterArgs_.setProperty(index, parameterData->getValue());
        } else if (type == "float") {
            auto parameterData = std::static_pointer_cast<FloatModuleData>(parameters_[index]);
            parameterArgs_.setProperty(index, parameterData->getValue());
        }
    }
    for (int i = 0; i < numInputs_; i++) {
        if (!inputs_[i]->getNodeData()) {
            return; // TODO : take "required" parameter into account
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
        QAction* action = menu->addAction(set->label());
        action->setCheckable(true);
        action->setChecked(moduleChart_->isVisible(set));
        action->setProperty("barSetIndex", index);
        connect(action, &QAction::toggled, this, &ScriptWrapperModel::toggleBarSet);
    }
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
