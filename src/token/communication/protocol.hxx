//  Author Amoriello Hutti - 2015

#include <stdio.h>

#include <Arduino.h>

#include <token/commands.h>
#include <token/token.h>
#include <token/log.h>


template <class Channel>
void TokenProtocol<Channel>::Initialize() {
  chan_.Initialize();
}


template <class Channel>
int TokenProtocol<Channel>::Available() {
  return chan_.Available();
}


template <class Channel>
void TokenProtocol<Channel>::ProcessEvents() {
  chan_.ProcessEvents();
}


template <class Channel>
int TokenProtocol<Channel>::ReadCommand(Command* p_cmd, Response* /* p_resp */) {
  ReadContext read_ctx;
  WriteContext write_ctx;
  bool done = false;

  TLOG("Read Sec Token");
  done = Read(&read_ctx, p_cmd->sec_token, sizeof(p_cmd->sec_token));

  if (!done) {
    TLOG("Cannot Read SecToken");
    return false;
  }

  
  TLOG("Read Hdr");
  done = Read(&read_ctx, (uint8_t*)&p_cmd->hdr, sizeof(p_cmd->hdr));

  if (!done) {
    TLOG("Cannot Read Hdr");
    return false;
  }

  if (p_cmd->hdr.id >= cmdid::kNbCmd || p_cmd->hdr.arg_size > sizeof(p_cmd->arg)) {
    TLOG("BADCMD");
    return 0;
  }

  TLOG("Read Arg");

  done = Read(&read_ctx, p_cmd->arg, p_cmd->hdr.arg_size);

  if (!done) {
    TLOG("Cannot Read Arg");
    return false;
  }

  return done;
}


template <class Channel>
int TokenProtocol<Channel>::WriteResponse(const Response& resp) {
  WriteContext write_ctx;
  bool done = false;

  done = Write(&write_ctx, (uint8_t*)&resp.hdr, sizeof(resp.hdr));

  done = Write(&write_ctx, resp.arg, resp.hdr.arg_size);

  return done;
}


template <class Channel>
bool TokenProtocol<Channel>::Write(WriteContext* p_ctx, const uint8_t* p_data, uint8_t size) {
  return WriteBufferCtx(p_ctx, p_data, size) == size;
}


template <class Channel>
bool TokenProtocol<Channel>::Read(ReadContext* p_ctx, uint8_t* p_data, uint8_t size) {
  return ReadBufferCtx(p_ctx, p_data, size) == size;
}


template <class Channel>
uint8_t TokenProtocol<Channel>::ReadBufferCtx(ReadContext* p_ctx, uint8_t* p_data, uint8_t size) {
  uint8_t session_bytes_read = 0;
  uint8_t attempt_retry = 0;
  const uint8_t max_retry = 3;

  while (size > 0) {
    uint8_t bytes_to_read = min(size, kMaxBurst - p_ctx->bytes_read);
    uint8_t brust_read_byte = chan_.readBytes((char*)(p_data + session_bytes_read), bytes_to_read);
    
    TLOG("brustRead:");
    TLOG((const char*)(p_data + session_bytes_read), brust_read_byte, HEX);
    TLOG("============");

    if (brust_read_byte != bytes_to_read) {
      if (brust_read_byte == 0) {
        if (attempt_retry < max_retry) {
          TLOG("Fail Read... waiting...");
          ++attempt_retry;
          delay(500);
          continue;
        }
      }

      TLOG_("bad read: ");
      TLOG_(brust_read_byte, DEC);
      TLOG_(" intead of: ");
      TLOG(bytes_to_read, DEC);
      break;
    }
    

    size -= brust_read_byte;
    p_ctx->bytes_read += brust_read_byte;
    session_bytes_read += brust_read_byte;
    
    if (p_ctx->bytes_read == kMaxBurst) {
      TLOG("Read 5, sending ack");
      //  Ack for partial read with nb bytes read
      uint8_t ack = 0x80 | p_ctx->bytes_read;
      chan_.write(ack);
      p_ctx->bytes_read = 0;
    }
  
  }
  return session_bytes_read;
}


template <class Channel>
uint8_t TokenProtocol<Channel>::WriteBufferCtx(WriteContext* p_ctx, const uint8_t* p_data, uint8_t size) {
  uint8_t session_bytes_sent = 0;
  
  while (size > 0) {
    uint8_t bytes_to_write = min(size, kMaxBurst - p_ctx->bytes_sent);
    uint8_t brust_write_bytes = chan_.writeBytes((char*)p_data + session_bytes_sent, bytes_to_write);

    if (brust_write_bytes != bytes_to_write) {
      TLOG("Brust");
      TLOG(brust_write_bytes, DEC);
      TLOG("ToWrite:");
      TLOG(bytes_to_write, DEC);

      TLOG("brust != byte_to_write!!");
      break;
    }

    size -= brust_write_bytes;
    p_ctx->bytes_sent += brust_write_bytes;
    session_bytes_sent += brust_write_bytes;

    if (p_ctx->bytes_sent == kMaxBurst) {
      uint8_t ack;
      uint8_t attempt = 0;
      const uint8_t kMaxAttempt = 3;
      while (attempt < kMaxAttempt) {
        uint8_t read_bytes = chan_.readBytes((char*)&ack, sizeof(ack));

        if (read_bytes != 0) {
          TLOG("Read Ack");
          break;
        }
        TLOG("+Attempt wait reader");
        ++attempt;
        delay(1000);
      }


      if (ack != p_ctx->bytes_sent) {
        TLOG("Bad Ack: ");
        TLOG(ack, DEC);
        break;
      }
      // reset context for another burst without ack
      p_ctx->bytes_sent = 0;
    }
  }
  return session_bytes_sent;
}

