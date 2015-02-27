//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_COMMUNICATION_SERIAL_CHANNEL_H_
#define SRC_TOKEN_COMMUNICATION_SERIAL_CHANNEL_H_

#include <Arduino.h>
#include <token/communication/protocol.h>

class SerialChannel {
 public:
  void Initialize() {
    Serial.begin(9600);
  }

  void ProcessEvents() {
    return;
  }

  uint8_t Available() {
    return Serial.available();
  }

  uint8_t read() {
    return Serial.read();
  }

  uint8_t readBytes(char* data, uint8_t size) {
    return Serial.readBytes(data, size);
  }

  uint8_t write(uint8_t byte) {
    return Serial.write(byte);
  }

  uint8_t writeBytes(const char* p_data, uint8_t size) {
    for (uint8_t i = 0; i < size; ++i) {
      Serial.write(p_data[i]);
    }
    return size;
  }
};

typedef TokenProtocol<SerialChannel> TokenSerialChan;

extern TokenSerialChan g_serial_chan;

#endif  // SRC_TOKEN_COMMUNICATION_SERIAL_CHANNEL_H_