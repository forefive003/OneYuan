
#ifndef _PRT_BASE64_H
#define _PRT_BASE64_H


extern char * strtok_my (char *s,const char *delim);

extern char* base64encode(char *src, unsigned int srclen, unsigned int *dstlen);
extern char* base64decode(char *src, unsigned int srclen,
						char *dst, unsigned int dstMaxLen,
						unsigned int* dstlen);

void get_local_ipaddress(int *count, char **hosts_arr);

#endif

