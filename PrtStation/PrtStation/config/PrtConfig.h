#ifndef _PRT_CONFIG_H
#define _PRT_CONFIG_H

#include "icdataFormat.h"
#include "placeCode.h"
#include "printerConfig.h"
#include "regConfig.h"
#include "tempConfig.h"

#define PRT_CFG_STATION_FILE "config\\station\\station.ini"

#define PRT_CFG_REGISTER_FILE   "GYregister.ini"
#define PRT_CFG_PRINTER_FILE  "GYprinter.ini"
#define PRT_CFG_TEMPLATE_FILE  "GYtemplate.ini"

#define PRT_CFG_ICDATAFMT_FILE  "config\\format\\GYicdataFormat.xml"
#define PRT_CFG_AUTOFILLFMT_FILE  "config\\format\\GYautoFillFormat.xml"

#define PRT_CFG_PLACE_FILE  "config\\bianma\\GYPlaceCode.xml"
#define PRT_CFG_BANK_FILE  "config\\bianma\\GYBankCode.xml"
#define PRT_CFG_BANKWD_FILE  "config\\bianma\\GYBankWangdianCode.xml"
#define PRT_CFG_MINGZU_FILE  "config\\bianma\\GYMingzuCode.xml"
#define PRT_CFG_XINGZHENG_FILE  "config\\bianma\\GYZhengjianCode.xml"


extern char g_configTemplateDir[MAX_PATH + 1];
extern char g_configIcDataFmtDir[MAX_PATH + 1];
extern char g_configAutoFillDir[MAX_PATH + 1];


extern char g_configPlaceCodeDir[MAX_PATH + 1];
extern char g_configBankCodeDir[MAX_PATH + 1];
extern char g_configBankWangdianCodeDir[MAX_PATH + 1];
extern char g_configMingzuCodeDir[MAX_PATH + 1];
extern char g_configZengjianCodeDir[MAX_PATH + 1];

/*是否使用代理*/
extern bool g_is_use_proxy;


int sysConfigInit(void);
void sysConfigUninit(void);

int sysConfigReload(char *stationType);

HWND get_proc_hwnd(const char *proc_name);
#endif
