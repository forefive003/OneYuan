#ifndef _STATION_CODE_H
#define _STATION_CODE_H

#define BANBEN_NUM "20170512"

#define MAX_STATION_NAME_LEN 256


#define XPS_PRINT_LIB_PATH	"lib\\XPS_DLL.dll"
#define XPS_WRITE_LIB_GUANGYUAN_PATH		"lib\\SiChuanShebao(广元).dll"
#define XPS_WRITE_LIB_MIANSHANGHANG_PATH	"lib\\SiChuanShebao(绵商行).dll"
#define XPS_WRITE_LIB_TONGYONG_PATH			"lib\\SiChuanShebao(通用).dll"



extern char g_dllPrtDir[MAX_PATH + 1];
extern char g_dllWrIcDir[MAX_PATH + 1];

extern int g_dll_cnt;
extern char g_writeIcDlls[3][64];
extern char g_writeIcDllTypes[64];

UINT32 prtGetStationNum();
UINT32 prtGetCurSerailNum();
char* prtGetStationType();
int prtSetStationType(char *stationType);
int prtSetWrDllType(char *wrIcDllType);

int prtStationInfoInit();
UINT32 prtGenCardIdCode();

void get_local_ipaddress();

extern int g_station_cnt;
extern char g_stations[100][MAX_STATION_NAME_LEN + 1];


extern int g_localip_cnt;
extern char g_localip_desc[10][256];

#define MAX_LINE_LEN 256
extern int g_sales_cnt;
extern char g_sales_phone[10][MAX_LINE_LEN];
#endif
