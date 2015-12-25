//  Author Amoriello Hutti - 2015

#include <token/crypto.h>

#include <Arduino.h>
#include <EEPROM.h>
#include <sha256.h>
#include <Entropy.h>

#include <token/commands.h>
#include <token/log.h>


//  The current challenge.
//  Next Command MUST reslove this challenge
Crypto::Challenge g_current_challenge;

namespace {

void _ComputeValidSecToken(const Command& cmd, const Crypto::Challenge& challenge,
                           const Crypto::SymKey& cr_key, uint8_t* p_result) {
  Sha256.initHmac(cr_key.data, sizeof(cr_key.data));

  // Computing hash for sent nonce
  for (uint8_t i = 0; i < sizeof(challenge.nonce); ++i) {
    Sha256.write(challenge.nonce[i]);
  }

  // with command
  Sha256.write(cmd.hdr.id);
  Sha256.write(cmd.hdr.arg_size);

  for (uint8_t i = 0; i < cmd.hdr.arg_size; ++i) {
    Sha256.write(cmd.arg[i]);    
  }

  uint8_t* valid_response = Sha256.resultHmac();
  for (uint8_t i = 0; i < HASH_LENGTH; ++i) {
    p_result[i] = valid_response[i];
  }
}


void _ResetKey(Crypto::SymKey* p_key, uint8_t idx) {
  for (uint16_t i = 0; i < sizeof(p_key->data); ++i) {
    p_key->data[i] = 0;
    EEPROM.write(10 + idx + i, 0);
  }
}


void _StoreSymKey(const Crypto::SymKey& key, uint8_t idx) {
  for (uint16_t i = 0; i < sizeof(key.data); ++i) {
    // Permanently store key
    EEPROM.write(10 + idx + i, key.data[i]);
  }
}


void _LoadSymKey(Crypto::SymKey* p_key, uint8_t idx) {
  for (uint16_t i = 0; i < sizeof(p_key->data); ++i) {
    p_key->data[i] = EEPROM.read(10 + idx + i);
  }
}


void _CreateSymKey(Crypto::SymKey* p_key) {
  auto& key = *p_key;
  uint8_t max_rand_value = 255;
  
  // fill key data with random values between 0 and 255;
  FillWithRandom(key.data, sizeof(key.data), Range(0, max_rand_value));
}

}  // anonymous






void Crypto::Initialize(bool first_time) {
  Entropy.initialize();
  if (first_time) {
    TLOG("New Keys!");
    CreateAndStoreKeys();
  }
  LoadKeys();
}


const Crypto::SymKey& Crypto::PassKey() const {
  return pass_key_;
}


const Crypto::SymKey& Crypto::CRKey() const {
  return cr_key_;
}


const Crypto::SymKey& Crypto::ComKey() const {
  return com_key_;
}


const Crypto::SymKey& Crypto::LoginKey() const {
  return login_key_;
}


void Crypto::LoadKeys() {
  _LoadSymKey(&pass_key_, kPassKey);
  _LoadSymKey(&cr_key_, kCRKey);
  _LoadSymKey(&com_key_, kComKey);
  _LoadSymKey(&login_key_, kLoginKey);
}


void Crypto::CreateAndStoreKeys() {
  _CreateSymKey(&pass_key_);
  _CreateSymKey(&cr_key_);
  _CreateSymKey(&com_key_);
  _CreateSymKey(&login_key_);

  _StoreSymKey(pass_key_, kPassKey);
  _StoreSymKey(cr_key_, kCRKey);
  _StoreSymKey(com_key_, kComKey);
  _StoreSymKey(login_key_, kLoginKey);
}


void Crypto::StoreKeys(const SymKey& pass_key,
                       const SymKey& cr_key,
                       const SymKey& com_key,
                       const SymKey& login_key) {
  _StoreSymKey(pass_key, kPassKey);
  _StoreSymKey(cr_key, kCRKey);
  _StoreSymKey(com_key, kComKey);
  _StoreSymKey(login_key, kLoginKey);

  LoadKeys();
}

void Crypto::Reset() {
  _ResetKey(&pass_key_, kPassKey);
  _ResetKey(&cr_key_, kCRKey);
  _ResetKey(&com_key_, kComKey);
  _ResetKey(&login_key_, kLoginKey);
}


void FillWithRandom(uint8_t* p_data, uint16_t data_size, const Range& range,
                    const uint8_t* p_exculdes, uint8_t exculde_size) {
  auto should_exculde = [&](char byte) {
    if (p_exculdes == nullptr) {
      return false;
    }

    for (auto i = 0; i < exculde_size; ++i) {
      if (byte == p_exculdes[i]) {
        return true;
      }
    }
    return false;
  };

  for (uint16_t i = 0; i < data_size; ++i) {
    uint8_t random_byte = 0;
    do {
      random_byte = Entropy.random(range.begin, range.end);
    } while (should_exculde(random_byte));

    p_data[i] = random_byte;
  }
}


void CreateChallenge(Crypto::Challenge* p_challenge) {
  uint8_t max_rand_value = 255;
  // fill key data with random values between 0 and 255;
  FillWithRandom(p_challenge->nonce, sizeof(p_challenge->nonce), Range(0, max_rand_value));

  // The generated challenge is now the current challenge for next command.
  memcpy(&g_current_challenge, p_challenge, sizeof(Crypto::Challenge));
}


bool IsValidSecurityToken(const Command& cmd, const Crypto::SymKey& cr_key) {
  uint8_t valid_response[HASH_LENGTH];

  _ComputeValidSecToken(cmd, g_current_challenge, cr_key, valid_response);

  for (uint8_t i = 0; i < HASH_LENGTH; ++i) {
    if (cmd.sec_token[i] != valid_response[i]) {
      return false;
    }
  }

  return true;
}


void CreatePassword(Password* p_pass, uint8_t required_pass_size) {
  const uint8_t exclude[] = {'|', ' ', '\'', '\"', '`'};

  Range ASCII_password_range(32, 126); // see ascii table
  
  uint8_t pass_size = min(NbMaxASCIIChar<Password>::value, required_pass_size);
  memset(p_pass->data, 0, sizeof(Password));
  
  FillWithRandom(p_pass->data, pass_size, ASCII_password_range,
                 &exclude[0], sizeof(exclude));
}

