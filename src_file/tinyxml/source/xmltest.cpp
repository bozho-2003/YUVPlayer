/*
   Test program for TinyXML.
*/

#include "stdio.h"
#include "uti_xmlparse.h"
#include "tinyxml.h"

void main()
{

	//
	// We start with the 'demoStart' todo list. Process it. And
	// should hopefully end up with the todo list as illustrated.
	//
	const s8* demoStart =
		//"<?xml version=\"1.0\"  standalone='no' >\n"
		"<!-- Our to do list data -->"
		"<Face>\n"
		"<!-- Do I need a secure PDA? -->\n"
		"<Para priority=\"1\" priorityD=\"1.0888\"distance='close'><bold>Toy store!</bold><name>aaa</name><item><add>kkk</add></item></Para>"
		"<Para priority=\"2\" distance='none'>Do bills</Para>"
		"</Face>";

	static s8 *ps8ResInfo =
		"<?xml version=\"1.0\"  encoding=\"utf-8\" >\
		<CIVC>\
		<command value=\"2\"/>\
		<result err=\"0\" desc=\"resultinformation\">\
		</result>\
		</CIVC>";

	TXMLHandlePtr ptXMLHanIn, ptXMLHanOut;

	TXMLHandlePtr ptHan, ptHanLoad;
	TXMLElePtr t1, t2, t3, t4, t5, t6,t7;
	const s8 *ptTmp;
	int iOut, iIn;
	double d64Out, d64In;

	void *ptOutHan;
	const s8 *ptOutStr;


	ptXMLHanOut = XMLParseOpenFromFile("f:\\MyDoc\\KedacomDoc\\ivcconfig\\AIParam.xml");
	if (ptXMLHanOut != NULL)
	{
		t1 = XMLGetRootElement(ptXMLHanOut, "algparam");
		t2 = XMLGetFirstChildElement(t1, "ivsConfigInfo");
		t3 = XMLGetFirstChildElement(t2, "ParaF");
		printf("%s", XMLGetElementText(t3));

		ptOutHan = XMLSaveToStrOpen();
		ptOutStr = XMLSaveToStrProcess(ptXMLHanOut, ptOutHan);

		XMLSaveToStrClose(ptOutHan);
		printf("%s", ptOutStr);
	}

	ptHan = XMLParseOpenFromStr(demoStart);
	t1 = XMLGetRootElement(ptHan, "Face");

	t2 = XMLGetFirstChildElement(t1, "Para");

	TiXmlElement  *ptEle = (TiXmlElement *)t2;
	printf("%s\n", ptEle->Value());

	t3 = XMLGetNextSiblingElement(t2);

	XMLSetElementAttributes8(t2, "distance", "far");
	XMLSetElementAttributel32(t2, "priority", 3);
	XMLSetElementAttributed64(t2, "priorityD", 2.0888);

	t7 = XMLNewElementPtr("Value", "2");
	printf("%s\n", XMLGetElementText(t7));

	XMLRemoveChildElement(t1, t3);
	XMLInsertElement(t1, t2, t7);

	//XMLReplaceChildElement(t1, t3, t7);
	t2 = XMLGetFirstChildElement(t1, "Para");
	t6 = XMLGetNextSiblingElement(t2);
	printf("%s\n", XMLGetElementText(t6));

	XMLSaveContentToFile(ptHan, "F:\\AControl\\1130Tinyxml\\test2.xml");
	//printf("%s\n", XMLGetElementAttributeStr(t6, "distance"));
	//XMLGetElementAttributeInt(t2, "priority", &iOut);
	//XMLGetElementAttributeD64(t2, "priorityD", &d64Out);
	
	//printf("%d\n", iOut);
	//printf("%f\n", d64Out);

	printf("%s\n", XMLGetElementAttributes8(t2, "distance"));	

	t4 = XMLGetFirstChildElement(t2, "bold");
	t5 = XMLGetFirstChildElement(t2, "name");
	
	printf("%s\n", XMLGetElementText(t1));
	printf("%s\n", XMLGetElementText(t5));

	//XMLSaveToFile("F:\\test1.xml", demoStart);

	ptTmp = (s8 *)malloc(1024);
	std::string tmp;	
	std::cout << tmp << std::endl;
	
	/*
	ptHanLoad = XMLLoadFromFile("F:\\AControl\\1130Tinyxml\\test1.xml");

	t1 = XMLGetRootElement(ptHanLoad, "Face");
	t2 = XMLGetFirstChildElement(t1, "Para");
	printf("%s\n", XMLGetElementAttributeStr(t2, "distance"));	
	*/

	XMLParseClose(&ptHan);
}
