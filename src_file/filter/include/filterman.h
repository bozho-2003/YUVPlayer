//

#ifndef FILTERMAN_H_
#define FILTERMAN_H_

#include "format.h"
#include <QString>





class CFilterItem{
public:
	int flag;
	IImage_Filter *pItem;
    QString path;
};

typedef int (*image_filter_get_interface_func)(void **ppIntf);

class CFilterItemInner{
public:
    void *m_hDLL;
	image_filter_get_interface_func pf_get_interface;
    QString path;
};

class CFilterMan{
	CFilterItemInner *m_pItems;
	int m_num;
	int m_size;
	int m_initialized;	
	
    QString m_prefix;
	
	int expand();

public:
	CFilterMan();
	~CFilterMan();

	int beInitialized();
	int init(const char *path);

	int getNum();
	CFilterItem **getList();

	static int destroyList(CFilterItem **list);
	int destroy();
};


extern CFilterMan gbl_fielter_nam;



#endif



