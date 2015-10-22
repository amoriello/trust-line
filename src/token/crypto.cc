//  Author Amoriello Hutti - 2015

#include <token/crypto.h>

#include <Arduino.h>
#include <EEPROM.h>
#include <sha256.h>
#include <Entropy.h>

#include <token/commands.h>
#include <token/log.h>


//  The current challenge.
//  Next Command's response MUST reslove this challenge
Challenge g_current_challenge;

namespace {

void _ComputeValidSecToken(const Command& cmd, const Challenge& challenge,
                           const SymKey& cr_key, uint8_t* p_result) {
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


void _ResetKey(SymKey* p_key, uint8_t idx) {
  for (uint16_t i = 0; i < sizeof(p_key->data); ++i) {
    p_key->data[i] = 0;
    EEPROM.write(10 + idx + i, 0);
  }
}


void _StoreSymKey(const SymKey& key, uint8_t idx) {
  for (uint16_t i = 0; i < sizeof(key.data); ++i) {
    // Permanently store key
    EEPROM.write(10 + idx + i, key.data[i]);
  }
}


void _LoadSymKey(SymKey* p_key, uint8_t idx) {
  for (uint16_t i = 0; i < sizeof(p_key->data); ++i) {
    p_key->data[i] = EEPROM.read(10 + idx + i);
  }
}


void _CreateSymKey(SymKey* p_key) {
  SymKey& key = *p_key;
  uint8_t max_rand_value = 255;
  
  // fill key data with random values between 0 and 255;
  FillWithRandom(key.data, sizeof(key.data), max_rand_value);
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



const SymKey& Crypto::PassKey() const {
  return pass_key_;
}


const SymKey& Crypto::CRKey() const {
  return cr_key_;
}


const SymKey& Crypto::ReqKey() const {
  return req_key_;
}


void Crypto::LoadKeys() {
  _LoadSymKey(&pass_key_, kPassKey);
  _LoadSymKey(&cr_key_, kCRKey);
  _LoadSymKey(&req_key_, kReqKey);
}


void Crypto::CreateAndStoreKeys() {
  _CreateSymKey(&pass_key_);
  _CreateSymKey(&cr_key_);
  _CreateSymKey(&req_key_);

  _StoreSymKey(pass_key_, kPassKey);
  _StoreSymKey(cr_key_, kCRKey);
  _StoreSymKey(req_key_, kReqKey);
}


void Crypto::StoreKeys(const SymKey& pass_key,
                       const SymKey& cr_key,
                       const SymKey& req_key) {
  _StoreSymKey(pass_key, kPassKey);
  _StoreSymKey(cr_key_, kCRKey);
  _StoreSymKey(req_key_, kReqKey);

  LoadKeys();
}

void Crypto::Reset() {
  _ResetKey(&pass_key_, kPassKey);
  _ResetKey(&cr_key_, kCRKey);
  _ResetKey(&req_key_, kReqKey);
}


void FillWithRandom(uint8_t* p_data, uint16_t data_size, const Range& range) {
  for (uint16_t i = 0; i < data_size; ++i) {
    p_data[i] = Entropy.random(range.begin, range.end);
  }
}


void CreateChallenge(Challenge* p_challenge) {
  uint8_t max_rand_value = 255;
  // fill key data with random values between 0 and 255;
  FillWithRandom(p_challenge->nonce, sizeof(p_challenge->nonce), max_rand_value);

  // The generated challenge is now the current challenge for next command.
  memcpy(&g_current_challenge, p_challenge, sizeof(Challenge));
}


bool IsValidSecurityToken(const Command& cmd, const SymKey& cr_key) {
  uint8_t valid_response[HASH_LENGTH];

  _ComputeValidSecToken(cmd, g_current_challenge, cr_key, valid_response);

  //TLOG("Token:");
  TLOG2((char*)g_current_challenge.nonce, 5, HEX);
  //TLOG("Valid:");
  TLOG2((char*)valid_response, 5, HEX);

  for (uint8_t i = 0; i < HASH_LENGTH; ++i) {
    if (cmd.sec_token[i] != valid_response[i]) {
      return false;
    }
  }

  return true;
}

