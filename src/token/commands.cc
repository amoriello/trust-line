//  Author Amoriello Hutti - 2015

#include <token/commands.h>

#include <Arduino.h>

#ifndef TEST_NO_KEYBOARD
  //#include <Keyboard.h>
  //#include <HID.h>
  #include <HID-Project.h>
#endif  // TEST_NO_KEYBOARD

#include <token/token.h>
#include <token/password.h>
#include <token/crypto.h>
#include <token/communication/channel.h>
#include <token/log.h>


CmdProcessor g_cmd_registrer[cmdid::kNbCmd];



void Pair(const Command& cmd) {
  Response resp;

  g_token.Pair();

  TLOG2("Pair");

  const SymKey& pass_key = g_token.PassKey();
  const SymKey& cr_key = g_token.CRKey();
  const SymKey& req_key = g_token.ReqKey();

  resp.hdr.id = respid::kOk;
  uint8_t key_size = sizeof(pass_key.data);
  resp.hdr.arg_size = 3 * key_size;

  for (int i = 0; i < key_size; ++i) {
    resp.arg[i] = pass_key.data[i];
    resp.arg[16 + i] = cr_key.data[i];
    resp.arg[32 + i] = req_key.data[i];
  }

  g_chan.WriteResponse(resp);
}


void CreateChallenge(const Command& cmd) {
  Response resp;

  TLOG2("Challenge");

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = sizeof(Challenge);

  Challenge* p_challenge = (Challenge*)(&resp.arg);
  CreateChallenge(p_challenge);

  g_chan.WriteResponse(resp);
}


void CreatePassword(const Command& cmd) {
  Response resp;
  Password pass;

  TLOG2("CreatePassword");

  uint8_t size = cmd.arg[0];
  if (size > Password::kMaxSize) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return;
  }

  CreatePassword(&pass, size);
  
  CipherPassword(pass, g_token.PassKey(), resp.arg);

  resp.hdr.id = respid::kOk;
  // IV + 4 * 16 bytes
  resp.hdr.arg_size = 80;

  g_chan.WriteResponse(resp);
}


void TypePassword(const Command& cmd) {
  Response resp;

  TLOG2("TypePassword");

  if (cmd.hdr.arg_size < 80 || cmd.hdr.arg_size > 81) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return;    
  }

  DecipherPassword(cmd.arg, g_token.PassKey(), (Password*)&resp.arg);

  char* p_password = (char*)&resp.arg[1];

#ifndef TEST_NO_KEYBOARD
  Keyboard.print(p_password);

  if (cmd.hdr.arg_size == 81) {  // Need to press enter?
    Keyboard.press(KEY_ENTER);
    Keyboard.releaseAll();
  }
#endif  // TEST_NO_KEYBOARD

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;
  g_chan.WriteResponse(resp);
}


void LockComputer(const Command& cmd) {
  Response resp;

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.release('l');

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;
  g_chan.WriteResponse(resp);
}


void TypeString(const Command& cmd) {
  Response resp;

  if (cmd.hdr.arg_size >= 100) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return; 
  }

  char* string = (char*)cmd.arg;

#ifndef TEST_NO_KEYBOARD
  Keyboard.print(string);

  //length will contains string length + 1
  uint8_t length = 0;
  while (string[length++]);

  // Check for additional key
  if (cmd.hdr.arg_size > length) {
    uint8_t nbAdditionnalKeyPress = cmd.hdr.arg_size - length;
    uint8_t beginKeyIndex = cmd.hdr.arg_size - nbAdditionnalKeyPress;
    for (uint8_t i = 0; i < nbAdditionnalKeyPress; ++i) {
      Keyboard.press(cmd.arg[beginKeyIndex + i]);
      Keyboard.releaseAll();
    }
  }
#endif  // TEST_NO_KEYBOARD



  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;
  g_chan.WriteResponse(resp);
}


void ReturnPassword(const Command& cmd) {
  Response resp;
  Password pass;
  TLOG2("ReturnPassword");

  if (cmd.hdr.arg_size != 80) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);    
  }

  DecipherPassword(cmd.arg, g_token.PassKey(), &pass);

  CipherPassword(pass, g_token.ReqKey(), resp.arg);

  resp.hdr.id = respid::kOk;
  // IV + 4 * 16 bytes
  resp.hdr.arg_size = 80;

  g_chan.WriteResponse(resp);
}


void Reset(const Command& cmd) {
  Response resp;

  g_token.Reset();

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;

  g_chan.WriteResponse(resp);
}

namespace /* anonymous */ {

void RegistrerCommands() {
  g_cmd_registrer[cmdid::kPair] = &Pair;
  g_cmd_registrer[cmdid::kCreateChallenge] = &CreateChallenge;
  g_cmd_registrer[cmdid::kReset] = &Reset;  

  g_cmd_registrer[cmdid::kCreatePassword] = &CreatePassword;
  g_cmd_registrer[cmdid::kTypePassword] = &TypePassword;
  g_cmd_registrer[cmdid::kReturnPassword] = &ReturnPassword;
  g_cmd_registrer[cmdid::kLockComputer] = &LockComputer;
  g_cmd_registrer[cmdid::kTypeString] = &TypeString;
}


struct AutoCmdRegistrer {
  AutoCmdRegistrer() {
    RegistrerCommands();
  }
};

static AutoCmdRegistrer registrer2;

}  // namespace anonymous