#include <one/arcus/array.h>

#include <one/arcus/internal/rapidjson/writer.h>
#include <one/arcus/object.h>

namespace one {

Array::Array() : _doc(rapidjson::kArrayType) {}

Array::Array(const Array &other) {
    _doc.CopyFrom(other.get(), _doc.GetAllocator());
}

Array &Array::operator=(const Array &other) {
    _doc.CopyFrom(other.get(), _doc.GetAllocator());
    return *this;
}

Error Array::set(const rapidjson::Value &array) {
    if (!array.IsArray()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    _doc.Clear();
    _doc.CopyFrom(array, _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

void Array::clear() {
    _doc.Clear();
}

void Array::reserve(size_t size) {
    _doc.Reserve(size, _doc.GetAllocator());
}

bool Array::is_empty() const {
    return _doc.Empty();
}

size_t Array::size() const {
    return _doc.Size();
}

size_t Array::capacity() const {
    return _doc.Capacity();
}

void Array::push_back_bool(bool val) {
    _doc.PushBack(val, _doc.GetAllocator());
}

void Array::push_back_int(int val) {
    _doc.PushBack(val, _doc.GetAllocator());
}

void Array::push_back_string(const std::string &val) {
    _doc.PushBack(rapidjson::Value(val.c_str(), _doc.GetAllocator()).Move(),
                  _doc.GetAllocator());
}

void Array::push_back_array(const Array &val) {
    _doc.PushBack(rapidjson::Value(val.get(), _doc.GetAllocator()).Move(),
                  _doc.GetAllocator());
}

void Array::push_back_object(const Object &val) {
    _doc.PushBack(rapidjson::Value(val.get(), _doc.GetAllocator()).Move(),
                  _doc.GetAllocator());
}

void Array::pop_back() {
    _doc.PopBack();
}

bool Array::is_val_bool(unsigned int pos) const {
    if (_doc.Size() <= pos) {
        return false;
    }

    return _doc[pos].IsBool();
}

bool Array::is_val_int(unsigned int pos) const {
    if (_doc.Size() <= pos) {
        return false;
    }

    return _doc[pos].IsInt();
}

bool Array::is_val_string(unsigned int pos) const {
    if (_doc.Size() <= pos) {
        return false;
    }

    return _doc[pos].IsString();
}

bool Array::is_val_array(unsigned int pos) const {
    if (_doc.Size() <= pos) {
        return false;
    }

    return _doc[pos].IsArray();
}

bool Array::is_val_object(unsigned int pos) const {
    if (_doc.Size() <= pos) {
        return false;
    }

    return _doc[pos].IsObject();
}

Error Array::val_bool(unsigned int pos, bool &val) const {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    const auto &elem = _doc[pos];
    if (!elem.IsBool()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_BOOL;
    }

    val = elem.GetBool();
    return ONE_ERROR_NONE;
}

Error Array::val_int(unsigned int pos, int &val) const {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    const auto &elem = _doc[pos];
    if (!elem.IsInt()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_INT;
    }

    val = elem.GetInt();
    return ONE_ERROR_NONE;
}

Error Array::val_string_size(unsigned int pos, size_t &size) const {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    const auto &elem = _doc[pos];
    if (!elem.IsString()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    size = elem.GetStringLength();
    return ONE_ERROR_NONE;
}

Error Array::val_string(unsigned int pos, std::string &val) const {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    const auto &elem = _doc[pos];
    if (!elem.IsString()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    val = elem.GetString();
    return ONE_ERROR_NONE;
}

Error Array::val_array(unsigned int pos, Array &val) const {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    const auto &elem = _doc[pos];
    if (!elem.IsArray()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    val.set(elem);
    return ONE_ERROR_NONE;
}

Error Array::val_object(unsigned int pos, Object &val) const {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    const auto &elem = _doc[pos];
    if (!elem.IsObject()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    val.set(elem);
    return ONE_ERROR_NONE;
}

Error Array::set_val_bool(unsigned int pos, bool val) {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    auto &elem = _doc[pos];

    // Avoid changing the current element type.
    if (!elem.IsBool()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_BOOL;
    }

    elem.Set(val);
    return ONE_ERROR_NONE;
}

Error Array::set_val_int(unsigned int pos, int val) {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    auto &elem = _doc[pos];

    // Avoid changing the current element type.
    if (!elem.IsInt()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_INT;
    }

    elem.Set(val);
    return ONE_ERROR_NONE;
}

Error Array::set_val_string(unsigned int pos, const std::string &val) {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    auto &elem = _doc[pos];

    // Avoid changing the current element type.
    if (!elem.IsString()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    elem.Set(val.c_str());
    return ONE_ERROR_NONE;
}

Error Array::set_val_array(unsigned int pos, const Array &val) {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    auto &elem = _doc[pos];

    // Avoid changing the current element type.
    if (!elem.IsArray()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    elem.CopyFrom(val.get(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

Error Array::set_val_object(unsigned int pos, const Object &val) {
    if (_doc.Size() <= pos) {
        return ONE_ERROR_ARRAY_POSITION_OUT_OF_BOUNDS;
    }

    auto &elem = _doc[pos];

    // Avoid changing the current element type.
    if (!elem.IsObject()) {
        return ONE_ERROR_ARRAY_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    elem.CopyFrom(val.get(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

}  // namespace one
