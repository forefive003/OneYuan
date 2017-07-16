#pragma once

#include <set>

#define MAX_PLACE_CNT  512

#define CODE_MAX_LEN 16
#define NAME_MAX_LEN  128

typedef struct{
	char code[CODE_MAX_LEN + 1];
	char name[NAME_MAX_LEN + 1];
}PLACE_T;

typedef std::set<PLACE_T *> PlacePool;
typedef PlacePool::iterator PlacePoolItr;

class CPlaceCode
{
public:
	CPlaceCode(void);
	~CPlaceCode(void);

	int placeInit(char *file);
	char* getNameByCode(char *code);
	char* getCodeByName(char *code);

private:
	PlacePool m_places;
};


int placeCodeInit(void);

extern CPlaceCode g_placeMgr;
extern CPlaceCode g_bankMgr;
extern CPlaceCode g_bankWangdianMgr;
extern CPlaceCode g_mingzuMgr;
extern CPlaceCode g_zengjianMgr;
