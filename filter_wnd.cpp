#include <QDebug>
#include <QPainter>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QDate>
#include <QLayout>
#include <QMessageBox>
#include "filter_wnd.h"
#include <iostream>

FilterWnd::FilterWnd(QWidget *parent) : QWidget(parent)
    ,ui(new Ui::FilterWnd),filter_choosen_max_num(32)
{
    ui->setupUi(this);
    setFixedWidth(size().width());
    setFixedHeight(size().height());
    connect(ui->listWidget_all, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chooseItem(QListWidgetItem *)));
    connect(ui->listWidge_choosen, SIGNAL(itemSelectionChanged()), this, SLOT(onClickedChoosenFilter()));

    connect(ui->pushButton_remove, SIGNAL(clicked()), this, SLOT(onRemove()));
    connect(ui->pushButton_clear, SIGNAL(clicked()), this, SLOT(onClear()));
    connect(ui->pushButton_up, SIGNAL(clicked()), this, SLOT(onUp()));
    connect(ui->pushButton_down, SIGNAL(clicked()), this, SLOT(onDown()));
    connect(ui->pushButton_enable, SIGNAL(clicked()), this, SLOT(onEnable()));
    connect(ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(onApplyProperty()));

    m_variantFactory = new QtVariantEditorFactory();
    m_variantEditor = new QtTreePropertyBrowser();
    m_variantManager = new QtVariantPropertyManager();
    //onRemove
#if 0

    int i = 0;
    QtProperty *topItem = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                QString::number(i++) + QLatin1String(" Group Property"));

    QtVariantProperty *item;
    item = m_variantManager->addProperty(QVariant::Bool, QString::number(i++) + QLatin1String(" Bool Property"));
    item->setValue(true);
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Int, QString::number(i++) + QLatin1String(" Int Property"));
    item->setValue(20);
    item->setAttribute(QLatin1String("minimum"), 0);
    item->setAttribute(QLatin1String("maximum"), 100);
    item->setAttribute(QLatin1String("singleStep"), 10);
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Double, QString::number(i++) + QLatin1String(" Double Property"));
    item->setValue(1.2345);
    item->setAttribute(QLatin1String("singleStep"), 0.1);
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property"));
    item->setValue("Value");
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Date, QString::number(i++) + QLatin1String(" Date Property"));
    item->setValue(QDate::currentDate().addDays(2));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Time, QString::number(i++) + QLatin1String(" Time Property"));
    item->setValue(QTime::currentTime());
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::DateTime, QString::number(i++) + QLatin1String(" DateTime Property"));
    item->setValue(QDateTime::currentDateTime());
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::KeySequence, QString::number(i++) + QLatin1String(" KeySequence Property"));
    item->setValue(QKeySequence(Qt::ControlModifier | Qt::Key_Q));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Char, QString::number(i++) + QLatin1String(" Char Property"));
    item->setValue(QChar(386));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Locale, QString::number(i++) + QLatin1String(" Locale Property"));
    item->setValue(QLocale(QLocale::Polish, QLocale::Poland));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Point, QString::number(i++) + QLatin1String(" Point Property"));
    item->setValue(QPoint(10, 10));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::PointF, QString::number(i++) + QLatin1String(" PointF Property"));
    item->setValue(QPointF(1.2345, -1.23451));
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Size, QString::number(i++) + QLatin1String(" Size Property"));
    item->setValue(QSize(20, 20));
    item->setAttribute(QLatin1String("minimum"), QSize(10, 10));
    item->setAttribute(QLatin1String("maximum"), QSize(30, 30));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::SizeF, QString::number(i++) + QLatin1String(" SizeF Property"));
    item->setValue(QSizeF(1.2345, 1.2345));
    item->setAttribute(QLatin1String("decimals"), 3);
    item->setAttribute(QLatin1String("minimum"), QSizeF(0.12, 0.34));
    item->setAttribute(QLatin1String("maximum"), QSizeF(20.56, 20.78));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Rect, QString::number(i++) + QLatin1String(" Rect Property"));
    item->setValue(QRect(10, 10, 20, 20));
    topItem->addSubProperty(item);
    item->setAttribute(QLatin1String("constraint"), QRect(0, 0, 50, 50));

    item = m_variantManager->addProperty(QVariant::RectF, QString::number(i++) + QLatin1String(" RectF Property"));
    item->setValue(QRectF(1.2345, 1.2345, 1.2345, 1.2345));
    topItem->addSubProperty(item);
    item->setAttribute(QLatin1String("constraint"), QRectF(0, 0, 50, 50));
    item->setAttribute(QLatin1String("decimals"), 3);

    item = m_variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),
                    QString::number(i++) + QLatin1String(" Enum Property"));
    QStringList enumNames;
    enumNames << "Enum0" << "Enum1" << "Enum2";
    item->setAttribute(QLatin1String("enumNames"), enumNames);
    item->setValue(1);
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QtVariantPropertyManager::flagTypeId(),
                    QString::number(i++) + QLatin1String(" Flag Property"));
    QStringList flagNames;
    flagNames << "Flag0" << "Flag1" << "Flag2";
    item->setAttribute(QLatin1String("flagNames"), flagNames);
    item->setValue(5);
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::SizePolicy, QString::number(i++) + QLatin1String(" SizePolicy Property"));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Font, QString::number(i++) + QLatin1String(" Font Property"));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Cursor, QString::number(i++) + QLatin1String(" Cursor Property"));
    topItem->addSubProperty(item);

    item = m_variantManager->addProperty(QVariant::Color, QString::number(i++) + QLatin1String(" Color Property"));
    topItem->addSubProperty(item);


    m_variantEditor->setFactoryForManager(m_variantManager, m_variantFactory);
    m_variantEditor->addProperty(topItem);
    m_variantEditor->setPropertiesWithoutValueMarked(true);
    m_variantEditor->setRootIsDecorated(false);

