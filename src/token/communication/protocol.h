//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_COMMUNICATION_PROTOCOL_H_
#define SRC_TOKEN_COMMUNICATION_PROTOCOL_H_

#include <stdint.h>

class Command;
class Response;
class SymKey;
class Challenge;
class CommandHeader;


template <class Channel>
class TokenProtocol {
 public:
  int Available();

  void Initialize();

  void ProcessEvents();

  /*!
  * Read a command from the associated master device
  *
  * A command should always have an associated SecurityToken, but
  * the device can use it or not, depeding on the security level of the
  * command (@see commands.h, authenticated (or not) allowed commands list)
  * but that decision is not in the scope of this class.
  *
  * [out] p_cmd : the command sent by the associated master device
  * [out] p_resp : a response to send in case of error @todo: think about it.
  * [out] p_challenge : the computed challenge : [random nonce + received SecurityToken]
  */
  int ReadCommand(Command* p_cmd, Response* p_resp, Challenge*);

  int WriteResponse(const Response& response);

private:
  enum max_burst_size {
    //!
    // Because communication channel's stack size is limited :
    // Long data (> stack size) is sent truncated into kMaxBurst
    // buffers and ack
    //
    // PROTOCOL DESC:
    // Sending long buffers :
    // - send long buffer byte burst of kMaxBurst bytes.
    // - then wait for a ack : a byte that contains the nb of bytes read
    // - repeat until full buffer is fully transfered
    //
    // Receiving long buffers :
    // - receive kMaxBurst data size
    // - sends a ack byte set to kMaxBurst
    // - repeat until full buffer is fully transfered
    //
    kMaxBurst = 50
  };

  //!
  // Context structures helps to keep the number of bytes
  // wrote / send between a communication sequence (multiples read)
  // followed by multiple writes...
  //
  // This is usefull information to keep up with protococl acks.
  //
  struct ReadContext {
    ReadContext() : bytes_read(0) {}
    uint8_t bytes_read;
  };

  struct WriteContext {
    WriteContext() : bytes_sent(0) {}
    uint8_t bytes_sent;
  };


  //*
  //
  // High level primitive that take care of the protocol
  // handling.
  //
  // Use these functions to read / writes any size of data
  // using the communication's channel.
  //
  bool Write(WriteContext* p_ctx, const uint8_t* p_data, uint8_t size);

  bool Read(ReadContext* p_ctx, uint8_t* p_data, uint8_t size);



  // See burst for infos
  uint8_t ReadBufferCtx(ReadContext* p_ctx, uint8_t* p_data, uint8_t size);

  uint8_t WriteBufferCtx(WriteContext* p_ctx, const uint8_t* p_data, uint8_t size);


private:
  Channel chan_;
};


#include <token/communication/protocol.hxx>


#endif  // SRC_TOKEN_COMMUNICATION_PROTCOL_H_