//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_PASSWORD_H_
#define SRC_TOKEN_PASSWORD_H_

#include <stdint.h>

// Forward decls
struct SymKey;
struct AES;

#pragma pack(1)
struct Password {
  enum info : uint8_t {
  	kMaxCharacters = 63
  };

  // Password Data
  // is multple of AES 128 blocksize for ease of use later
  //
  uint8_t data[kMaxCharacters + 1];  // + 1 for ending 0

};
#pragma pack()



/**
 * Create a password with default ASCII printable range
 *
 * Note that character Quote (') Pipe (|) and space ( )
 * is excluded for various reasons:
 *  - i,l I and | could be hard to destinguish, idem for space
 *  - ' " or ` can poorly fail in some site...
 *
 * in/out p_pass  : The password (@see Password) to be filled
 * in required_pass_size : the password's number of character
 *                         (must not exceed Password::kMaxCharachter)
 */
void CreatePassword(Password* p_pass, uint8_t required_pass_size);




///////////////////////////////////////////////////////////////
// ciphered data layout :
//
// [......][block1 | block2 | block3 | block4]
// [0---15][xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]
//   IV         K(password)
//
// 4 blocks of 16 bytes + iv = 4 * 16 + 16 = 80 bytes
///////////////////////////////////////////////////////////////


/**
 * Encrypt a Password with given key. p_data is filled with ciphered 
 * password + iv (see ciphered data layout)
 *
 * in p_pass  : The password (see Password)
 * in sym_key : The AES128 byte symetric key
 * in/out p_data : the ciphered data ouput (80 bytes, see ciphered data layout)
 */
uint8_t EncryptPassword(const Password& pass, const SymKey& sym_key, uint8_t* p_data);


/**
 * Decrypt a Password with given key. p_pass is filled with decrypted password
 *
 * in p_data  : the ciphered data ouput (80 bytes, see ciphered data layout)
 * in sym_key : The AES128 byte symetric key
 * in/out p_pass : The password (see Password)
 */
uint8_t DecryptPassword(const uint8_t* p_data, const SymKey& sym_key, Password* p_pass);


#endif  // SRC_TOKEN_PASSWORD_H_