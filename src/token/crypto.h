//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_SYMKEY_H_
#define SRC_TOKEN_SYMKEY_H_

#include <stdint.h>
#include <LiquidCrystal.h>
#include <AES.h>

#include <token/range.h>
#include <token/encryptable.h>

struct Command;



class Crypto {
 public:  // Make it non-copyable
  Crypto(const Crypto&) = delete;
  Crypto& operator=(const Crypto&) = delete;

  Crypto() {}


 public:
  struct SymKey {
    uint8_t data[16];
  };

  struct Challenge {
    uint8_t nonce[8];  // 64 bits random value
  };

  using EncryptableKey = EncryptableData<sizeof(Crypto::SymKey::data)>;

 public:
  void Initialize(bool first_time);

  // The key used to ciph password
  const Crypto::SymKey& PassKey() const;

  // The key use to verify peer and command by challenge/response
  const Crypto::SymKey& CRKey() const;

  // The key used to trans-cipher a password, send it back securely
  // to the phone, so that the peer can deciph and see a clear
  // password
  const Crypto::SymKey& ComKey() const;

  // The key used to encrypt login information
  const Crypto::SymKey& LoginKey() const;

  // Stores and reload given symetric keys.
  // This function is used to restore a token
  // using QrCode (@see Command "ResetKey")
  void StoreKeys(const SymKey& pass_key, const SymKey& cr_key,
                 const SymKey& req_key, const SymKey& login_key);


  void Reset();

 private:

  enum key_idx {
    kPassKey  = 0,
    kCRKey    = kPassKey + sizeof(SymKey::data),
    kComKey   = kCRKey   + sizeof(SymKey::data),
    kLoginKey = kComKey  + sizeof(SymKey::data)
  };


private:
  void LoadKeys();
  
  void CreateAndStoreKeys();

private:
  SymKey pass_key_;
  SymKey cr_key_;
  SymKey com_key_;
  SymKey login_key_;
};


void CreateChallenge(Crypto::Challenge* p_challenge);

bool IsValidSecurityToken(const Command& cmd, const Crypto::SymKey& cr_key);

void FillWithRandom(uint8_t* p_data, uint16_t data_size, const Range& range,
                    const uint8_t* p_exculdes = 0, uint8_t exclude_size = 0);

void CreatePassword(Password* p_pass, uint8_t required_pass_size);

template <class EncryptableData>
uint8_t Encrypt(const EncryptableData& input, const Crypto::SymKey& sym_key, uint8_t* output);

template <class EncryptableData>
uint8_t Decrypt(const uint8_t* input, const Crypto::SymKey& sym_key, EncryptableData* ouput);

#include <token/crypto.hxx>

#endif  // SRC_TOKEN_SYMKEY_H_