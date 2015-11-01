//  Author Amoriello Hutti - 2015

#ifndef SRC_TOKEN_TOKEN_H_
#define SRC_TOKEN_TOKEN_H_

#include <token/crypto.h>


class Token {
 public:
    // Make it non-copyable
  Token(const Token&) = delete;
  Token& operator=(const Token&) = delete;

  Token() {}

 public:
  /*!
   * Initializes the token.
   *
   * IF the token run the the first time, or after Reset :
   *    - Initialize (create/store keys) and Load (load keys in memory) Crypto
   *    - Initialize Keyboard
   *    - Initialize Communication Channel
   *    - Mark the token as initialized
   *
   * ELSE (the token does not run for the first time)
   *    - Load (load keys in memory) Crypto
   *    - Initialize Keyboard
   *    - Initialize Communication Channel
   */
  void Initialize();

  bool IsInitialized() const;

  bool IsPaired() const;

  /*!
   * Pair the token with its device
   *
   * Set differents flags in eeprom to concider this device as paired.
   * Pairing can only be made once, or after a reset.
   *
   * Once paired, the token will not allow anymore pairing command
   */
  void Pair();

  /*!
   * Get the symmetric key used to cipher password.
   * 
   * The paired device doesn't know this key, and cannot decipher any password
   * ciphered with this key.
   *
   */
  const Crypto::SymKey& PassKey() const  {
    return crypto_.PassKey();
  }

  /*!
   * Get the symmetric key used for challenge/response and authentication
   *
   * This key is shared between the Token and the paired device.
   * When the paired device sends a command, it requests a challenge (nonce)
   * to the token (a 64bit random value) and computes a "SecurityToken" 
   * applying HMAC-SHA256(cr_key, nonce | cmd).
   *
   * The paired device then sends the SecurityToken followed by the command cmd
   * itself : SecurityToken -> cmd.
   *
   * On reception, the token computes the HMAC-SHA256(cr_key, nonce | cmd) of the
   * saved nonce and received command. If the SecurityToken matches the token
   * commputation, then the command is authenticated because only the Token and
   * the paired device knows CRKey.
   */
  const Crypto::SymKey& CRKey() const {
    return crypto_.CRKey();
  }

  /*!
   * Get the symmetric key use to cipher a clear password or other strings
   * allowing the paired device to decipher it.
   *
   * This key is shared by the token and the paired device. It is
   * used when the paired device wants to get a clear info, or sends clear info.
   * to the token.
   */
  const Crypto::SymKey& ComKey() const {
    return crypto_.ComKey();
  }


  /*!
   * Store given key and reload them
   *
   * This function is used when you want to replicate a token using
   * keys from a previous (broken?) token, using Trustline secret QRCode
   *
   */
  void StoreKeys(const Crypto::SymKey& pass_key, const Crypto::SymKey& cr_key,
                 const Crypto::SymKey& req_key) {
    crypto_.StoreKeys(pass_key, cr_key, req_key);
  }


  /*!
   * Factory reset the Token.
   *
   * - Rewrites Keys with 0 inside eeprom
   * - Rewrites memory loaded keys with 0
   * - Mark as not paired
   * - Mark as not initialized
   */
  void Reset();

 private:
  void SetInitializedState(bool initialize);

  void SetPairedState(bool paired);

  void ReadInitializedInfo(uint8_t* p_data) const;

  void ReadPairInfo(uint8_t* p_data) const;

 private:
  Crypto crypto_;
};

extern Token g_token;

#endif  // SRC_TOKEN_TOKEN_H_