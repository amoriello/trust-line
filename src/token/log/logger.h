//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_LOG_LOGGER_H_
#define SRC_TOKEN_LOG_LOGGER_H_

#ifdef USE_LCD_LOGGER
  #include <token/log/lcd_sink.h>
  typedef LcdSink LoggerSink;
#else
  #include <token/log/serial_sink.h>
  typedef SerialSink LoggerSink;
#endif

template <typename Sink>
class BasicLogger {
 public:
  void Intialize() { sink_.Intialize(); }

  void print(const char* msg) { sink_.print(msg); }
  void println(const char* msg) { sink_.println(msg); }
  void print(int val, int fmt) { sink_.print(val, fmt); }
  void println(int val, int fmt) { sink_.println(val, fmt); }
  void println(const char* p_data, unsigned int size, int fmt) { sink_.println(p_data, size, fmt); }

 private:
  Sink sink_;
};

typedef BasicLogger<LoggerSink> Logger;

extern Logger g_log;

#endif  // SRC_TOKEN_LOG_LOGGER_H_