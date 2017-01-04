
#ifndef __UTI_XMLPARSE_H__
#define __UTI_XMLPARSE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef char s8;
typedef short           s16;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef float           f32;
typedef double          d64;
typedef int                l32;
typedef unsigned int       u32;

typedef int BOOL32;
typedef BOOL32 BOOL;

typedef void *TXMLElePtr;
typedef void *TXMLHandlePtr;



 TXMLHandlePtr XMLParseOpenFromStr(const s8 *ps8Str);


 TXMLHandlePtr XMLParseOpenFromFile(const s8 *ps8PathStr);


 void XMLParseClose(TXMLHandlePtr *ptHandle);


 void XMLSaveContentToFile(TXMLHandlePtr tHandle, const s8 *ps8PathStr);


 void *XMLSaveToStrOpen();


 const s8 *XMLSaveToStrProcess(TXMLHandlePtr tHandle, void *pvOut);


 void XMLSaveToStrClose(void *pvOut);


 TXMLElePtr XMLGetRootElement(TXMLHandlePtr tHandle, const s8 *ps8Str);


 TXMLElePtr XMLGetFirstChildElement(TXMLElePtr tEleIn, const s8 *ps8Str);


 TXMLElePtr XMLGetNextSiblingElement(TXMLElePtr tEleIn);


 void XMLRemoveChildElement(TXMLElePtr tEleIn, TXMLElePtr tRemove);


 void XMLReplaceChildElement(TXMLElePtr tFather, TXMLElePtr tPrevious, const TXMLElePtr tReplace);


 void XMLInsertElement(TXMLElePtr tFather, TXMLElePtr tAfter, TXMLElePtr tEleIn);


 void XMLInsertElementAsCDATA(TXMLElePtr tFather, TXMLElePtr tAfter, TXMLElePtr tEleIn);


 TXMLElePtr XMLNewElementPtr(const s8 *ps8NameStr, const s8 *ps8StrValue);


 void XMLDelElementPtr(TXMLElePtr *ptEleIn);


 const s8 *XMLGetEleName(TXMLElePtr tEleIn);


 const s8 *XMLGetElementText(TXMLElePtr tEleIn);


 const s8 *XMLGetElementAttributes8(TXMLElePtr tEleIn, const s8 *ps8StrName);


 int XMLGetElementAttributel32(TXMLElePtr tEleIn, const s8 *ps8Str, l32 *pl32Out);


 int XMLGetElementAttributed64(TXMLElePtr tEleIn, const s8 *ps8Str, d64 *pd64Out);


 int XMLGetElementAttributef32(TXMLElePtr tEleIn, const s8 *ps8Str, f32 *pf32Out);


 void XMLSetElementAttributes8(TXMLElePtr tEleIn, const s8 *ps8Str, const s8 *ps8Value);


 void XMLSetElementAttributel32(TXMLElePtr tEleIn, const s8 *ps8Str, l32 l32In);


 void XMLSetElementAttributed64(TXMLElePtr tEleIn, const s8 *ps8Str, d64 d64In);


 void XMLSetElementAttributef32(TXMLElePtr tEleIn, const s8 *ps8Str, f32 f32In);


 void XMLSetElementAttributef32Prs(TXMLElePtr tEleIn, const s8 *ps8Str, f32 f32In, const int si32Precision);


 BOOL XMLGetParamValue(TXMLElePtr ptParam, const s8 *ps8Name, void *pvValue);

#ifdef __cplusplus
}
#endif

#endif
