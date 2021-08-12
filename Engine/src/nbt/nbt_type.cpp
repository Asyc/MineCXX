#include "engine/nbt/nbt_type.hpp"

#include <stdexcept>

namespace engine::nbt {

NbtValue::NbtValue() : m_Type(NbtType::TAG_END), m_ResourceDestructor(), m_Union() {}

NbtValue& NbtValue::operator=(int8_t value) {
  initializeResourceType(NbtType::TAG_BYTE);
  m_Union.byteValue = value;
  return *this;
}

NbtValue& NbtValue::operator=(int16_t value) {
  initializeResourceType(NbtType::TAG_SHORT);
  m_Union.shortValue = value;
  return *this;
}
NbtValue& NbtValue::operator=(int32_t value) {
  initializeResourceType(NbtType::TAG_INT);
  m_Union.intValue = value;
  return *this;
}
NbtValue& NbtValue::operator=(int64_t value) {
  initializeResourceType(NbtType::TAG_LONG);
  m_Union.longValue = value;
  return *this;
}

NbtValue& NbtValue::operator=(float value) {
  initializeResourceType(NbtType::TAG_FLOAT);
  m_Union.floatValue = value;
  return *this;
}

NbtValue& NbtValue::operator=(double value) {
  initializeResourceType(NbtType::TAG_DOUBLE);
  m_Union.doubleValue = value;
  return *this;
}

NbtValue& NbtValue::operator=(NbtType type) {
  initializeResourceType(type);
  return *this;
}

int8_t NbtValue::getByte() const {
  return m_Union.byteValue;
}

int16_t NbtValue::getShort() const {
  return m_Union.shortValue;
}

int32_t NbtValue::getInt() const {
  return m_Union.intValue;
}

int64_t NbtValue::getLong() const {
  return m_Union.longValue;
}

float NbtValue::getFloat() const {
  return m_Union.floatValue;
}

double NbtValue::getDouble() const {
  return m_Union.doubleValue;
}

std::vector<int8_t>& NbtValue::getByteArray() const {
  return *m_Union.byteArrayValue;
}

std::vector<int32_t>& NbtValue::getIntArray() const {
  return *m_Union.intArrayValue;
}

std::vector<int64_t>& NbtValue::getLongArray() const {
  return *m_Union.longArrayValue;
}

NbtCompound& NbtValue::getCompound() const {
  return *m_Union.compoundValue;
}

NbtType NbtValue::getType() const {
  return m_Type;
}

void NbtValue::initializeResourceType(NbtType type) {
  if (m_Type == type) return;

  m_ResourceDestructor = nullptr;
  m_Type = type;

  switch (type) {
    case NbtType::TAG_END:break;
    case NbtType::TAG_BYTE:m_Union.byteValue = 0;
      break;
    case NbtType::TAG_SHORT:m_Union.shortValue = 0;
      break;
    case NbtType::TAG_INT:m_Union.intValue = 0;
      break;
    case NbtType::TAG_LONG:m_Union.longValue = 0;
      break;
    case NbtType::TAG_FLOAT:m_Union.floatValue = 0.0f;
      break;
    case NbtType::TAG_DOUBLE:m_Union.doubleValue = 0.0f;
      break;
    case NbtType::TAG_BYTE_ARRAY: {
      auto byteArray = std::make_shared<std::vector<int8_t>>();
      m_Union.byteArrayValue = byteArray.get();
      m_ResourceDestructor = std::move(byteArray);
    }
    case NbtType::TAG_STRING: {
      auto byteArray = std::make_shared<std::vector<int8_t>>();
      m_Union.byteArrayValue = byteArray.get();
      m_ResourceDestructor = std::move(byteArray);
    }
    case NbtType::TAG_LIST: {
      auto byteArray = std::make_shared<std::vector<int8_t>>();
      m_Union.byteArrayValue = byteArray.get();
      m_ResourceDestructor = std::move(byteArray);
    }
    case NbtType::TAG_COMPOUND: {
      auto compound = std::make_shared<NbtCompound>();
      m_Union.compoundValue = compound.get();
      m_ResourceDestructor = std::move(compound);
    }
    case NbtType::TAG_INT_ARRAY: {
      auto intArray = std::make_shared<std::vector<int32_t>>();
      m_Union.intArrayValue = intArray.get();
      m_ResourceDestructor = std::move(intArray);
    }
    case NbtType::TAG_LONG_ARRAY: {
      auto longArray = std::make_shared<std::vector<int64_t>>();
      m_Union.longArrayValue = longArray.get();
      m_ResourceDestructor = std::move(longArray);
    }
  }
}

NbtValue& NbtCompound::operator[](const char* key) {
  return m_Values[key];
}

bool NbtCompound::removeKey(const char* key) {
  const auto it = m_Values.find(key);
  if (it == m_Values.end()) return false;
  m_Values.erase(it);

  return true;
}

bool NbtCompound::hasKey(const char* key) const {
  return m_Values.find(key) != m_Values.end();
}

NbtList::NbtList() : m_Type(NbtType::TAG_END) {}

void NbtList::pushBack(const NbtValue& value) {
  if (value.getType() != m_Type) {
    throw std::runtime_error("nbt list: element type did not match list type");
  }

  m_Values.push_back(value);
}

NbtList& NbtList::operator=(NbtType type) {
  m_Values.clear();
  m_Type = type;
  return *this;
}

}