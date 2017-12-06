/*
 * the CWE_TabWidget is an extended version of a tab widget where
 * Tabs display a label AND a state
 * the data display area itself hold a standard QTabWidget, one tab per
 * variable group (as defined in the JSon config file)
 */

#include "cwe_tabwidget.h"
#include "ui_cwe_tabwidget.h"
#include "cwe_parampanel.h"
#include "cwe_stagestatustab.h"
#include "cwe_groupswidget.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "qdebug.h"

#include "../CFDClientProgram/vwtinterfacedriver.h"

CWE_TabWidget::CWE_TabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_TabWidget)
{
    ui->setupUi(this);

    groupWidgetList = new QMap<QString, CWE_GroupsWidget *>();
    stageTabList    = new QMap<QString, CWE_StageStatusTab *>();
    //varTabWidgets   = new QMap<QString, QMap<QString, QWidget *> *>();
    //variableWidgets = new QMap<QString, InputDataType *>();

    this->setButtonMode(CWE_BTN_ALL);
    //this->setButtonMode(CWE_BTN_NONE);

    this->setViewState(SimCenterViewState::visible);
}

CWE_TabWidget::~CWE_TabWidget()
{
    delete ui;
}

void CWE_TabWidget::setupDriver(VWTinterfaceDriver * theDriver)
{
    myDriver = theDriver;
}

QWidget *CWE_TabWidget::currentWidget()
{
    return ui->stackedWidget->currentWidget();
}

void CWE_TabWidget::setCurrentWidget(QWidget *w)
{
    ui->stackedWidget->setCurrentWidget(w);
}

QWidget *CWE_TabWidget::widget(int idx)
{
    return ui->stackedWidget->widget(idx);
}



void CWE_TabWidget::setViewState(SimCenterViewState state)
{
    switch (state)
    {
    case SimCenterViewState::editable:
        m_viewState = SimCenterViewState::editable;
        break;
    case SimCenterViewState::hidden:
        m_viewState = SimCenterViewState::hidden;
        break;
    case SimCenterViewState::visible:
    default:
        m_viewState = SimCenterViewState::visible;
    }
}

SimCenterViewState CWE_TabWidget::viewState()
{
    return m_viewState;
}

void CWE_TabWidget::resetView()
{
    // delete all stage tabs and everything within
    for (auto stageItr = stageTabList->begin(); stageItr != stageTabList->end();)
    {
        delete stageItr.value();                    // delete the CWE_GroupTab for the stage
        stageItr = stageTabList->erase(stageItr);   // erase the stage from the map
    }
    // delete all groupsWidgets and everything within
    for (auto groupItr = groupWidgetList->begin(); groupItr != groupWidgetList->end();)
    {
        delete groupItr.value();                    // delete the CWE_GroupTab for the stage
        groupItr = groupWidgetList->erase(groupItr);   // erase the stage from the map
    }
}
int CWE_TabWidget::addVarTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo, QMap<QString,QString> * setVars)
{
    int index = addVarTab(key, label);
    if (index >= 0)
    {
        addVarsToTab(key, label, varList, varsInfo, setVars);
    }
    return index;
}

void CWE_TabWidget::addVarsData(QJsonObject JSONgroup, QJsonObject JSONvars)
{

}

