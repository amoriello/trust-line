//  Author Amoriello Hutti - 2015

#include <token/password.h>

#include <Arduino.h>
#include <AES.h>

#include <token/crypto.h>
#include <token/log.h>
#include <assert.h>

AES g_aes;
AES* g_p_aes = &g_aes;


void CreatePassword(Password* p_pass, uint8_t required_pass_size) {
  assert(required_pass_size < Password::kMaxCharacters);

  Range ASCII_password_range(32, 126); // see ascii table
  
  uint8_t pass_size = min(Password::kMaxCharacters, required_pass_size);
  memset(p_pass->data, 0, sizeof(Password));
  
  FillWithRandom(p_pass->data, pass_size, ASCII_password_range);
}


uint8_t EncryptPassword(const Password& pass, const SymKey& sym_key, uint8_t* p_data) {
  const uint8_t block_size = 16;
  uint8_t iv[block_size];
  

  // Initialize IV with random data
  FillWithRandom(iv, sizeof(iv), 255);


  //
  // cipher data layout :
  // [------][block1 | block2 | block3 | block4]
  // [0---15][xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]
  //   IV         K(password)
  //
  memset(p_data, 0, (1 + 4) * block_size);

  // Copy IV to the leading p_data bytes
  memcpy(p_data, iv, sizeof(iv));

  // Encrypted password begins after IV
  uint8_t* pass_cipher_begin = p_data + sizeof(iv);

  g_p_aes->set_key((byte*)sym_key.data, 128);

  // bypass constness here because of third_party missing constness
  g_p_aes->cbc_encrypt((uint8_t*)&pass.data[0], pass_cipher_begin, sizeof(Password::data) / block_size, iv);
  return 80;
}


uint8_t DecryptPassword(const uint8_t* p_data, const SymKey& sym_key, Password* p_pass) {
  uint8_t block_size = 16;

  // cipher data is like this :
  // [0---15][16---------------79]
  //   IV         K(password)
  //
  uint8_t* iv = (uint8_t*)p_data;  //bypass constness because of third_party

  // Encrypted password begins after IV
  uint8_t* pass_cipher_begin = (uint8_t*)(p_data + 16);  // idem

  g_p_aes->set_key((byte*)sym_key.data, 128);
  g_p_aes->cbc_decrypt(pass_cipher_begin, (uint8_t*)p_pass->data, sizeof(Password::data) / block_size, iv);  // bypass constness...
  return 0xff;
}

