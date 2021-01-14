#include <catch.hpp>
#include <util.h>

#include <one/arcus/c_api.h>
#include <one/arcus/error.h>
#include <one/arcus/array.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/object.h>
#include <one/arcus/types.h>

using namespace i3d::one;

TEST_CASE("array unit tests", "[array]") {
    Array a;

    const bool boolean = true;
    const int integer = 1;
    const String string = "tata";
    const Array array;
    const Object object;

    // Checking manipulations.
    REQUIRE(a.is_empty());
    REQUIRE(a.size() == 0);
    REQUIRE(a.capacity() == 0);
    const size_t size = 5;
    const size_t capacity = size;
    a.reserve(capacity);
    REQUIRE(a.capacity() == capacity);
    a.push_back_bool(boolean);
    a.push_back_int(integer);
    a.push_back_string(string);
    a.push_back_array(array);
    a.push_back_object(object);
    REQUIRE(!a.is_empty());
    REQUIRE(a.size() == size);
    REQUIRE(a.capacity() == capacity);

    Array b(a);
    REQUIRE(b.get() == a.get());

    Array c;
    a.reserve(a.size());
    c = a;
    REQUIRE(c.get() == a.get());
    c.pop_back();
    REQUIRE(c.size() == size - 1);
    REQUIRE(c.capacity() == capacity);
    c.clear();
    REQUIRE(c.size() == 0);
    REQUIRE(c.capacity() == capacity);

    // Checking types.
    REQUIRE(a.is_val_bool(0));
    REQUIRE(!a.is_val_bool(1));
    REQUIRE(!a.is_val_bool(2));
    REQUIRE(!a.is_val_bool(3));
    REQUIRE(!a.is_val_bool(4));
    REQUIRE(!a.is_val_bool(5));

    REQUIRE(!a.is_val_int(0));
    REQUIRE(a.is_val_int(1));
    REQUIRE(!a.is_val_int(2));
    REQUIRE(!a.is_val_int(3));
    REQUIRE(!a.is_val_int(4));
    REQUIRE(!a.is_val_int(5));

    REQUIRE(!a.is_val_string(0));
    REQUIRE(!a.is_val_string(1));
    REQUIRE(a.is_val_string(2));
    REQUIRE(!a.is_val_string(3));
    REQUIRE(!a.is_val_string(4));

    REQUIRE(!a.is_val_string(0));
    REQUIRE(!a.is_val_string(1));
    REQUIRE(a.is_val_string(2));
    REQUIRE(!a.is_val_string(3));
    REQUIRE(!a.is_val_string(4));

    REQUIRE(!a.is_val_array(0));
    REQUIRE(!a.is_val_array(1));
    REQUIRE(!a.is_val_array(2));
    REQUIRE(a.is_val_array(3));
    REQUIRE(!a.is_val_array(4));

    REQUIRE(!a.is_val_object(0));
    REQUIRE(!a.is_val_object(1));
    REQUIRE(!a.is_val_object(2));
    REQUIRE(!a.is_val_object(3));
    REQUIRE(a.is_val_object(4));

    // Checking getters.
    bool val_boolean = false;
    int val_integer = 2;
    String val_string = "tete";
    Array val_array(b);
    Object val_object;
    size_t val_size = 0;

    REQUIRE(!is_error(a.val_bool(0, val_boolean)));
    REQUIRE(val_boolean == boolean);
    REQUIRE(!is_error(a.val_int(1, val_integer)));
    REQUIRE(val_integer == integer);
    REQUIRE(!is_error(a.val_string_size(2, val_size)));
    REQUIRE(val_size == string.size());
    REQUIRE(!is_error(a.val_string(2, val_string)));
    REQUIRE(val_string == string);
    REQUIRE(!is_error(a.val_array(3, val_array)));
    REQUIRE(val_array.is_empty());
    REQUIRE(!is_error(a.val_object(4, val_object)));
    REQUIRE(val_object.is_empty());

    // Checking setters.
    Array aa;
    aa.push_back_bool(true);
    Object oo;
    oo.set_val_bool("toto", true);
    REQUIRE(!is_error(a.set_val_bool(0, true)));
    REQUIRE(!is_error(a.set_val_int(1, 2)));
    REQUIRE(!is_error(a.set_val_string(2, "bbb")));
    REQUIRE(!is_error(a.set_val_array(3, aa)));
    REQUIRE(!is_error(a.set_val_object(4, oo)));

    REQUIRE(!is_error(a.val_array(3, val_array)));
    REQUIRE(val_array.get() == aa.get());
    REQUIRE(!is_error(a.val_object(4, val_object)));
    REQUIRE(val_object.get() == oo.get());

    // Checking out of bounds.
    REQUIRE(!a.is_val_bool(5));
    REQUIRE(!a.is_val_bool(1000));
    REQUIRE(!a.is_val_int(5));
    REQUIRE(!a.is_val_int(1000));
    REQUIRE(!a.is_val_string(5));
    REQUIRE(!a.is_val_string(1000));
    REQUIRE(!a.is_val_array(5));
    REQUIRE(!a.is_val_array(1000));
    REQUIRE(!a.is_val_object(5));
    REQUIRE(!a.is_val_object(1000));

    REQUIRE(is_error(a.val_bool(5, val_boolean)));
    REQUIRE(is_error(a.val_bool(1000, val_boolean)));
    REQUIRE(is_error(a.val_int(5, val_integer)));
    REQUIRE(is_error(a.val_int(1000, val_integer)));
    REQUIRE(is_error(a.val_string(5, val_string)));
    REQUIRE(is_error(a.val_string(1000, val_string)));
    REQUIRE(is_error(a.val_string_size(5, val_size)));
    REQUIRE(is_error(a.val_string_size(1000, val_size)));
    REQUIRE(is_error(a.val_array(5, val_array)));
    REQUIRE(is_error(a.val_array(1000, val_array)));
    REQUIRE(is_error(a.val_object(5, val_object)));
    REQUIRE(is_error(a.val_object(1000, val_object)));

    REQUIRE(is_error(a.set_val_bool(5, true)));
    REQUIRE(is_error(a.set_val_bool(1000, true)));
    REQUIRE(is_error(a.set_val_int(5, 2)));
    REQUIRE(is_error(a.set_val_int(1000, 2)));
    REQUIRE(is_error(a.set_val_string(5, "")));
    REQUIRE(is_error(a.set_val_string(1000, "")));
    REQUIRE(is_error(a.set_val_array(5, array)));
    REQUIRE(is_error(a.set_val_array(1000, array)));
    REQUIRE(is_error(a.set_val_object(5, object)));
    REQUIRE(is_error(a.set_val_object(1000, object)));

    a.clear();

    REQUIRE(!a.is_val_bool(0));
    REQUIRE(!a.is_val_int(0));
    REQUIRE(!a.is_val_string(0));
    REQUIRE(!a.is_val_array(0));
    REQUIRE(!a.is_val_object(0));

    REQUIRE(is_error(a.val_bool(0, val_boolean)));
    REQUIRE(is_error(a.val_int(0, val_integer)));
    REQUIRE(is_error(a.val_string(0, val_string)));
    REQUIRE(is_error(a.val_string_size(0, val_size)));
    REQUIRE(is_error(a.val_array(0, val_array)));
    REQUIRE(is_error(a.val_object(0, val_object)));

    REQUIRE(is_error(a.set_val_bool(0, true)));
    REQUIRE(is_error(a.set_val_int(0, 2)));
    REQUIRE(is_error(a.set_val_string(0, "")));
    REQUIRE(is_error(a.set_val_array(0, array)));
    REQUIRE(is_error(a.set_val_object(0, object)));
}

