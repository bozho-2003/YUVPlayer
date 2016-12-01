#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QWidget>
#include "uti_xmlparse.h"
#include "filterman.h"
#include "ui_filter.h"
#include "qtvariantproperty.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

class FilterWnd : public QWidget
{
    Q_OBJECT
public:
    explicit FilterWnd(QWidget *parent = 0);
    ~FilterWnd();

    bool addFilterModule();
    void setFilterModule(CFilterItem **pFilters);

    void printMsg(QString msg);
    void updateProperty(char *strIn);
    void getAProperty(TXMLElePtr tIn);
    void setAProperty(TXMLHandlePtr ptHan, QString propertyName, QString valStr);

    void saveProperty(char *strIn);
    void clearProperty();
    IImage_Filter **getEnableFilterList();
    int  setState(int state);

public slots:
    void chooseItem(QListWidgetItem *);
    void onClickedChoosenFilter();

    void onClear();
    void onUp();
    void onDown();
    void onRemove();
    void onEnable();
    void onApplyProperty();

    void show();
    void hide();

signals:
    void notifyMainWndApply();

public:
    CFilterItem **m_ppFilters;
    const int filter_choosen_max_num;
    int m_filter_choosen_num;
    IImage_Filter *m_ppFilters_choosen[32];

    typedef struct
    {
        QString filter_name;
        QString filter_conf;
    }FilterInfo;
    int m_filter_info_idx;
    QMap<QString, FilterInfo> m_filter_info;

private:
    Ui::FilterWnd *ui;
    QtVariantEditorFactory *m_variantFactory;
    QtTreePropertyBrowser *m_variantEditor;
    QtVariantPropertyManager *m_variantManager;

    QMap<QString, QtProperty*> m_property_dic;
    QMap<QString, QtProperty*>::Iterator m_property_dic_iter;

    int m_filter_len;
    QString m_filter_name[1024];
    CFilterItem *m_filter_ptr[1024];

    int m_filterInfo_len = 1024 * 10;
    char m_filterInfo[1024 * 10];

    CFilterItem *getMatchedItem(QString name);
    void cleanAllChoosenFilters();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void addFilterItem(QString itemName);
    void updateChoosenFilterList();

    void clearFilterMap();
    QString addItemToFilterMap(QString filter_name, QString filter_conf, QString flag = QString());
    QString modItemToFilterMap(QString map_key, QString filter_conf);

    QString getFilterNewConf(QString confOld);

signals:
    void notifyMainUncheckedFilterCombox();
};

#endif // FILTER_H
