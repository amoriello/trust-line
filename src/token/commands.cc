//  Author Amoriello Hutti - 2015

#include <token/commands.h>

#include <Arduino.h>

#ifndef TEST_NO_KEYBOARD
  //#include <Keyboard.h>
  //#include <HID.h>
  #include <HID-Project.h>
#endif  // TEST_NO_KEYBOARD

#include <token/token.h>
#include <token/crypto.h>
#include <token/communication/channel.h>
#include <token/log.h>


CmdProcessor g_cmd_registrer[cmdid::kNbCmd];


//-----------------------------------------------------------------------------
void Pair(const Command&) {
  Response resp;

  g_token.Pair();

  TLOG2("Pair");

  const auto& pass_key = g_token.PassKey();
  const auto& cr_key = g_token.CRKey();
  const auto& com_key = g_token.ComKey();

  resp.hdr.id = respid::kOk;
  uint8_t key_size = sizeof(pass_key.data);
  resp.hdr.arg_size = 3 * key_size;

  for (int i = 0; i < key_size; ++i) {
    resp.arg[i] = pass_key.data[i];
    resp.arg[16 + i] = cr_key.data[i];
    resp.arg[32 + i] = com_key.data[i];
  }

  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void CreateChallenge(const Command&) {
  Response resp;

  TLOG2("Challenge");

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = sizeof(Crypto::Challenge);

  auto p_challenge = (Crypto::Challenge*)(&resp.arg);
  CreateChallenge(p_challenge);

  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void Reset(const Command&) {
  Response resp;

  g_token.Reset();

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;

  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void CreatePassword(const Command& cmd) {
  Response resp;
  Password pass;

  TLOG2("CreatePassword");

  uint8_t size = cmd.arg[0];
  if (size > NbMaxASCIIChar<Password>::value) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return;
  }

  CreatePassword(&pass, size);
  
  uint8_t arg_size = Encrypt(pass, g_token.PassKey(), &resp.arg[0]);

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = arg_size;

  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void TypePassword(const Command& cmd) {
  Response resp;

  TLOG2("TypePassword");

  if (cmd.hdr.arg_size < 80 || cmd.hdr.arg_size > 81) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return;    
  }

  // This is valid has long has Password has 1 data member aligned
  // and packed(1)
  Password* p_pass = (Password*)resp.arg;

  Decrypt(cmd.arg, g_token.PassKey(), p_pass);


#ifndef TEST_NO_KEYBOARD
  Keyboard.print((char*)p_pass->data);

  if (cmd.hdr.arg_size == 81) {  // Need to press enter?
    Keyboard.press(KEY_ENTER);
    Keyboard.releaseAll();
  }
#endif  // TEST_NO_KEYBOARD

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;
  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void ReturnPassword(const Command& cmd) {
  Response resp;
  Password pass;

  TLOG2("ReturnPassword");

  if (cmd.hdr.arg_size != 80) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return;
  }

  Decrypt(cmd.arg, g_token.PassKey(), &pass);

  auto arg_size = Encrypt(pass, g_token.ComKey(), resp.arg);


  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = arg_size;

  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void LockComputer(const Command&) {
  Response resp;

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.release('l');

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;
  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void TypeString(const Command& cmd) {
  Response resp;
  UserString u_string;

  if (cmd.hdr.arg_size >= 100) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return; 
  }

  Decrypt(cmd.arg, g_token.ComKey(), &u_string);

#ifndef TEST_NO_KEYBOARD
  Keyboard.print((char*)u_string.data);

  if (cmd.hdr.arg_size > sizeof(UserString)) {
    auto additional_key_index = sizeof(UserString);
    auto additional_key = cmd.arg[additional_key_index];
    Keyboard.press(additional_key);
    Keyboard.releaseAll();
  }
#endif  // TEST_NO_KEYBOARD



  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;
  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void ResetKeys(const Command& cmd) {
  Response resp;
  // Version (1 byte) + 3 * 16 byte key
  if (cmd.hdr.arg_size != 49) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);    
    return;
  }

  //uint8_t version = cmd.arg[0];

  auto p_pass_key = (Crypto::SymKey*)&cmd.arg[1];
  auto p_cr_key = (Crypto::SymKey*)&cmd.arg[17];
  auto p_req_key = (Crypto::SymKey*)&cmd.arg[33];

  g_token.StoreKeys(*p_pass_key, *p_cr_key, *p_req_key);

  // The token is now paired
  g_token.Pair();

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
  g_cmd_registrer[cmdid::kResetKeys] = &ResetKeys;
}


struct AutoCmdRegistrer {
  AutoCmdRegistrer() {
    RegistrerCommands();
  }
};

static AutoCmdRegistrer registrer2;

}  // namespace anonymous