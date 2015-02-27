//  Author Amoriello Hutti - 2015

#include <token/communication/channel.h>

#ifdef USE_SERIAL_COM
  TokenSerialChan& g_chan = g_serial_chan;
#else
  TokenBleChan& g_chan =  g_ble_chan;
#endif