#endif

    QVBoxLayout *filter_list = new QVBoxLayout;
    filter_list->addWidget(ui->label_filter);
    filter_list->addWidget(ui->listWidget_all);
    filter_list->addWidget(ui->listWidge_choosen);

    QVBoxLayout *filter_button_v0 = new QVBoxLayout;
    filter_button_v0->addWidget(ui->pushButton_up);
    filter_button_v0->addWidget(ui->pushButton_remove);
    filter_button_v0->addWidget(ui->pushButton_enable);

    QVBoxLayout *filter_button_v1 = new QVBoxLayout;
    filter_button_v1->addWidget(ui->pushButton_down);
    filter_button_v1->addWidget(ui->pushButton_clear);
    filter_button_v1->addWidget(ui->pushButton_disable);

    QHBoxLayout *filter_button_h0 = new QHBoxLayout;
    filter_button_h0->addLayout(filter_button_v0);
    filter_button_h0->addLayout(filter_button_v1);

    filter_list->addLayout(filter_button_h0);

    QVBoxLayout *filter_property = new QVBoxLayout;
    filter_property->addWidget(m_variantEditor);
    filter_property->addWidget(ui->pushButton_apply);

    QGridLayout *main_layout = new QGridLayout;

    main_layout -> setColumnStretch(0, 1);
    main_layout -> setColumnStretch(1, 2);

    main_layout->addLayout(filter_list, 0, 0);
    main_layout->addLayout(filter_property, 0, 1);
    //main_layout->addLayout(function_list);

    m_ppFilters = NULL;
    setLayout(main_layout);

    for(int i= 0; i < 1024; i++)
    {
        m_filter_name[i] = QString();
        m_filter_ptr[i] =  NULL;
    }
    m_filter_len = 0;
    m_filter_choosen_num = 0;

    m_filter_info_idx = 0;

    for(int i= 0; i < filter_choosen_max_num; i++)
    {
        m_ppFilters_choosen[i] =  NULL;
    }
}

FilterWnd::~FilterWnd()
{
     delete ui;
}

bool FilterWnd::addFilterModule()
{
    ui->listWidget_all->clear();
    if(m_ppFilters)
    {
        for(int i = 0;;i++)
        {
            if(m_ppFilters[i] == NULL)
                break;
            const char *name = m_ppFilters[i]->pItem->getName();
            Module_Info *pInfo = new Module_Info;
            memset(pInfo, 0, sizeof(Module_Info));
            if(pInfo == NULL)
            {
                return true;
            }
            m_ppFilters[i]->pItem->getModuleInfo(pInfo);
            ui->listWidget_all->addItem(name);

            m_filter_name[m_filter_len] = QString(name);
            m_filter_ptr[m_filter_len] = m_ppFilters[i];
            m_filter_len++;

            delete pInfo;

        }
    }


    return true;
}

