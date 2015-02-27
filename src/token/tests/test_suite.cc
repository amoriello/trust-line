//  Author Amoriello Hutti - 2015

#include <token/commands.h>
#include <token/token.h>
#include <token/communication/channel.h>
#include <token/password.h>
#include <token/log.h>



void TestEcho(const Command& cmd) {
  TLOG("Test Echo");

  Response resp;
  
  resp.hdr.id = cmd.hdr.id;
  resp.hdr.arg_size = cmd.hdr.arg_size;

  for (int i = 0; i < cmd.hdr.arg_size; ++i) {
    resp.arg[i] = cmd.arg[i] + 1;
  }
  
  g_chan.WriteResponse(resp);
}


void TestPassword(const Command& cmd) {
  TLOG("Test Password");

  Response resp;
  Password pass;

  uint8_t size = cmd.arg[0];
  if (size > Password::kMaxSize) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
  }

  CreatePassword(&pass, size);

  resp.hdr.id = cmd.hdr.id;
  // arg contains [pass_size + pass]
  resp.hdr.arg_size = size + 1;

  for (int i = 0; i < size + 1; ++i) {
    resp.arg[i] = pass.data[i];
  }

  g_chan.WriteResponse(resp);
}


void TestKey(const Command& cmd) {
  Response resp;
  
  resp.hdr.id = cmd.hdr.id;
  resp.hdr.arg_size = 1;
  resp.arg[0] = 0x02;

  g_chan.WriteResponse(resp);
}


void TestPasswordCrypto(const Command& cmd) {
  Response resp;
  Password pass;

  uint8_t size = cmd.arg[0];
  if (size > Password::kMaxSize) {
    resp.hdr.id = respid::kInvalidArgument;
    resp.hdr.arg_size = 0;
  }

  CreatePassword(&pass, cmd.arg[0]);
  
  CipherPassword(pass, g_token.PassKey(), resp.arg);

  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = 80;

  g_chan.WriteResponse(resp);
}


void TestIsInitialized(const Command& cmd) {
  bool is_initialized = g_token.IsInitialized();
  Response resp;

  resp.hdr.id = cmd.hdr.id;
  resp.hdr.arg_size = 1;
  resp.arg[0] = is_initialized;
  g_chan.WriteResponse(resp);
}


void TestIsAuthorized(const Command& cmd) {
  bool is_authorized = false;
  Response resp;

  resp.hdr.id = cmd.hdr.id;
  resp.hdr.arg_size = 1;
  resp.arg[0] = is_authorized;
  g_chan.WriteResponse(resp);
}

/*
void TestChallenge(const Command& cmd) {
  Response resp;
  Challenge* p_challenge = (Challenge*)&resp.arg;
  CreateChallenge(p_challenge);

  resp.hdr.id = cmd.hdr.id;
  resp.hdr.arg_size = sizeof(Challenge);
  g_chan.WriteResponse(resp);

  Command cmd_containing_sec_token;
  g_chan.ReadCommand(&cmd_containing_sec_token, &resp);

  bool is_valid_response = IsValidResponse(cmd_containing_sec_token, *p_challenge, *g_p_cr_key);

  resp.hdr.id = cmd.hdr.id;
  resp.hdr.arg_size = 1;
  resp.arg[0] = is_valid_response;
  g_com.WriteResponse(resp);
}
*/

namespace /* anonymous */ {

void RegistrerCommands() {
  g_cmd_registrer[cmdid::kTestEcho] = &TestEcho;
  g_cmd_registrer[cmdid::kTestPassword] = &TestPassword;
  g_cmd_registrer[cmdid::kTestKey] = &TestKey;
  g_cmd_registrer[cmdid::kTestPasswordCrypto] = &TestPasswordCrypto;
  g_cmd_registrer[cmdid::kTestIsInitialized] = &TestIsInitialized;
  g_cmd_registrer[cmdid::kTestIsAuthorized] = &TestIsAuthorized;
  //g_cmd_registrer[cmdid::kTestChallenge] = &TestChallenge;
}

struct AutoCmdRegistrer {
  AutoCmdRegistrer() {
    RegistrerCommands();
  }
};

static AutoCmdRegistrer registrer;

}  // namespace anonymous