TEST_CASE("array c_api", "[array]") {
    OneArrayPtr a = nullptr;
    REQUIRE(!is_error(one_array_create(&a)));
    REQUIRE(a != nullptr);

    unsigned int size = 0;
    bool val_bool = false;
    int val_int = 0;
    String val_string = "";
    Array array;
    Object object;
    Array val_array;
    Object val_object;

    // Checking nullptr.
    REQUIRE(is_error(one_array_create(nullptr)));
    REQUIRE(is_error(one_array_copy(nullptr, a)));
    REQUIRE(is_error(one_array_copy(a, nullptr)));
    REQUIRE(is_error(one_array_clear(nullptr)));
    REQUIRE(is_error(one_array_reserve(nullptr, size)));
    REQUIRE(is_error(one_array_is_empty(nullptr, &val_bool)));
    REQUIRE(is_error(one_array_is_empty(a, nullptr)));
    REQUIRE(is_error(one_array_size(nullptr, &size)));
    REQUIRE(is_error(one_array_capacity(nullptr, &size)));
    REQUIRE(is_error(one_array_capacity(a, nullptr)));

    REQUIRE(is_error(one_array_push_back_bool(nullptr, val_bool)));
    REQUIRE(is_error(one_array_push_back_int(nullptr, val_int)));
    REQUIRE(is_error(one_array_push_back_string(nullptr, val_string.c_str())));
    REQUIRE(is_error(one_array_push_back_string(a, nullptr)));
    OneArrayPtr a_ptr = (OneArray *)&val_array;
    REQUIRE(is_error(one_array_push_back_array(nullptr, a_ptr)));
    REQUIRE(is_error(one_array_push_back_array(a, nullptr)));
    OneObjectPtr o_ptr = (OneObject *)&val_array;
    REQUIRE(is_error(one_array_push_back_object(nullptr, o_ptr)));
    REQUIRE(is_error(one_array_push_back_object(a, nullptr)));

    REQUIRE(is_error(one_array_pop_back(nullptr)));

    int pos = 0;
    bool result = false;

    REQUIRE(is_error(one_array_is_val_bool(nullptr, pos, &result)));
    REQUIRE(is_error(one_array_is_val_bool(a, pos, nullptr)));
    REQUIRE(is_error(one_array_is_val_int(nullptr, pos, &result)));
    REQUIRE(is_error(one_array_is_val_int(a, pos, nullptr)));
    REQUIRE(is_error(one_array_is_val_string(nullptr, pos, &result)));
    REQUIRE(is_error(one_array_is_val_string(a, pos, nullptr)));
    REQUIRE(is_error(one_array_is_val_array(nullptr, pos, &result)));
    REQUIRE(is_error(one_array_is_val_array(a, pos, nullptr)));
    REQUIRE(is_error(one_array_is_val_object(nullptr, pos, &result)));
    REQUIRE(is_error(one_array_is_val_object(a, pos, nullptr)));

    REQUIRE(is_error(one_array_val_bool(nullptr, pos, &val_bool)));
    REQUIRE(is_error(one_array_val_bool(a, pos, nullptr)));
    REQUIRE(is_error(one_array_val_int(nullptr, pos, &val_int)));
    REQUIRE(is_error(one_array_val_int(a, pos, nullptr)));
    REQUIRE(is_error(one_array_val_string_size(nullptr, pos, &size)));
    REQUIRE(is_error(one_array_val_string_size(a, pos, nullptr)));
    String val;
    val.reserve(size);
    REQUIRE(is_error(one_array_val_string(nullptr, pos, &val[0], size)));
    REQUIRE(is_error(one_array_val_string(a, pos, nullptr, size)));
    REQUIRE(is_error(one_array_val_array(nullptr, pos, a_ptr)));
    REQUIRE(is_error(one_array_val_array(a, pos, nullptr)));
    REQUIRE(is_error(one_array_val_object(nullptr, pos, o_ptr)));
    REQUIRE(is_error(one_array_val_object(a, pos, nullptr)));

    REQUIRE(is_error(one_array_set_val_bool(nullptr, pos, val_bool)));
    REQUIRE(is_error(one_array_set_val_int(nullptr, pos, val_int)));
    REQUIRE(is_error(one_array_set_val_string(nullptr, pos, val_string.c_str())));
    REQUIRE(is_error(one_array_set_val_string(a, pos, nullptr)));
    REQUIRE(is_error(one_array_set_val_array(nullptr, pos, a_ptr)));
    REQUIRE(is_error(one_array_set_val_array(a, pos, nullptr)));
    REQUIRE(is_error(one_array_set_val_object(nullptr, pos, o_ptr)));
    REQUIRE(is_error(one_array_set_val_object(a, pos, nullptr)));

    REQUIRE(!is_error(one_array_clear(a)));
    REQUIRE(!is_error(one_array_reserve(a, 5)));
    size = 0;
    REQUIRE(!is_error(one_array_size(a, &size)));
    REQUIRE(size == 0);
    unsigned int capacity = 0;
    REQUIRE(!is_error(one_array_capacity(a, &capacity)));
    REQUIRE(capacity == 5);

    const String string_value = "string";
    const size_t string_value_size = string_value.size();
    REQUIRE(!is_error(one_array_push_back_bool(a, true)));
    REQUIRE(!is_error(one_array_push_back_int(a, 1)));
    REQUIRE(!is_error(one_array_push_back_string(a, string_value.c_str())));
    REQUIRE(!is_error(one_array_push_back_array(a, (OneArray *)&array)));
    REQUIRE(!is_error(one_array_push_back_object(a, (OneObject *)&object)));

    result = false;
    REQUIRE(!is_error(one_array_is_val_bool(a, 0, &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_array_is_val_int(a, 1, &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_array_is_val_string(a, 2, &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_array_is_val_array(a, 3, &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_array_is_val_object(a, 4, &result)));
    REQUIRE(result == true);

    REQUIRE(!is_error(one_array_val_bool(a, 0, &val_bool)));
    REQUIRE(val_bool == true);
    REQUIRE(!is_error(one_array_val_int(a, 1, &val_int)));
    REQUIRE(val_int == 1);
    REQUIRE(!is_error(one_array_val_string_size(a, 2, &size)));
    REQUIRE(size == string_value_size);
    const size_t val_string_bis_size = 100;
    char val_string_bis[val_string_bis_size];
    REQUIRE(is_error(one_array_val_string(a, 2, val_string_bis, string_value_size - 1)));
    REQUIRE(!is_error(one_array_val_string(a, 2, val_string_bis, string_value_size)));
    REQUIRE(!is_error(one_array_val_string(a, 2, val_string_bis, val_string_bis_size)));
    REQUIRE(String(val_string_bis, string_value_size) == string_value);
    REQUIRE(!is_error(one_array_val_array(a, 3, (OneArray *)&val_array)));
    REQUIRE(val_array.get() == array.get());
    REQUIRE(!is_error(one_array_val_object(a, 4, (OneObject *)&val_object)));
    REQUIRE(val_object.get() == object.get());

    REQUIRE(!is_error(one_array_set_val_bool(a, 0, false)));
    REQUIRE(!is_error(one_array_set_val_int(a, 1, 2)));
    REQUIRE(!is_error(one_array_set_val_string(a, 2, "toto")));
    REQUIRE(!is_error(one_array_set_val_array(a, 3, (OneArray *)&val_array)));
    REQUIRE(!is_error(one_array_set_val_object(a, 4, (OneObject *)&val_object)));

    one_array_destroy(a);
    one_array_destroy(nullptr);
}