void FilterWnd::setFilterModule(CFilterItem **ppFilters)
{
    m_ppFilters = ppFilters;
    addFilterModule();
}

void FilterWnd::printMsg(QString msg)
{
    if(!msg.isEmpty())
        QMessageBox::information(this, tr("Food"), msg);
}

void FilterWnd::chooseItem(QListWidgetItem *itemChoosen)
{
    addFilterItem(itemChoosen->text());
}

void FilterWnd::addFilterItem(QString itemName)
{
    QListWidgetItem *lst1 = new QListWidgetItem(itemName, ui->listWidge_choosen);
    ui->listWidge_choosen->insertItem(1, lst1);
}

void FilterWnd::onClear()
{
    ui->listWidge_choosen->clear();
}

void FilterWnd::onUp()
{
    QList<QListWidgetItem*> list = ui->listWidge_choosen->selectedItems();

    if(list.size() == 0)
        return;

    QListWidgetItem* sel = list[0];
    if (sel)
    {
        int r = ui->listWidge_choosen->row(sel);
        if(r > 0)
        {
            ui->listWidge_choosen->takeItem(r);
            ui->listWidge_choosen->insertItem(r - 1, sel);
            ui->listWidge_choosen->setCurrentRow(r - 1);
        }
    }
}

CFilterItem *FilterWnd::getMatchedItem(QString name)
{
    QString filter_name;
    if(m_filter_info.find(name) != m_filter_info.end())
    {
        filter_name = m_filter_info[name].filter_name;
    }
    else
    {
        filter_name = name;
    }

    for(int i = 0; i < m_filter_len; i++)
    {
        if(m_filter_name[i] == filter_name)
        {
            return m_filter_ptr[i];
        }
    }

    return NULL;

}

void FilterWnd::cleanAllChoosenFilters()
{
    for(int i = 0; i < filter_choosen_max_num; i++)
    {
        if(m_ppFilters_choosen[i] != NULL)
        {
            delete m_ppFilters_choosen[i];
        }
        m_ppFilters_choosen[i] = NULL;
    }
    m_filter_choosen_num = 0;
}

void FilterWnd::hide()
{
    QWidget::hide();
}
void FilterWnd::clearFilterMap()
{
    m_filter_info.clear();
    m_filter_info_idx = 0;
}

QString FilterWnd::addItemToFilterMap(QString filter_name, QString filter_conf, QString flag)
{
    QString map_key = QString("%1:%2").arg(filter_name).arg(m_filter_info_idx++) + flag;
    FilterInfo filterInfo;
    filterInfo.filter_name = filter_name;
    filterInfo.filter_conf = filter_conf;
    m_filter_info.insert(map_key, filterInfo);
    return map_key;
}

QString FilterWnd::modItemToFilterMap(QString map_key, QString filter_conf)
{
    if(m_filter_info.find(map_key) != m_filter_info.end())
    {
        m_filter_info[map_key].filter_conf = filter_conf;
    }
    return map_key;
}

void FilterWnd::updateChoosenFilterList()
{
    clearFilterMap();
    ui->listWidge_choosen->clear();

    for(int i = 0; i < m_filter_choosen_num; i++)
    {
        if(m_ppFilters_choosen[i] != NULL)
        {
            const char *filter_name = m_ppFilters_choosen[i]->getName();
            m_ppFilters_choosen[i]->queryRuntimeInfo(m_filterInfo, m_filterInfo_len);
            QString map_key = addItemToFilterMap( QString(filter_name), QString(m_filterInfo));
            addFilterItem(map_key);
        }
    }

    QWidget::show();
}

void FilterWnd::show()
{
    updateChoosenFilterList();
    QWidget::show();
}

void FilterWnd::closeEvent(QCloseEvent *event)
{
    //hide();
    emit notifyMainUncheckedFilterCombox();
    QWidget::closeEvent(event);
}

QString FilterWnd::getFilterNewConf(QString confOld)
{
    TXMLHandlePtr ptHan, ptHanLoad;
    TXMLElePtr t1, t2, t3, t4, t5, t6,t7;
    ptHan = XMLParseOpenFromStr(confOld.toStdString().c_str());

    for(m_property_dic_iter = m_property_dic.begin(); m_property_dic_iter != m_property_dic.end(); ++m_property_dic_iter)
    {
        QString filter_name = m_property_dic_iter.key();
        QtProperty* proPtr= m_property_dic_iter.value();
        QString filter_val = proPtr->valueText();

        setAProperty(ptHan, filter_name, filter_val);
    }
    //void *str_save = XMLSaveToStrOpen();
    char str_save[4096 * 10];
    XMLSaveToStrProcess(ptHan, str_save);
    QString filter_conf_new = QString((char *)str_save);
    return filter_conf_new;
}

