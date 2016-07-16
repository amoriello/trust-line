### DISCLAIMER
### This is an example Makefile and it MUST be configured to suit your needs.
### For detailled explanations about all the avalaible options,
### please refer to https://github.com/sudar/Arduino-Makefile/blob/master/arduino-mk-vars.md

### PROJECT_DIR
### This is the path to where you have created/cloned your project
#PROJECT_DIR       		= /Users/matt/Projects/trust-line
PROJECT_DIR       		= ${PWD}/../../
TOKEN_DIR_SRC       		= ${PROJECT_DIR}/src/token
THIRD_PARTY_DIR             	= ${PROJECT_DIR}/third_party

### PROJECT_THIRD_PARTY_DIR
### This is the path for third party libraries and others...
THIRD_PARTY_LIB_DIR = ${THIRD_PARTY_DIR}/libraries

THIRD_PARTY_AES_SRCS = 						\
	${THIRD_PARTY_LIB_DIR}/AES/AES.cpp

THIRD_PARTY_ENTROPY_SRCS = 					\
	${THIRD_PARTY_LIB_DIR}/Entropy/Entropy.cpp

THIRD_PARTY_SHA256_SRCS = 					\
	${THIRD_PARTY_LIB_DIR}/sha256/sha256.cpp

THIRD_PARTY_NRF8001_SRCS =					\
	${THIRD_PARTY_LIB_DIR}/nRF8001/src/RBL_nRF8001.cpp


THIRD_PARTY_BLE_SDK_LIBDIR = ${THIRD_PARTY_LIB_DIR}/ble-sdk-arduino/libraries/BLE
THIRD_PARTY_BLE_SDK_SRCS = $(wildcard ${THIRD_PARTY_BLE_SDK_LIBDIR}/*.cpp)


THIRD_PARTY_HID_SRCS =  $(wildcard ${THIRD_PARTY_LIB_DIR}/HID/src/MultiReport/*.cpp)
THIRD_PARTY_HID_SRCS += $(wildcard ${${THIRD_PARTY_LIB_DIR}/HID/src/SingleReport/*.cpp)
THIRD_PARTY_HID_INC_DIR =				\
	${THIRD_PARTY_LIB_DIR}/HID/			\
	-I${THIRD_PARTY_LIB_DIR}/HID/src 	\
	-I${THIRD_PARTY_LIB_DIR}/HID/src/SingleReport	\
	-I${THIRD_PARTY_LIB_DIR}/HID/src/MultiReport




THIRD_PARTY_LIB_INCLUDES = \
		-I${THIRD_PARTY_LIB_DIR}/AES/ 				\
		-I${THIRD_PARTY_LIB_DIR}/Entropy/			\
		-I${THIRD_PARTY_LIB_DIR}/sha256/			\
		-I${THIRD_PARTY_LIB_DIR}/nRF8001/src/			\
		-I${THIRD_PARTY_BLE_SDK_LIBDIR}/			\
		-I${THIRD_PARTY_HID_INC_DIR}



LOCAL_CC_SRCS = \
	${TOKEN_DIR_SRC}/commands.cc                    \
	${TOKEN_DIR_SRC}/crypto.cc                      \
	${TOKEN_DIR_SRC}/fake_cxa_guards.cc                      \
	${TOKEN_DIR_SRC}/token.cc                       \
	${TOKEN_DIR_SRC}/main.cc                        \
                                                    \
	${TOKEN_DIR_SRC}/communication/ble.cc           \
	${TOKEN_DIR_SRC}/communication/channel.cc       \
	${TOKEN_DIR_SRC}/communication/serial.cc        \
                                                    \
	${TOKEN_DIR_SRC}/log/lcd_sink.cc                \
	${TOKEN_DIR_SRC}/log/logger.cc                  \
	${TOKEN_DIR_SRC}/log/serial_sink.cc

LOCAL_CPP_SRCS = \
	${PROJECT_DIR}/third_party/hardware/Blend/Arduino/hardware/blend/avr/variants/BlendMicro-8MHz/main.cpp


LOCAL_CPP_SRCS += \
	${THIRD_PARTY_AES_SRCS}				\
	${THIRD_PARTY_ENTROPY_SRCS}			\
	${THIRD_PARTY_SHA256_SRCS}			\
	${THIRD_PARTY_NRF8001_SRCS}			\
	${THIRD_PARTY_BLE_SDK_SRCS}			\
	${THIRD_PARTY_HID_SRCS}

ARDUINO_VERSION					= 16666

BOARD_HARDWARE_PATH = $(THIRD_PARTY_DIR)/hardware/Blend/Arduino/hardware/blend


ARDUINO_LIBS = Wire EEPROM LiquidCrystal SPI HID


### AVR_GCC_VERSION
### Check if the version is equal or higher than 4.9
AVR_GCC_VERSION  := $(shell expr `avr-gcc -dumpversion | cut -f1` \>= 4.9)

### ARDMK_DIR
### Path to the Arduino-Makefile directory.
ARDMK_DIR         = $(THIRD_PARTY_DIR)/Arduino-Makefile

### ARDUINO_DIR
### Path to the Arduino application and ressources directory.
### For Arduino IDE 1.6.x
ARDUINO_DIR       = /Applications/Arduino.app/Contents/Java

### USER_LIB_PATH
### Path to where the your project's libraries are stored.
#USER_LIB_PATH     :=  $(realpath $(PROJECT_DIR)/lib)


### BOARD_TAG & BOARD_SUB
### For Arduino IDE 1.6.x
### Both BOARD_TAG and BOARD_SUB are needed. They must be set to the board you are currently using. (i.e BOARD_TAG = uno, mega, etc. & BOARD_SUB = atmega2560, etc.)
### Note: for the Arduino Uno, only BOARD_TAG is mandatory and BOARD_SUB can be equal to anything
ARCHITECTURE  			= avr
ALTERNATE_CORE_PATH 	= $(BOARD_HARDWARE_PATH)/avr
BOARD_TAG         		= blendmicro8

### MONITOR_BAUDRATE
### It must be set to Serial baudrate value you are using.
MONITOR_BAUDRATE  = 115200

### AVR_TOOLS_DIR
### Path to the AVR tools directory such as avr-gcc, avr-g++, etc.
AVR_TOOLS_DIR     = /usr/local

NO_CORE_MAIN_CPP  = True

### AVRDDUDE
### Path to avrdude directory.
AVRDDUDE          = /usr/local/bin/avrdude

### CFLAGS_STD
#CFLAGS_STD        = -std=gnu11

### CXXFLAGS_STD
CXXFLAGS_STD      = -std=c++14

### CPPFLAGS
### Flags you might want to set for debugging purpose. Comment to stop.
CXXFLAGS         = -Wall -Wextra -I../ ${THIRD_PARTY_LIB_INCLUDES} -DTEST_RESET_ON_START
LDFLAGS 				 =

### If avr-gcc -v is higher than 4.9, activate coloring of the output
ifeq "$(AVR_GCC_VERSION)" "1"
    CXXFLAGS += -fdiagnostics-color
endif

### MONITOR_PORT
### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
MONITOR_PORT      = /dev/tty.usbmodem*

### OBJDIR
### This is were you put the binaries you just compile using 'make'
CURRENT_DIR       = $(shell basename $(CURDIR))
OBJDIR            = $(PROJECT_DIR)/bin/$(CURRENT_DIR)/$(BOARD_TAG)

### path to Arduino.mk, inside the ARDMK_DIR, don't touch.
include $(ARDMK_DIR)/Arduino.mk
