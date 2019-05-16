#ifndef LOG_H
#define LOG_H
#include <stdio.h>

#define debug_msg(fmt,...) printf("%s[%d]:"fmt,__FILE__,__LINE__,##__VA_ARGS__)
#define LOGI(fmt,...) printf("\033[32m%s[%d]:\033[0m"fmt,__FILE__,__LINE__,##__VA_ARGS__)
#define LOGE(fmt,...) printf("\033[31m%s[%d]:\033[0m"fmt,__FILE__,__LINE__,##__VA_ARGS__)
#define LOGD(fmt,...) printf("\033[33m%s[%d]:\033[0m"fmt,__FILE__,__LINE__,##__VA_ARGS__)
#endif // LOG_H