void FilterWnd::onApplyProperty()
{
    QList<QListWidgetItem*> list = ui->listWidge_choosen->selectedItems();

    if(list.size() == 0)
        return;

    QListWidgetItem* itemChoosen = list[0];

    if(m_filter_info.find(itemChoosen->text()) != m_filter_info.end())
    {
        QString filter_conf_new = getFilterNewConf(m_filter_info[itemChoosen->text()].filter_conf);
        m_filter_info[itemChoosen->text()].filter_conf = filter_conf_new;
    }
    else
    {
        if(m_ppFilters)
        {
            for(int i = 0;;i++)
            {
                if(m_ppFilters[i] == NULL)
                    break;
                const char *filter_name = m_ppFilters[i]->pItem->getName();
                if(itemChoosen->text() == QString(filter_name))
                {
                    m_ppFilters[i]->pItem->queryRuntimeInfo(m_filterInfo, m_filterInfo_len);
                    QString filter_conf = getFilterNewConf(QString(m_filterInfo));

                    QString map_key = addItemToFilterMap(QString(filter_name), filter_conf, QString(" - new"));
                    itemChoosen->setText(map_key);
                }
            }
        }
    }
}

void FilterWnd::onEnable()
{
    cleanAllChoosenFilters();
    int item_num = ui->listWidge_choosen->count();

    for(int i = 0; i < item_num; i++)
    {
        QListWidgetItem *item =  ui->listWidge_choosen->item(i);
        CFilterItem *filter_item = NULL;

        QString filter_name;
        QString filter_conf;
        if(m_filter_info.find(item->text()) != m_filter_info.end())
        {
            filter_name = m_filter_info[item->text()].filter_name;
            filter_conf = m_filter_info[item->text()].filter_conf;
        }
        else
        {
            filter_name = item->text();
        }

        for(int i = 0; i < m_filter_len; i++)
        {
            if(m_filter_name[i] == filter_name)
            {
                filter_item =  m_filter_ptr[i];
                break;
            }
        }

        if(filter_item != NULL)
        {
            m_ppFilters_choosen[m_filter_choosen_num] = filter_item->pItem->getNewInstance();

            if(!filter_conf.isEmpty())
            {
                m_ppFilters_choosen[m_filter_choosen_num]->selfConfig(const_cast<char *>(filter_conf.toStdString().c_str()));
            }
            else
            {
                m_ppFilters_choosen[m_filter_choosen_num]->queryRuntimeInfo(m_filterInfo, m_filterInfo_len);
                m_ppFilters_choosen[m_filter_choosen_num]->selfConfig(m_filterInfo);
            }

            m_ppFilters_choosen[m_filter_choosen_num + 1] = NULL;
            m_filter_choosen_num++;
        }
    }

    updateChoosenFilterList();

    emit notifyMainWndApply();
}

IImage_Filter **FilterWnd::getEnableFilterList()
{
    return m_ppFilters_choosen;
}

void FilterWnd::onDown()
{
    QList<QListWidgetItem*> list = ui->listWidge_choosen->selectedItems();

    if(list.size() == 0)
        return;

    QListWidgetItem* sel = list[0];
    if (sel)
    {
        int r = ui->listWidge_choosen->row(sel);
        int max = ui->listWidge_choosen->count();
        if(r < max - 1)
        {
            ui->listWidge_choosen->takeItem(r);
            ui->listWidge_choosen->insertItem(r + 1, sel);
            ui->listWidge_choosen->setCurrentRow(r + 1);
        }
    }
}

void FilterWnd::onRemove()
{
    QList<QListWidgetItem*> list = ui->listWidge_choosen->selectedItems();

    if(list.size() == 0)
        return;

    QListWidgetItem* sel = list[0];
    if (sel)
    {
        int r = ui->listWidge_choosen->row(sel);
        ui->listWidge_choosen->takeItem(r);
    }
}

