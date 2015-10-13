//  Author Amoriello Hutti - 2015

#include <token/token.h>
#include <token/communication/channel.h>

#include <EEPROM.h>

#ifndef TEST_NO_KEYBOARD
  //#include <USBAPI.h>
  #include <Keyboard.h>
  #include <HID.h>
#endif  // TEST_NO_KEYBOARD


Token g_token;

//-----------------------------------------------------------------------------
void Token::Initialize() {
  bool first_time = false;

  if (!IsInitialized()) {
    first_time = true;
  }

  crypto_.Initialize(first_time);
  g_chan.Initialize();

#ifndef TEST_NO_KEYBOARD
  Keyboard.begin();
#endif  // TEST_NO_KEYBOARD

  SetInitializedState(true);
}


bool Token::IsInitialized() const {
  uint8_t data[3];

  ReadInitializedInfo(data);

  return data[0] == 0x42 &&
         data[1] == 0x43 &&
         data[2] == 0x44;
}


bool Token::IsPaired() const {
  uint8_t state = 0;
  ReadPairInfo(&state);

  return state == 0x51;
}


void Token::Pair() {
  SetPairedState(true);
}


void Token::Reset() {
  crypto_.Reset();

  SetInitializedState(false);
  SetPairedState(false);
}


void Token::SetInitializedState(bool initialize) {
  if (initialize) {
    EEPROM.write(2, 0x42);
    EEPROM.write(3, 0x43);
    EEPROM.write(4, 0x44);
  } else {
    EEPROM.write(2, 0x00);
    EEPROM.write(3, 0x00);
    EEPROM.write(4, 0x00);    
  }
}


void Token::SetPairedState(bool paired) {
  if (paired) {
    EEPROM.write(1, 0x51);
  } else {
    EEPROM.write(1, 0x00);
  }
}


void Token::ReadInitializedInfo(uint8_t* p_data) const {
  p_data[0] = EEPROM.read(2);
  p_data[1] = EEPROM.read(3);
  p_data[2] = EEPROM.read(4);
}


void Token::ReadPairInfo(uint8_t* p_data) const {
  *p_data = EEPROM.read(1);
}