/* *** moved to SCtrDataWidget ***

QWidget * CWE_TabWidget::addStd(QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QVariant defaultOption = JSONvar["default"].toVariant();
    QString unit           = JSONvar["unit"].toString();
    // QJson fails to convert "1" to int, thus: QString::toInt( QJson::toString() )
    int precision          = JSONvar["precision"].toString().toInt();

    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);
    theName->setMinimumHeight(16);
    //theName->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    QWidget * theValue;

    if (precision > 0) {
        theValue = new QDoubleSpinBox(parent);
        ((QDoubleSpinBox *)theValue)->setValue(defaultOption.toDouble());
        ((QDoubleSpinBox *)theValue)->setDecimals(precision);
    }
    else {
        theValue = new QSpinBox(parent);
        ((QSpinBox *)theValue)->setValue(defaultOption.toInt());
    }

    QLabel *theUnit = new QLabel(parent);
    theUnit->setText(unit);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theValue,row,1);
    layout->addWidget(theUnit, row,2);

    return theValue;
}

QWidget * CWE_TabWidget::addBool(QJsonObject JSONvar, QWidget *parent, QString * setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QCheckBox *theBox = new QCheckBox(parent);
    if (setVal == NULL)
    {
        theBox->setChecked(JSONvar["default"].toBool());
    }
    else
    {
        theBox->setChecked(setVal);
    }

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theBox, row,1);

    return theBox;
}

QWidget * CWE_TabWidget::addFile(QJsonObject JSONvar, QWidget *parent, QString * setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QLineEdit *theFileName = new QLineEdit(parent);
    theFileName->setText("unknown file name");

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theFileName,row,1,1,2);

    return theFileName;
}

QWidget * CWE_TabWidget::addChoice(QJsonObject JSONvar, QWidget *parent, QString * setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QComboBox *theSelection = new QComboBox(parent);
    QJsonObject combo_options = JSONvar["options"].toObject();

    QStandardItemModel *theModel = new QStandardItemModel();
    foreach (const QString &theKey, combo_options.keys())
    {
        //QStandardItem *itm = new QStandardItem(theKey);
        QStandardItem *itm = new QStandardItem(combo_options[theKey].toString());
        theModel->appendRow(itm);
    }
    theSelection->setModel(theModel);
    if ((setVal == NULL) || (!combo_options.contains(*setVal)))
    {
        theSelection->setCurrentText(combo_options[JSONvar["default"].toString()].toString());
    }
    else
    {
        theSelection->setCurrentText(combo_options[*setVal].toString());
    }

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theSelection,row,1,1,2);

    return theSelection;
}


QWidget * CWE_TabWidget::addVector3D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * CWE_TabWidget::addVector2D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * CWE_TabWidget::addTensor3D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * CWE_TabWidget::addTensor2D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * CWE_TabWidget::addUnknown(QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);

    return NULL;
}

void CWE_TabWidget::addType(const QString &varName, const QString &type, QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QWidget *widget;

    widget = NULL;

    if      (type == "std")      { widget = this->addStd(JSONvar, parent, setVal); }
    else if (type == "bool")     { widget = this->addBool(JSONvar, parent, setVal); }
    else if (type == "file")     { widget = this->addFile(JSONvar, parent, setVal); }
    else if (type == "choose")   { widget = this->addChoice(JSONvar, parent, setVal); }
    else if (type == "vector2D") { widget = this->addVector2D(JSONvar, parent, setVal); }
    else if (type == "tensor2D") { widget = this->addTensor2D(JSONvar, parent, setVal); }
    else if (type == "vector3D") { widget = this->addVector3D(JSONvar, parent, setVal); }
    else if (type == "tensor3D") { widget = this->addTensor3D(JSONvar, parent, setVal); }
    else                         { widget = this->addUnknown(JSONvar, parent, setVal); }

    // store information for reset operations, data collection, and validation
    InputDataType *varData = new InputDataType;
    varData->name        = varName;
    varData->displayName = JSONvar["displayname"].toString();
    varData->type        = type;
    varData->defValue    = JSONvar["default"].toString();
    varData->widget      = widget;
    if (type == "choose") {
        varData->options = new QJsonObject(JSONvar["options"].toObject());
    }
    else {
        varData->options = NULL;
    }
    variableWidgets->insert(varName, varData);
}

*/


