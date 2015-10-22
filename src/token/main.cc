//  Author Amoriello Hutti - 2015

#include <Arduino.h>
#include <Entropy.h>


#include <token/commands.h>
#include <token/token.h>
#include <token/communication/channel.h>
#include <token/log.h>

void ExecuteCommand();


void setup() {
  TLOG_INIT();

  g_token.Initialize();

#ifdef TEST_RESET_ON_START
  TLOG("Reset!");
  g_token.Reset();
  g_token.Initialize();
#endif // TEST_RESET_ON_START

  pinMode(13, OUTPUT);

  TLOG("Ready!");
}



void TestBleCom() {
  uint8_t data[20];
  uint8_t *p_size_to_read = &data[0];
 
  TokenBleChan::ReadContext  r_ctx;
  TokenBleChan::WriteContext w_ctx;

  g_chan.Read(&r_ctx, p_size_to_read, 1);
  TLOG("Size To Read:");
  TLOG(*p_size_to_read, DEC);

  g_chan.Read(&r_ctx, data + 1, *p_size_to_read);

  //g_chan.Read(&r_ctx, data, sizeof(data));

  TLOG((char*)data);


  Response resp;
  resp.hdr.id = respid::kOk;
  resp.hdr.arg_size = sizeof(data);

  for (uint8_t i = 0; i < sizeof(data); ++i) {
    resp.arg[i] = data[i] + 1;
  }

  g_chan.WriteResponse(resp);

  //uint8_t ack = 0x80;
  //g_chan.Write(&w_ctx, data, sizeof(data));
  //g_chan.Write(&w_ctx, &ack, 1);
}



void loop() {
  if (g_chan.Available()) {
    TLOG("Command!");
    ExecuteCommand();
    //TestBleCom();
  }
  g_chan.ProcessEvents();
  delay(100);
}


void ExecuteCommand() {
  Command cmd;
  Response resp;


  if (!g_chan.ReadCommand(&cmd, &resp)) {
    // Invalid command (communication, bad id, etc...)
    // Blink Yellow
    //g_chan.WriteResponse(resp);
    TLOG("!Invalid Command");
    return;
  }

  if (IsTesting(cmd)) {
    g_cmd_registrer[cmd.hdr.id](cmd);
    return;
  }

  if (!g_token.IsPaired()) {
    // Token first use, we only accept command kPair
    // Or command ResetKeys
    // Blink Yellow
    if (cmd.hdr.id == cmdid::kPair || cmd.hdr.id == cmdid::kResetKeys) {
      g_cmd_registrer[cmd.hdr.id](cmd);
    } else {
      // Blink yellow code signal invalid command
      // send response
      TLOG("!Pair with device first");
      resp.hdr.id = respid::kPairWithDeviceFirst;
      resp.hdr.arg_size = 0;
      g_chan.WriteResponse(resp);
    }
    return;
  }

  if (g_token.IsPaired() && cmd.hdr.id == cmdid::kPair) {
      TLOG("!Already Paired");
      resp.hdr.id = respid::kDeviceAlreadyPaired;
      resp.hdr.arg_size = 0;
      g_chan.WriteResponse(resp);
      return;
  }

  if (IsNoAuthCommand(cmd)) {
    g_cmd_registrer[cmd.hdr.id](cmd);
  } else {
    // Command requires authentication
    if (!IsValidSecurityToken(cmd, g_token.CRKey())) {
      // Blink red code signal unauthorized command
      // send response
      TLOG("!Invalid Token");
      resp.hdr.id = respid::kBadAuth;
      resp.hdr.arg_size = 0;
      g_chan.WriteResponse(resp);
      return;
    }

    TLOG("OK Token");
    TLOG(cmd.hdr.id, DEC);
    // Can safely execute command
    g_cmd_registrer[cmd.hdr.id](cmd);    
  }
}