void FilterWnd::onClickedChoosenFilter()
{
    QList<QListWidgetItem*> list = ui->listWidge_choosen->selectedItems();

    if(list.size() == 0)
        return;

    QListWidgetItem* itemChoosen = list[0];
    clearProperty();
    if(m_filter_info.find(itemChoosen->text()) != m_filter_info.end())
    {
        updateProperty(const_cast<char *>(m_filter_info[itemChoosen->text()].filter_conf.toStdString().c_str()));
    }
    else if(m_ppFilters)
    {
        for(int i = 0;;i++)
        {
            if(m_ppFilters[i] == NULL)
                break;
            const char *name = m_ppFilters[i]->pItem->getName();
            Module_Info *pInfo = new Module_Info;
            memset(pInfo, 0, sizeof(Module_Info));
            if(pInfo == NULL)
            {
                return;
            }
            m_ppFilters[i]->pItem->getModuleInfo(pInfo);

            if(itemChoosen->text() == QString(name))
            {
                m_ppFilters[i]->pItem->queryRuntimeInfo(m_filterInfo, m_filterInfo_len);
                updateProperty(m_filterInfo);
            }

            delete pInfo;
        }
    }
}

void FilterWnd::setAProperty(TXMLHandlePtr ptHan, QString propertyName, QString valStr)
{
    TXMLHandlePtr ptHanLoad;
    TXMLElePtr tLabel, tParaInfo, t1, t2;

    t1 = XMLGetRootElement(ptHan, "filterinfo");
    t2 = XMLGetFirstChildElement(t1, "para");

    for(TXMLElePtr tLabel = XMLGetFirstChildElement(t2, "label");
        tLabel != NULL;
        tLabel = XMLGetNextSiblingElement(tLabel))
    {
        const char *ele_name = XMLGetEleName(tLabel);
        if(QString(ele_name) == QString("label"))
        {
            for(TXMLElePtr tParaInfo = XMLGetFirstChildElement(tLabel, "parainfo");
                tParaInfo != NULL;
                tParaInfo = XMLGetNextSiblingElement(tParaInfo))
            {
                const char *ele_name = XMLGetEleName(tParaInfo);
                if(QString(ele_name) == QString("parainfo"))
                {
                    const char *type = XMLGetElementAttributes8(tParaInfo, "type");
                    const char *name = XMLGetElementAttributes8(tParaInfo, "name");
                    if (type != NULL && name != NULL)
                    {
                        if (QString(type) == QString("float") && QString(name) == propertyName)
                        {
                            TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                            float val = valStr.toFloat();
                            XMLSetElementAttributef32(tVal, "value", val);
                        }
                        else if (QString(type) == QString("int") && QString(name) == propertyName)
                        {
                            TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                            int val = valStr.toInt();
                            XMLSetElementAttributel32(tVal, "value", val);
                        }
                    }
                }
            }
        }
    }

}

