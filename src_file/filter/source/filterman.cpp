
#include "filterman.h"
#include "inner_modules.h"



CFilterMan gbl_fielter_nam;

int CFilterMan::expand(){
	CFilterItemInner *pItems = NULL;
	int sz = m_size;

	if(sz <= 0)
		sz =  16;
	else
		sz *= 2;
	
	pItems = new CFilterItemInner[sz];
	if(pItems == NULL)
		return -2;

	if(m_num > 0){
		for(int i = 0;i < m_num;i++){
            pItems[i].m_hDLL = m_pItems[i].m_hDLL;
			pItems[i].path = m_pItems[i].path;
			pItems[i].pf_get_interface = m_pItems[i].pf_get_interface;
		}

		delete m_pItems;
	}

	m_pItems = pItems;
	m_size = sz;
	return 1;
}

CFilterMan::CFilterMan(){
	m_pItems = NULL;
	m_num = 0;
	m_size = 0;
	m_initialized = 0;
}
CFilterMan::~CFilterMan(){
	destroy();
}

int CFilterMan::beInitialized(){
	return m_initialized;
}
int CFilterMan::init(const char *prefix)
{
	if(m_initialized)
		return 1;
	
	m_initialized = 1;
	
	int i;
	for(i = 0;i < inner_image_filters_num;i++){
		if(m_num >= m_size){
			if(expand() < 0)
				return -2;
		}
		m_pItems[m_num].path = "***内部插件***";
		m_pItems[m_num].m_hDLL = NULL;
		m_pItems[m_num].pf_get_interface = inner_image_filters[i];
		m_num++;
	}
#if 0
    QString  path;
    QString  path2;
	CFileFind finder;
	int bFind;

	m_prefix = prefix;
	path = m_prefix + "\\Filter_*.dll";
	bFind = finder.FindFile(path);
	while(bFind){
		HINSTANCE hDLL;
		image_filter_get_interface_func pf_func;

		bFind = finder.FindNextFile();
		path = finder.GetFilePath();
		
		// filename only, added on 2008/1030
		path2 = finder.GetFileName();

		hDLL = LoadLibrary(path);
		if(hDLL == NULL)
			continue;
		
		pf_func = (image_filter_get_interface_func)GetProcAddress(hDLL,"image_filter_get_interface");
		if(pf_func == NULL)
			continue;

		if(m_num >= m_size){
			if(expand() < 0)
				return -2;
		}

		m_pItems[m_num].path = path;
		m_pItems[m_num].pf_get_interface = pf_func;
		m_pItems[m_num].m_hDLL = hDLL;
		m_num++;
	}
#endif
	return m_num;
}

int CFilterMan::getNum(){
	return m_num;
}

CFilterItem **CFilterMan::getList(){
	if(m_num <= 0)
		return NULL;

	CFilterItem **ppItems = NULL;
	int num = m_num+1;

	ppItems = (CFilterItem**)malloc(num*sizeof(CFilterItem*));
	if(ppItems == NULL)
		return NULL;
	
	int i,j;
	for(i = 0,j = 0;i < m_num;i++){
		ppItems[j] = new CFilterItem;
		ppItems[j]->flag = 0;
		ppItems[j]->path = m_pItems[i].path;
		int ret;
		ret = m_pItems[i].pf_get_interface((void**)&ppItems[j]->pItem);
		if(ret < 0 || ppItems[j]->pItem == NULL)
			continue;
		j++;
	}

	if(j <= 0){
		free(ppItems);
		return NULL;
	}

	ppItems[j] = NULL;
	return ppItems;
}

int CFilterMan::destroyList(CFilterItem **list){
	if(list){
		int i;
		for(i = 0;;i++){
			if(list[i] == NULL)
				break;
			list[i]->pItem->destroy();
		}
		
		free(list);
	}
	return 1;
}

int CFilterMan::destroy(){
	if(m_num > 0 &&m_pItems){
		int i;
		for(i = 0;i < m_num;i++){
			if(m_pItems[i].m_hDLL)
                //FreeLibrary(m_pItems[i].m_hDLL);
                ;
		}
		delete [] m_pItems;
	}
	m_num = 0;
	return 1;
}

