//  Author Amoriello Hutti - 2015

#include <token/log/lcd_sink.h>
#include <Arduino.h>

LcdSink::LcdSink()
  : lcd_(12,11,5,8,3,2)
{ }

void LcdSink::Intialize() {
  lcd_.begin(16, 2);
  lcd_.clear();
}

void LcdSink::print(const char* msg) {
  lcd_.clear();
  lcd_.print(msg);
  delay(1000);
}

void LcdSink::println(const char* msg) {
  lcd_.clear();
  lcd_.println(msg);
  delay(1000);
}

void LcdSink::print(int val, int fmt) {
  lcd_.clear();
  lcd_.print(val, fmt);
  delay(1000);
}

void LcdSink::println(int val, int fmt) {
  lcd_.clear();
  lcd_.println(val, fmt);
  delay(1000);
}