void FilterWnd::getAProperty(TXMLElePtr tIn)
{
    TXMLElePtr t1, t2, t3, t4, t5, t6, t7;
    int idx;
    int i = 0;

    t1 = XMLGetFirstChildElement(tIn, "showname");
    QtProperty *topItem = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QLatin1String(XMLGetElementText(t1)));
    QtVariantProperty *item;

    XMLGetElementAttributel32(tIn, "index", &idx);

    for(TXMLElePtr tParaInfo = XMLGetFirstChildElement(tIn, "parainfo");
        tParaInfo != NULL;
        tParaInfo = XMLGetNextSiblingElement(tParaInfo))
    {
        const char *ele_name = XMLGetEleName(tParaInfo);
        if(QString(ele_name) == QString("parainfo"))
        {
            const char *type = XMLGetElementAttributes8(tParaInfo, "type");
            const char *name = XMLGetElementAttributes8(tParaInfo, "name");
            if (type != NULL && name != NULL)
            {
                if(QString(type) == QString("list"))
                {
                    item = m_variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), QLatin1String(name));
                    QStringList enumNames;

                    for(TXMLElePtr tChild = XMLGetFirstChildElement(tParaInfo, "list"); tChild != NULL; tChild = XMLGetNextSiblingElement(tChild))
                    {
                        const char *ele_name = XMLGetEleName(tChild);
                        if(QString(ele_name) == QString("list"))
                        {
                            const char *name = XMLGetElementAttributes8(tChild, "name");
                            enumNames << name;
                        }
                    }

                    item->setAttribute(QLatin1String("enumNames"), enumNames);
                    item->setValue(1);
                    topItem->addSubProperty(item);
                    m_property_dic[QString(name)] =item;
                }
                else if (QString(type) == QString("int"))
                {
                    TXMLElePtr tMin = XMLGetFirstChildElement(tParaInfo, "min");
                    TXMLElePtr tMax = XMLGetFirstChildElement(tParaInfo, "max");
                    TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                    TXMLElePtr tVisibility = XMLGetFirstChildElement(tParaInfo, "visibility");
                    TXMLElePtr tName = XMLGetFirstChildElement(tParaInfo, "showname");
                    int min, max, val, visibility_flag;
                    XMLGetElementAttributel32(tMin, "value", &min);
                    XMLGetElementAttributel32(tMax, "value", &max);
                    XMLGetElementAttributel32(tVal, "value", &val);
                    XMLGetElementAttributel32(tVisibility, "value", &visibility_flag);
                    const char *name_str = XMLGetElementText(tName);


                    item = m_variantManager->addProperty(QVariant::Int, QLatin1String(name_str));
                    item->setValue(val);
                    item->setAttribute(QLatin1String("minimum"), min);
                    item->setAttribute(QLatin1String("maximum"), max);
                    item->setAttribute(QLatin1String("singleStep"), 1);
                    topItem->addSubProperty(item);
                    m_property_dic[QString(name)] =item;
                }
                else if (QString(type) == QString("float"))
                {
                    TXMLElePtr tMin = XMLGetFirstChildElement(tParaInfo, "min");
                    TXMLElePtr tMax = XMLGetFirstChildElement(tParaInfo, "max");
                    TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                    TXMLElePtr tVisibility = XMLGetFirstChildElement(tParaInfo, "visibility");
                    TXMLElePtr tName = XMLGetFirstChildElement(tParaInfo, "showname");
                    float min, max, val, visibility_flag;
                    XMLGetElementAttributef32(tMin, "value", &min);
                    XMLGetElementAttributef32(tMax, "value", &max);
                    XMLGetElementAttributef32(tVal, "value", &val);
                    XMLGetElementAttributef32(tVisibility, "value", &visibility_flag);
                    const char *name_str = XMLGetElementText(tName);

                    item = m_variantManager->addProperty(QVariant::Double, QLatin1String(name_str));
                    item->setValue(val);
                    item->setAttribute(QLatin1String("minimum"), min);
                    item->setAttribute(QLatin1String("maximum"), max);
                    item->setAttribute(QLatin1String("singleStep"), 0.01);
                    item->setAttribute(QLatin1String("decimals"), 3);
                    topItem->addSubProperty(item);
                    m_property_dic[QString(name)] =item;
                }
            }
        }
    }

    m_variantEditor->addProperty(topItem);
}

void FilterWnd::clearProperty()
{
    m_variantEditor->clear();
    m_property_dic.clear();
}

void FilterWnd::updateProperty(char *strIn)
{
    TXMLHandlePtr ptHan, ptHanLoad;
    TXMLElePtr t1, t2, t3, t4, t5, t6,t7;

    ptHan = XMLParseOpenFromStr(strIn);
    t1 = XMLGetRootElement(ptHan, "filterinfo");
    t2 = XMLGetFirstChildElement(t1, "para");

    for(TXMLElePtr tLabel = XMLGetFirstChildElement(t2, "label");
        tLabel != NULL;
        tLabel = XMLGetNextSiblingElement(tLabel))
    {
        const char *ele_name = XMLGetEleName(tLabel);
        if(QString(ele_name) == QString("label"))
        {
            getAProperty(tLabel);
        }
    }


    m_variantEditor->setFactoryForManager(m_variantManager, m_variantFactory);
    m_variantEditor->setPropertiesWithoutValueMarked(true);
    m_variantEditor->setRootIsDecorated(false);
    m_variantEditor->update();
}

int  FilterWnd::setState(int state)
{
    int enable;
    if(state)
    {
        // when playing, set some button disabled.
        enable = true;
        enable = false;
        ui->pushButton_enable->setEnabled(enable);
        ui->pushButton_disable->setEnabled(enable);
    }
    else
    {
        enable = true;
        ui->pushButton_enable->setEnabled(enable);
        ui->pushButton_disable->setEnabled(enable);
        enable = false;
    }
    return 1;
}

