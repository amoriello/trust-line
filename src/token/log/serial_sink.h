//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_LOG_SERIAL_SINK_H_
#define SRC_TOKEN_LOG_SERIAL_SINK_H_

class SerialSink {
 public:
  void Intialize();

  void print(const char* msg);
  void println(const char* msg);
  void print(int val, int fmt);
  void println(int val, int fmt);
  void println(const char* p_data, unsigned int size, int fmt);
};

#endif  // SRC_TOKEN_LOG_SERIAL_SINK_H_