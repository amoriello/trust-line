//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_COMMUNICATION_CHANNEL_H_
#define SRC_TOKEN_COMMUNICATION_CHANNEL_H_

#include <token/communication/serial.h>
#include <token/communication/ble.h>

#ifdef USE_SERIAL_COM
 extern TokenSerialChan& g_chan;
#else
 extern TokenBleChan& g_chan;
#endif

#endif  // SRC_TOKEN_COMMUNICATION_CHANNEL_H_
