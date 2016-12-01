#include <string>
#include "tinystr.h"
#include "tinyxml.h"

#include <math.h>
#include <string.h>
#include "uti_xmlparse.h"


TXMLHandlePtr XMLParseOpenFromStr(const s8 *ps8Str)
{
	if(ps8Str == NULL)
	{
		return NULL;
	}

	TiXmlDocument *ptdc;
	ptdc = new TiXmlDocument;
	ptdc->Parse(ps8Str);
	if ( ptdc->Error() )
	{
		printf( "Error in %s: %s\n", ptdc->Value(), ptdc->ErrorDesc() );
        delete ptdc;
		return NULL;
	}
	return ptdc;
}

TXMLHandlePtr XMLParseOpenFromFile(const s8 *ps8PathStr)
{	
	TiXmlDocument *ptdc = NULL;
	TiXmlDocument doc( ps8PathStr );
	bool loadOkay = doc.LoadFile();

	if ( loadOkay )
	{
		ptdc = new TiXmlDocument;
		ptdc->operator =(doc);
	}
	else
	{
		printf( "Could not load test file. Error='%s'. Exiting.\n", doc.ErrorDesc() );		
	}

	return ptdc;
}


void XMLParseClose(TXMLHandlePtr *ptHandle)
{
	if (ptHandle == NULL)
	{
		return;
	}
	TiXmlDocument *ptdc = (TiXmlDocument *)(*ptHandle);
	delete ptdc;
	*ptHandle = NULL;		
}


void XMLSaveContentToFile(TXMLHandlePtr tHandle, const s8 *ps8Pathstr)
{
	if ((tHandle == NULL) || (ps8Pathstr == NULL))
	{
		return;
	}

	TiXmlDocument *ptdc = (TiXmlDocument *)(tHandle);
	ptdc->SaveFile(ps8Pathstr);
}


void *XMLSaveToStrOpen()
{
    std::string *pStr = new std::string();
	return pStr;
}


const s8 *XMLSaveToStrProcess(TXMLHandlePtr tHandle, void *pvOut)
{
	if (pvOut == NULL)
	{
		return NULL;
	}

	std::string *pStr = (std::string *)pvOut;
	TiXmlNode* node = (TiXmlNode *)tHandle;

    std::string str;

    str << (*node);
    memcpy(pvOut, str.c_str(), str.length());
    //return pStr->c_str();
    return (const s8 *)pvOut;
}


void XMLSaveToStrClose(void *pvOut)
{
	if (pvOut == NULL)
	{
		return;
	}
	std::string *pStr = (std::string *)pvOut;
	delete pStr;	
}


TXMLElePtr XMLGetRootElement(TXMLHandlePtr tHandle, const s8 *ps8str)
{
	if (tHandle == NULL)
	{
		return NULL;
	}
	TiXmlDocument *ptdc = (TiXmlDocument *)tHandle;
	TiXmlElement *pTiXmlEle;
	if(ps8str != NULL)
        pTiXmlEle = ptdc->FirstChildElement(ps8str);
    else
        pTiXmlEle = ptdc->FirstChildElement();
	return pTiXmlEle;
}

TXMLElePtr XMLGetFirstChildElement(TXMLElePtr tEleIn, const s8 *ps8str)
{
	if (tEleIn == NULL)
	{
		return NULL;
	}
	TiXmlElement *pTiXmlEle;
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	if (ps8str != NULL)
	{
		pTiXmlEle = pTiXmlEleIn->FirstChildElement(ps8str);
	}
	else
	{
		pTiXmlEle = pTiXmlEleIn->FirstChildElement();
	}
	
	return pTiXmlEle;
}

TXMLElePtr XMLGetNextSiblingElement(TXMLElePtr tEleIn)
{
	if (tEleIn == NULL)
	{
		return NULL;
	}
	TiXmlElement *pTiXmlEle;
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	pTiXmlEle = pTiXmlEleIn->NextSiblingElement();
	return pTiXmlEle;
}

//ptIn或ptRemove节点为空，什么都不做

