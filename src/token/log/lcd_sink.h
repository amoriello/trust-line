//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_LOG_LCD_SINK_H_
#define SRC_TOKEN_LOG_LCD_SINK_H_

#include <LiquidCrystal.h>

class LcdSink {
 public:
  LcdSink();
  void Intialize();

  void print(const char* msg);
  void println(const char* msg);
  void print(int val, int fmt);
  void println(int val, int fmt);

 private:
  LiquidCrystal lcd_;
};

#endif  // SRC_TOKEN_LOG_LCD_SINK_H_