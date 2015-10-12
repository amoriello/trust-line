#  Author Amoriello Hutti - 2015

set(Token_BOARD blendmicro8)
set(Token_PORT /dev/tty.usbmodem14131)
add_definitions(-DTEST_RESET_ON_START)


#set(Token_BOARD uno)
#set(Token_PORT /dev/tty.usbserial-DA00WTIG)
#add_definitions(-DUSE_SERIAL_COM
#                -DTEST_RESET_ON_START
#                -DTEST_NO_KEYBOARD
#                -DUSE_LCD_LOGGER)

set(Token_SRC
  commands.cc
  crypto.cc
  main.cc
  #simple_chat.cc
  password.cc
  token.cc
  log/serial_sink.cc
  log/lcd_sink.cc
  log/logger.cc
  communication/ble.cc
  communication/serial.cc
  communication/channel.cc
  tests/test_suite.cc
  )

set(Token_HDR
  commands.h
  crypto.h
  password.h
  token.h
  log/serial_sink.h
  log/lcd_sink.h
  log/logger.h
  log.h
  communication/ble.h
  communication/channel.h
  communication/serial.h
  )


file (GLOB nRF8001_files ${PROJECT_THIRD_PARTY_DIR}/libraries/BLE/*.cpp)
include_directories(${PROJECT_THIRD_PARTY_DIR}/libraries/BLE/)

generate_arduino_firmware(Token
  SRCS   ${Token_SRC} ${nRF8001_files}
  HDRS   ${Token_HDR}
  PORT   ${Token_PORT}
  BOARD  ${Token_BOARD}
  )