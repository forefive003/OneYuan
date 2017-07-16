#pragma once

#include "autoload.h"
#include <mshtml.h>  // 所有 IHTMLxxxx 的接口声明  

typedef enum
{
	FORM_UNUSED = 0,
	FORM_TEXT = 1,
	FORM_TEXT_AREA,
	FORM_CHK_BOX,
	FORM_RADIO,
	FORM_LIST,
	FORM_HIDDEN,

	FORM_TYPE_MAX,
}FORM_TYPE_E;

#define MAX_FIELD_FORM_NAME 64
typedef struct
{
	int formType;
	char formName[MAX_FIELD_FORM_NAME];
	char name[MAX_FIELD_NAME];
	char value[MAX_FIELD_VALUE_LEN];

	int hasHiden;

	int hidenformType;
	char hidenFormName[MAX_FIELD_FORM_NAME];
}FIELD_INFO_T;

class CBrowserOper
{
public:
	CBrowserOper();
	~CBrowserOper(void);

public:
	int init(void);
	int formScan(CString &webTitle, CString &formTitle);
	int formFieldFill(CString *strField, int size);

private:
	int fmtConfigInit();
	int getIndexByFormname(char *formname, char *formtype, int *isHidden);
	int getFormTypeByName(char *formtype);

	int m_curOperType; /*0表示读，1表示写*/
	int formReadHandle(CComDispatchDriver &spInputElement,
						LPCTSTR lpName, LPCTSTR lpType, LPCTSTR lpValue);
	int formWriteHandle(CComDispatchDriver &spInputElement,
						LPCTSTR lpName, LPCTSTR lpType, LPCTSTR lpValue);

	void EnumFrame( IHTMLDocument2 * pIHTMLDocument2);
	void EnumForm( IHTMLDocument2 * pIHTMLDocument2 );
	int EnumBrowser(void);

public:
	CString m_webTitle;
	CString m_formTitle;

	int m_fieldCnt;
	FIELD_INFO_T m_fields[ELM_MAX];
};

extern FIELD_INFO_T g_tmp_fields[ELM_MAX];
extern int g_tmp_fields_cnt;
extern int g_tmp_wr_fields_cnt;
extern CString g_tmp_webTitle;
extern CString g_tmp_formTitle;
extern char* g_form_type_desc[];