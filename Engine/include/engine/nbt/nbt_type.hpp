#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_TYPE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_TYPE_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace engine::nbt {

enum class NbtType : uint8_t {
  TAG_END = 0,
  TAG_BYTE = 1,
  TAG_SHORT = 2,
  TAG_INT = 3,
  TAG_LONG = 4,
  TAG_FLOAT = 5,
  TAG_DOUBLE = 6,
  TAG_BYTE_ARRAY = 7,
  TAG_STRING = 8,
  TAG_LIST = 9,
  TAG_COMPOUND = 10,
  TAG_INT_ARRAY = 11,
  TAG_LONG_ARRAY = 12
};

class NbtList;
class NbtCompound;

class NbtValue {
 public:
  NbtValue();

  template <typename T>
  NbtValue& operator=(T value) {
    operator=(value);
    return *this;
  }

  NbtValue& operator=(int8_t value);
  NbtValue& operator=(int16_t value);
  NbtValue& operator=(int32_t value);
  NbtValue& operator=(int64_t value);

  NbtValue& operator=(float value);
  NbtValue& operator=(double value);

  NbtValue& operator=(NbtType type);

  [[nodiscard]] int8_t getByte() const;
  [[nodiscard]] int16_t getShort() const;
  [[nodiscard]] int32_t getInt() const;
  [[nodiscard]] int64_t getLong() const;

  [[nodiscard]] float getFloat() const;
  [[nodiscard]] double getDouble() const;

  [[nodiscard]] std::vector<int8_t>& getByteArray() const;
  [[nodiscard]] std::vector<int32_t>& getIntArray() const;
  [[nodiscard]] std::vector<int64_t>& getLongArray() const;

  [[nodiscard]] NbtCompound& getCompound() const;

  [[nodiscard]] NbtType getType() const;
 private:
  void initializeResourceType(NbtType type);

  NbtType m_Type;
  std::shared_ptr<void> m_ResourceDestructor;

  union {
    int8_t byteValue;
    int16_t shortValue;
    int32_t intValue;
    int64_t longValue;

    float floatValue;
    double doubleValue;

    std::vector<int8_t>* byteArrayValue;
    std::string* stringValue;
    std::vector<NbtList>* listValue;
    NbtCompound* compoundValue;
    std::vector<int32_t>* intArrayValue;
    std::vector<int64_t>* longArrayValue;
  } m_Union;
};

class NbtCompound {
 public:
  NbtValue& operator[](const char* key);

  bool removeKey(const char* key);

  bool hasKey(const char* key) const;

  [[nodiscard]] const std::string& getName() const { return m_Name; }
 private:
  std::string m_Name;
  std::unordered_map<std::string, NbtValue> m_Values;
};

class NbtList {
 public:
  NbtList();

  void pushBack(const NbtValue& value);

  NbtList& operator=(NbtType type);
 private:
  NbtType m_Type;
  std::vector<NbtValue> m_Values;
};
}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_NBT_NBT_TYPE_HPP_