void CWE_TabWidget::setIndex(int idx)
{
    // set active tab to idx
    ui->stackedWidget->setCurrentIndex(idx);
    activeIndex   = ui->stackedWidget->currentIndex();
    displayWidget = ui->stackedWidget->currentWidget();

    // set stylesheet for buttons
    foreach (const QString &key, stageTabList->keys())
    {
        CWE_StageStatusTab *btn = stageTabList->value(key);
        //qDebug() << idx << "<>" << btn->index();

        if (btn->index() == idx)
            { btn->setActive(); }
        else
            { btn->setInActive(); }
    }
}

void CWE_TabWidget::setWidget(QWidget *w)
{
    // set active tab to idx
    ui->stackedWidget->setCurrentWidget(w);
    activeIndex   = ui->stackedWidget->currentIndex();
    displayWidget = ui->stackedWidget->currentWidget();
}

void CWE_TabWidget::on_pbtn_run_clicked()
{
    myDriver->getCurrentCase()->startStageApp(currentSelectedStage);

    // enable the cancel button
    //this->setButtonMode(CWE_BTN_CANCEL);
}

QMap<QString, QString> CWE_TabWidget::collectParamData()
{
    /*
     * TODO:
     * -- loop through groupsWidgetList and collect information fromCWE_GroupsWidgets
     */

    QString val;
    QMap<QString, QString> currentParameters;

    // collect parameter values from all groupWidgets in groupWidgetList
    foreach (const CWE_GroupsWidget *itm, groupWidgetList->values())
    {
        QMap<QString, QString> groupParams = itm->collectParamData();

        // add to output
        foreach (QString varName, groupParams.keys())
        {
            val = groupParams.value(varName);
            currentParameters.insert(varName, val);
        }
    }

    return currentParameters;
}

void CWE_TabWidget::on_pbtn_cancel_clicked()
{
    // initiate job cancellation

    // enable the run button
    this->setButtonMode(CWE_BTN_RUN);
}

void CWE_TabWidget::on_pbtn_results_clicked()
{
    // set run and rollback button active
    this->setButtonMode(CWE_BTN_RESULTS|CWE_BTN_ROLLBACK);

    // switch to the results tab
    emit switchToResultsTab();
}

void CWE_TabWidget::on_pbtn_rollback_clicked()
{
    myDriver->getCurrentCase()->rollBack(currentSelectedStage);

    // set run button active
    //TODO: ???? Remove? this->setButtonMode(CWE_BTN_RUN);
}

void CWE_TabWidget::on_groupTabSelected(int idx, QString selectedStage)
{
    currentSelectedStage = selectedStage;
    this->setIndex(idx);

    // check for status of tab #idx

    // set button state accordingly
    //this->setButtonMode(CWE_BTN_RUN);
}

QString CWE_TabWidget::getStateText(StageState theState)
{
    if (theState == StageState::ERROR)
        return "*** ERROR ***";
    if (theState == StageState::FINISHED)
        return "Task Finished";
    if (theState == StageState::LOADING)
        return "Loading Data ...";
    if (theState == StageState::RUNNING)
        return "Task Running";
    if (theState == StageState::UNRUN)
        return "Not Yet Run";
    return "*** TOTAL ERROR ***";
}

void CWE_TabWidget::setButtonMode(uint mode)
{
    bool btnState;

    btnState = (mode & CWE_BTN_RUN)?true:false;
    ui->pbtn_run->setEnabled(btnState);

    btnState = (mode & CWE_BTN_CANCEL)?true:false;
    ui->pbtn_cancel->setEnabled(btnState);

    btnState = (mode & CWE_BTN_RESULTS)?true:false;
    ui->pbtn_results->setEnabled(btnState);

    btnState = (mode & CWE_BTN_ROLLBACK)?true:false;
    ui->pbtn_rollback->setEnabled(btnState);

}

void CWE_TabWidget::addStageTab(QString key, QJsonObject &obj)
{
    /*
     * create a stage tab for a stage identified by key
     *
     * the stage tab will add a pointer to itself to the m_stageTabs QMap
     */

    CWE_StageTab *newTab = new CWE_StageTab();

    m_stageTabs->insert(key, newTab);
}