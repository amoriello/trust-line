//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_SYMKEY_H_
#define SRC_TOKEN_SYMKEY_H_

#include <stdint.h>
#include <LiquidCrystal.h>

struct Command;

struct SymKey {
  uint8_t data[16];  // 128 bits aes symkey
};


#pragma pack(1)
struct Challenge {
  uint8_t nonce[8];  // 64 bit random value
};
#pragma pack(0)


struct Range {
  Range(uint16_t i_begin, uint16_t i_end)
    : begin(i_begin), end(i_end) { }
    
  Range(uint16_t i_end)
    : begin(0), end(i_end) { }

  uint16_t begin;
  uint16_t end;
};

namespace keyindex {

enum idx {
  kPassKey = 0,
  kCRKey = 16,
  kReqKey = 32
};

}  // keyindex


class Crypto {
 public:
  void Initialize(bool first_time);

  // The key used to ciph password
  const SymKey& PassKey() const;

  // The key use to verify peer and command by challenge/response
  const SymKey& CRKey() const;

  // The key used to trans-cipher a password, send it back securely
  // to the phone, so that the peer can deciph and see a clear
  // password
  const SymKey& ReqKey() const;

  void Reset();

 private:
  enum key_idx {
    kPassKey = 0,
    kCRKey = 16,
    kReqKey = 32
  };


private:
  void LoadKeys();
  
  void CreateAndStoreKeys();

private:
  SymKey pass_key_;
  SymKey cr_key_;
  SymKey req_key_;
};



void CreateChallenge(Challenge* p_challenge);

bool IsValidSecurityToken(const Command& cmd, const SymKey& cr_key);

void FillWithRandom(uint8_t* p_data, uint16_t data_size, const Range& range);


#endif  // SRC_TOKEN_SYMKEY_H_