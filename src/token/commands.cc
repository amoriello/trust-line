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

  const auto& pass_key = g_token.PassKey();
  const auto& cr_key = g_token.CRKey();
  const auto& com_key = g_token.ComKey();
  const auto& login_key = g_token.LoginKey();

  const auto key_size = sizeof(Crypto::SymKey::data);

  for (uint8_t i = 0; i < key_size; ++i) {
    resp.arg[i] = pass_key.data[i];
    resp.arg[1 * key_size + i] = cr_key.data[i];
    resp.arg[2 * key_size + i] = com_key.data[i];
    resp.arg[3 * key_size + i] = login_key.data[i];
  }

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 4 * key_size;

  g_chan.WriteResponse(resp);
}


//-----------------------------------------------------------------------------
void CreateChallenge(const Command&) {
  Response resp;

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

  auto is_token_encrypted = cmd.arg[0];

  if (is_token_encrypted) {
    // The string was token-protected (e.g login)
    Decrypt(cmd.arg, g_token.PassKey(), &u_string); 
  } else {
    // The string was "only" com-protected
    Decrypt(cmd.arg, g_token.ComKey(), &u_string);
  }

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
void EncryptUserString(const Command& cmd) {
  Response resp;
  UserString u_string;

  if (cmd.hdr.arg_size >= 100) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
    g_chan.WriteResponse(resp);
    return; 
  }

  // The clear string is com-protected
  Decrypt(cmd.arg, g_token.ComKey(), &u_string);

  // Make it token-protected, so that the App cannot decrypt it by itself.
  auto arg_size = Encrypt(u_string, g_token.PassKey(), &resp.arg[0]);

  // Send the token-protected string to the app, so it can save it on iCloud.
  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = arg_size;
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

  const uint8_t key_size = sizeof(Crypto::SymKey::data);

  // cmd.arg[0] is the version of this keymaterial
  auto p_pass_key = (Crypto::SymKey*)&cmd.arg[1];
  auto p_cr_key = (Crypto::SymKey*)&cmd.arg[1 + key_size];
  auto p_com_key = (Crypto::SymKey*)&cmd.arg[1 + (2 * key_size)];
  auto p_login_key = (Crypto::SymKey*)&cmd.arg[1 + (3 * key_size)];

  g_token.StoreKeys(*p_pass_key, *p_cr_key, *p_com_key, *p_login_key);

  // The token is now paired
  g_token.Pair();

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 0;

  g_chan.WriteResponse(resp);
}


void ReturnLoginKey(const Command&) {
  Response resp;
  const Crypto::EncryptableKey& key_to_encrypt = (const Crypto::EncryptableKey&)g_token.LoginKey();

  auto size = Encrypt(key_to_encrypt, g_token.ComKey(), &resp.arg[0]);

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = size;

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
  g_cmd_registrer[cmdid::kEncryptUserString] = &EncryptUserString;
  g_cmd_registrer[cmdid::kResetKeys] = &ResetKeys;
  g_cmd_registrer[cmdid::kReturnLoginKey] = &ReturnLoginKey;
}


struct AutoCmdRegistrer {
  AutoCmdRegistrer() {
    RegistrerCommands();
  }
};

static AutoCmdRegistrer registrer2;

}  // namespace anonymous