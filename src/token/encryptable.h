#ifndef SRC_TOKEN_ENCRYPTABLE_H_
#define SRC_TOKEN_ENCRYPTABLE_H_


template <int Size>
struct EncryptableData {
  uint8_t data[Size];
};

template <class Encryptable> 
struct NbMaxASCIIChar {
  static const uint8_t value = sizeof(Encryptable::data) - 1;
};

template <class Encryptable, int KeySize = 16>
struct NbAESBlocks {
  enum info : uint8_t { value = sizeof(Encryptable::data) / KeySize };
};

template <class Encryptable, int KeySize = 16>
struct IsEncryptable {
  static const uint8_t value = !(sizeof(Encryptable::data) % KeySize);
};


using UserString = EncryptableData<48>;
using Password = EncryptableData<64>;


#endif // SRC_TOKEN_ENCRYPTABLE_H_