void XMLRemoveChildElement(TXMLElePtr tEleIn, TXMLElePtr tRemove)
{
	if (tEleIn == NULL)
	{
		return;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	if(tRemove == NULL)
	{
		return;
	}
	TiXmlElement *pTiXmlEleRemove = (TiXmlElement *)tRemove;
	pTiXmlEleIn->RemoveChild(pTiXmlEleRemove);	
}

void XMLReplaceChildElement(TXMLElePtr tFather, TXMLElePtr tPrevious, const TXMLElePtr tReplace)
{
	if ((tFather == NULL) || (tPrevious == NULL) || (tReplace == NULL))
	{
		return;
	}
	
	TiXmlElement *pTiXmlEleFather = (TiXmlElement *)tFather;
	TiXmlElement *pTiXmlElePre = (TiXmlElement *)tPrevious;
	pTiXmlEleFather->ReplaceChild(pTiXmlElePre, *((TiXmlElement *)tReplace));
}


void XMLInsertElement(TXMLElePtr tFather, TXMLElePtr tAfter, TXMLElePtr tEleIn)
{
	if ((tFather == NULL) || (tEleIn == NULL))
	{
		return;		
	}
	TiXmlElement *pTiXmlEle = (TiXmlElement *)tFather;
	if (tAfter == NULL)
	{
		pTiXmlEle->InsertEndChild(*(TiXmlElement *)tEleIn);
	}
	else
	{
		TiXmlElement *pTiXmlElePre = (TiXmlElement *)tAfter;
		pTiXmlEle->InsertAfterChild(pTiXmlElePre, *((TiXmlElement *)tEleIn));
	}
}

void XMLInsertElementAsCDATA(TXMLElePtr tFather, TXMLElePtr tAfter, TXMLElePtr tEleIn)
{
	if ((tFather == NULL) || (tEleIn == NULL))
	{
		return;		
	}

	void *ptVoid = NULL;
	ptVoid = XMLSaveToStrOpen();
	if (ptVoid != NULL)
	{
		const char *ptOutStr;
		ptOutStr = XMLSaveToStrProcess(tEleIn, ptVoid);
		TiXmlText* text = new TiXmlText(ptOutStr);
		text->SetCDATA( true );	
		XMLInsertElement(tFather, tAfter, text);
		XMLSaveToStrClose(ptVoid); 
		delete text;
	}
	return;
}


TXMLElePtr XMLNewElementPtr(const s8 *ps8NameStr, const s8 *ps8StrValue)
{
	if (ps8NameStr == NULL)
	{
		return NULL;
	}
	TiXmlElement *NewEle = new TiXmlElement(ps8NameStr);
	if (ps8StrValue == NULL)
	{
		return NewEle;
	}
	TiXmlText NewText(ps8StrValue);
	NewEle->InsertEndChild(NewText);	
	return NewEle;
}


void XMLDelElementPtr(TXMLElePtr *tEleIn)
{
	if (tEleIn == NULL)
	{
		return;
	}
	TiXmlElement *pTiXmlNodeIn = (TiXmlElement *)(*tEleIn);
	delete pTiXmlNodeIn;
	tEleIn = NULL;	
}


const s8 *XMLGetEleName(TXMLElePtr tEleIn)
{
	if (tEleIn == NULL)
	{
		return NULL;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	return pTiXmlEleIn->Value();
}

const s8 *XMLGetElementText(TXMLElePtr tEleIn)
{
	if(tEleIn == NULL)
	{
		return NULL;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	return pTiXmlEleIn->GetText();
}

const s8 *XMLGetElementAttributes8(TXMLElePtr tEleIn, const s8 *ps8strName)
{
	if((tEleIn == NULL) || (ps8strName == NULL))
	{
		return NULL;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	return pTiXmlEleIn->Attribute(ps8strName);	
};

int XMLGetElementAttributel32(TXMLElePtr tEleIn, const s8 *ps8Str, l32 *pl32Out)
{
	int l32Tmp;
	if((tEleIn == NULL) || (ps8Str == NULL) || (pl32Out == NULL))
	{
		return 0;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	if (pTiXmlEleIn->Attribute(ps8Str, &l32Tmp))
	{
		*pl32Out = (l32)l32Tmp;
		return 1;
	}	
	return 0;
};

int XMLGetElementAttributed64(TXMLElePtr tEleIn, const s8 *ps8Str, d64 *pd64Out)
{
	if((tEleIn == NULL) || (ps8Str == NULL) || (pd64Out == NULL))
	{
		return 0;		
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	if (pTiXmlEleIn->Attribute(ps8Str, pd64Out))
	{
		return 1;
	}	
	return 0;
};

int XMLGetElementAttributef32(TXMLElePtr tEleIn, const s8 *ps8Str, f32 *pf32Out)
{
	double d64Tmp;

	if((tEleIn == NULL) || (ps8Str == NULL) || (pf32Out == NULL))
	{
		return 0;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	if (pTiXmlEleIn->Attribute(ps8Str, &d64Tmp))
	{
		*pf32Out = (float)d64Tmp;
		return 1;
	}
	return 0;
};

void XMLSetElementAttributes8(TXMLElePtr tEleIn, const s8 *ps8Str, const s8 *ps8Value)
{
	if((tEleIn == NULL) || (ps8Str == NULL) || (ps8Value == NULL))
	{
		return;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	pTiXmlEleIn->SetAttribute(ps8Str, ps8Value);
	return;
}

void XMLSetElementAttributel32(TXMLElePtr tEleIn, const s8 *ps8Str, l32 l32In)
{
	if((tEleIn == NULL) || (ps8Str == NULL))
	{
		return;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	pTiXmlEleIn->SetAttribute(ps8Str, l32In);
	return;
}

void XMLSetElementAttributed64(TXMLElePtr tEleIn, const s8 *ps8Str, d64 d64In)
{
	if((tEleIn == NULL) || (ps8Str == NULL))
	{
		return;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	pTiXmlEleIn->SetDoubleAttribute(ps8Str, d64In);
	return;
}

void XMLSetElementAttributef32(TXMLElePtr tEleIn, const s8 *ps8Str, f32 f32In)
{
	double d64Tmp;
	d64Tmp = (double)f32In;
	if((tEleIn == NULL) || (ps8Str == NULL))
	{
		return;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	pTiXmlEleIn->SetDoubleAttribute(ps8Str, d64Tmp);
	return;
}


void XMLSetElementAttributef32Prs(TXMLElePtr tEleIn, const s8 *ps8Str, f32 f32In, const int si32Precision)
{
	double d64Tmp;
	d64Tmp = (double)f32In;
	if((tEleIn == NULL) || (ps8Str == NULL))
	{
		return;
	}
	TiXmlElement *pTiXmlEleIn = (TiXmlElement *)tEleIn;
	pTiXmlEleIn->SetDoubleAttribute(ps8Str, d64Tmp, si32Precision);
	return;
}


BOOL XMLGetParamValue(TXMLElePtr ptParam, const s8 *ps8Name, void *pvValue)
{
    TXMLElePtr ptBase;
    TXMLElePtr ptAdvanced;
    TXMLElePtr ptParamInfo;
    TXMLElePtr ptValue;
    l32 l32Idx;
    const s8 *ps8CurTagName;
    BOOL bFind = false;

    static const s8 *aps8ParamNames[] =
    {
        "BaseInfo",
        "AdvancedInfo"
    };

    if(ptParam == 0 || ps8Name == 0 || pvValue == 0)
    {
        printf("输入参数错误.\n");
        return false;
    }

    for(l32Idx = 0; l32Idx < sizeof(aps8ParamNames) / sizeof(aps8ParamNames[0]); l32Idx++)
    {
        ptBase = XMLGetFirstChildElement(ptParam, aps8ParamNames[l32Idx]);
        if(ptBase == 0)
        {
            printf("未找到BaseInfo标签.\n");
            return false;
        }
        // 第一个参数
        ptParamInfo = XMLGetFirstChildElement(ptBase, "ParaInfo");
        // 遍历所有的参数
        while(ptParamInfo)
        {
            ps8CurTagName = XMLGetElementAttributes8(ptParamInfo, "name");
            if(strcmp(ps8CurTagName, ps8Name) == 0)
            {
                bFind = true;
                break;
            }
            ptParamInfo = XMLGetNextSiblingElement(ptParamInfo);
        }
        if(bFind)
        {
            break;
        }
    }

    // 解析参数
    if(bFind)
    {
        ps8CurTagName = XMLGetElementAttributes8(ptParamInfo, "type");
        ptValue = XMLGetFirstChildElement(ptParamInfo, "Value");
        if(ptValue == 0)
        {
            printf("Value标签未找到\n");
            return false;
        }

        if(strcmp(ps8CurTagName, "List") == 0)
        {
            *((l32*)pvValue) = atoi(XMLGetElementText(ptValue));
        }
        else if(strcmp(ps8CurTagName, "Int") == 0)
        {
            *((l32*)pvValue) = atoi(XMLGetElementText(ptValue));
        }
        else if(strcmp(ps8CurTagName, "Double") == 0)
        {
            *((double*)pvValue) = atof(XMLGetElementText(ptValue));
        }
        else
        {
            printf("不支持的类型'%s'\n", ps8CurTagName);
            return false;
        }
    }

    return true;
}

