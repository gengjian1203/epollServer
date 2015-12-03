#ifndef __CCONFIG_H__
#define __CCONFIG_H__

#include "global.h"

#define SUCCESS                 0x00 /*成功*/
#define FAILURE                 0x01 /*失败*/

#define FILENAME_NOTEXIST       0x02 /*配置文件名不存在*/
#define SECTIONNAME_NOTEXIST    0x03 /*节名不存在*/
#define KEYNAME_NOTEXIST        0x04 /*键名不存在*/
#define STRING_LENNOTEQUAL      0x05 /*两个字符串长度不同*/
#define STRING_NOTEQUAL         0x06 /*两个字符串内容不相同*/
#define STRING_EQUAL            0x00 /*两个字符串内容相同*/

class CConfig
{
private:
    CConfig();
    ~CConfig();

private:
    int CompareString(char *pInStr1,char *pInStr2);
    int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue);

public:
    static CConfig* getInstance();
    int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue);
    int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue);

};

#endif // CONFIG_H
