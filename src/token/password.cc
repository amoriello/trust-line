//  Author Amoriello Hutti - 2015

#include <token/password.h>

#include <Arduino.h>
#include <AES.h>

#include <token/crypto.h>
#include <assert.h>

AES g_aes;
AES* g_p_aes = &g_aes;


void CreatePassword(Password* p_pass, uint8_t required_pass_size) {
  Range ASCII_password_range(32, 126); // see ascii table
  
  // First byte is reserved for actual password size
  uint8_t* p_begin_password = p_pass->data + 1;

  memset(p_begin_password, 0, Password::kMaxSize);
  uint8_t pass_size = min(Password::kMaxSize, required_pass_size);
  
  
  FillWithRandom(p_begin_password, pass_size, ASCII_password_range);
  p_pass->size = pass_size;
}


uint8_t CipherPassword(const Password& pass, const SymKey& sym_key, uint8_t* p_data) {
  uint8_t iv[16];
  uint8_t block_size = 16;

  FillWithRandom(iv, sizeof(iv), 255);
  g_p_aes->set_key((byte*)sym_key.data, 128);

  //
  // cipher data layout :
  // [block1][block2 | block3 | block4 | block5]
  // [0---16][xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]
  //   IV         K(password)
  //
  memset(p_data, 0, 5 * block_size); 
  memcpy(p_data, iv, sizeof(iv));
  uint8_t* pass_cipher_begin = p_data + sizeof(iv);

  // bypass constness
  g_p_aes->cbc_encrypt((uint8_t*)pass.data, pass_cipher_begin, 80 / block_size, iv);
  return 80;
}


uint8_t DecipherPassword(const uint8_t* p_data, const SymKey& sym_key, Password* p_pass) {
  uint8_t block_size = 16;

  // cipher data is like this :
  // [0---16][16---------------80]
  //   IV         K(password)
  //
  uint8_t* iv = (uint8_t*)p_data;  //bypass constness...
  uint8_t* pass_cipher_begin = (uint8_t*)(p_data + 16);  // bypass constness

  g_p_aes->cbc_decrypt(pass_cipher_begin, p_pass->data, 80 / block_size, iv);  // bypass constness...
  return p_pass->size;
}

