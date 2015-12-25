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

  //  Read Sec Token
  done = Read(&read_ctx, p_cmd->sec_token, sizeof(p_cmd->sec_token));

  //  Check succesful security token read
  if (!done) {
    return false;
  }

  // Read Header  
  done = Read(&read_ctx, (uint8_t*)&p_cmd->hdr, sizeof(p_cmd->hdr));

  //  Check succesful header read
  if (!done) {
    return false;
  }

  //  Check command id
  if (p_cmd->hdr.id >= cmdid::kNbCmd || p_cmd->hdr.arg_size > sizeof(p_cmd->arg)) {
    return false;
  }

  //  Read Command argument
  done = Read(&read_ctx, p_cmd->arg, p_cmd->hdr.arg_size);
    
  //  Check succesful read of argument
  if (!done) {
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

    if (brust_read_byte != bytes_to_read) {
      if (brust_read_byte == 0) {
        if (attempt_retry < max_retry) {
          //  Fail Read... waiting...
          ++attempt_retry;
          delay(500);
          continue;
        }
      }
      // bad read
      break;
    }
    

    size -= brust_read_byte;
    p_ctx->bytes_read += brust_read_byte;
    session_bytes_read += brust_read_byte;
    
    if (p_ctx->bytes_read == kMaxBurst) {
      //  Read kMaxBurst, sending ack
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
          //  Read Ack
          break;
        }
        //  Attempt wait
        ++attempt;
        delay(1000);
      }


      if (ack != p_ctx->bytes_sent) {
        //  Bad Ack
        break;
      }

      // reset context for another burst without ack
      p_ctx->bytes_sent = 0;
    }
  }
  return session_bytes_sent;
}

