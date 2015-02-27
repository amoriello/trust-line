//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_COMMUNICATION_BLE_CHANNEL_H_
#define SRC_TOKEN_COMMUNICATION_BLE_CHANNEL_H_

#include <Stream.h>
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>

#include <token/communication/protocol.h>


class BleChannel {
 private:
  class BleStream : public Stream {
   public:
    void Initialize() {
      char token_name[] = "token";
      ble_set_name(token_name);
      ble_begin();
    }

    void ProcessEvents() {
      ble_do_events();
    }

   public:
    int available() {
      return ble_available();
    }

    int read() {
      return ble_read();
    }

    int peek() {
      // not implemented yet
      return -1;
    };

    void flush() {
      // not implemented yet
      return;
    }

    size_t write(uint8_t byte) {
      ble_write(byte);
      return 1;
    }
  };

 public:
  void Initialize() {
    ble_.Initialize();
  }

  uint8_t Available() {
    return ble_.available();
  }

  void ProcessEvents() {
    ble_.ProcessEvents();
  }

  uint8_t read() {
    ble_.ProcessEvents();
    return ble_.read();
  }

  uint8_t readBytes(char* data, uint8_t size) {
    ble_.ProcessEvents();
    return ble_.readBytes(data, size);
  }

  uint8_t write(uint8_t byte) {
    ble_.write(byte);
    ble_.ProcessEvents();
    return 1;
  }

  uint8_t writeBytes(const char* p_data, uint8_t size) {
    for (uint8_t i = 0; i < size; ++i) {
      ble_.write(p_data[i]);
    }
    ble_.ProcessEvents();
    return size;
  }

private:
  BleStream ble_;
};

typedef TokenProtocol<BleChannel> TokenBleChan;

extern TokenBleChan g_ble_chan;

#endif  // SRC_TOKEN_COMMUNICATION_BLE_CHANNEL_H_