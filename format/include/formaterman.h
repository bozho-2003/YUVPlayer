#ifndef FORMATERMAN_H
#define FORMATERMAN_H

#include "format.h"
#include <QString>
class CFormaterItem{
public:
    int flag;
    IYUV_Formater *pItem;
    QString path;
};

typedef int (*yuv_formater_get_interface_func)(void **ppIntf);

class CFormaterItemInner{
public:
    void * m_hDLL;
    yuv_formater_get_interface_func pf_get_interface;
    QString path;
};

class CFormaterMan{
    CFormaterItemInner *m_pItems;
    int m_num;
    int m_size;
    int m_initialized;

    QString m_prefix;

    int expand();

public:
    CFormaterMan();
    ~CFormaterMan();

    int beInitialized();
    int init(const char *path);

    int getNum();
    CFormaterItem **getList();

    static int destroyList(CFormaterItem **list);
    int destroy();
};


extern CFormaterMan gbl_formater_man;


#endif // FORMATERMAN_H
