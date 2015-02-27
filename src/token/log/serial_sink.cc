//  Author Amoriello Hutti - 2015

#include <token/log/serial_sink.h>
#include <Arduino.h>

void SerialSink::Intialize() {
  Serial.begin(9600);
}

void SerialSink::print(const char* msg) {
  Serial.print(msg);
}

void SerialSink::println(const char* msg) {
  Serial.println(msg);
}

void SerialSink::print(int val, int fmt) {
  Serial.print(val, fmt);
}

void SerialSink::println(int val, int fmt) {
  Serial.println(val, fmt);
}