//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_LOG_H_
#define SRC_TOKEN_LOG_H_

#include <token/log/logger.h>

#ifdef ENABLE_TLOG
  #define TLOG_INIT() g_log.Intialize()
  #define TLOG(...) g_log.println(__VA_ARGS__)
  #define TLOG_(...) g_log.print(__VA_ARGS__)
# else
  #define TLOG_INIT()
  #define TLOG(...)
  #define TLOG_(...)
#endif  //  ENABLE_TLOG


#endif  // SRC_TOKEN_LOG_H_
