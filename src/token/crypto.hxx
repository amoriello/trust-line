//  Author Amoriello Hutti - 2015


template <class EncryptableData>
uint8_t Encrypt(const EncryptableData& input, const Crypto::SymKey& sym_key, uint8_t* output) {
  AES aes;
  const uint8_t block_size = sizeof(Crypto::SymKey::data);
  uint8_t iv[block_size];
  // Nb of blocks to encrypt for this EncryptableData
  const auto nb_blocks = NbAESBlocks<EncryptableData>::value;

  static_assert(IsEncryptable<EncryptableData>::value,
                "Input data size must be a multiple of block size");

  //
  // cipher data layout :
  // [------][block1 | block2 | block3 | ......]
  // [0---15][xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx]
  //   IV         K(RawData::data)
  //
  memset(output, 0, (1 + nb_blocks) * block_size);

  // Initialize IV with random data
  FillWithRandom(iv, block_size, Range(0, 255));

  // Copy IV to the leading p_data bytes
  memcpy(output, iv, block_size);

  // Encrypted data begins after IV
  uint8_t* cipher_begin = output + block_size;

  // set_key takes size has bits, not bytes
  aes.set_key((byte*)sym_key.data, block_size * 8);

  // bypass constness here because of third_party missing constness
  uint8_t* data = (uint8_t*)&input.data[0];

  aes.cbc_encrypt(data, cipher_begin, nb_blocks, iv);

  return (1 + nb_blocks) * block_size;
}


template <class EncryptableData>
uint8_t Decrypt(const uint8_t* input, const Crypto::SymKey& sym_key, EncryptableData* ouput) {
  AES aes;
  const uint8_t block_size = sizeof(Crypto::SymKey::data);

  // Nb of blocks to encrypt for this EncryptableData
  const auto nb_blocks = NbAESBlocks<EncryptableData>::value;

  static_assert(IsEncryptable<EncryptableData>::value,
                "Ouput size must be a multiple of block size");


  // cipher data is like this :
  // [0---15][16---------------]
  //   IV         K(password)
  //
  auto iv = input;  //bypass constness because of third_party

  // Encrypted password begins after IV
  // Forcing bypass constness here because of third_party
  uint8_t* cipher_begin = (uint8_t*)(input + block_size);

  aes.set_key((byte*)sym_key.data, block_size * 8);

  aes.cbc_decrypt(cipher_begin, (byte*)ouput->data, nb_blocks, (byte*)iv);

  return sizeof(EncryptableData::data);
}
