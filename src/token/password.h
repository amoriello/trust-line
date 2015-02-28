//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_PASSWORD_H_
#define SRC_TOKEN_PASSWORD_H_

#include <stdint.h>

// Forward decls
struct SymKey;
struct AES;

union Password {
  enum PassEnum {
    kMaxSize = 63
  };
  
  enum SizeInfo {
    kSizeIndex = 0
  };

  uint8_t size;
  uint8_t data[64];  // 1 byte to store size + 63 password chars
  // One byte reserved for real password size
};




//
// ciphered data layout :
//
// [block1][block2 | block3 | block4 | block5]
// [0---16][xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]
//   IV         K(password)
//
// 5 blocks of 16 bytes (80 bytes)


void CreatePassword(Password* p_pass, uint8_t required_pass_size);


/////// Constraints :
//// Buffer input size : 64 bytes (4 blocks)
//// p_data size : 80 bytes (iv + 4 blocks)
// Return size of cipher (80 for now)
uint8_t CipherPassword(const Password& pass, const SymKey& sym_key, uint8_t* p_data);


/////// Constraints :
//// p_data size : 80 bytes (iv + 4 blocks)
// Return size of deciphered password
uint8_t DecipherPassword(const uint8_t* p_data, const SymKey& sym_key, Password* p_pass);


#endif  // SRC_TOKEN_PASSWORD_H_