#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/arcus/array.h>
#include <one/arcus/object.h>
#include <one/arcus/internal/rapidjson/document.h>

#include <string>

using namespace one;

TEST_CASE("array unit tests", "[array]") {
    Array a;

    const bool boolean = true;
    const int integer = 1;
    const std::string string = "tata";
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
    std::string val_string = "tete";
    Array val_array(b);
    Object val_object;

    REQUIRE(!is_error(a.val_bool(0, val_boolean)));
    REQUIRE(val_boolean == boolean);
    REQUIRE(!is_error(a.val_int(1, val_integer)));
    REQUIRE(val_integer == integer);
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
    REQUIRE(is_error(a.val_string(1000, val_string)));
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
    REQUIRE(is_error(a.val_array(0, val_array)));
    REQUIRE(is_error(a.val_object(0, val_object)));

    REQUIRE(is_error(a.set_val_bool(0, true)));
    REQUIRE(is_error(a.set_val_int(0, 2)));
    REQUIRE(is_error(a.set_val_string(0, "")));
    REQUIRE(is_error(a.set_val_array(0, array)));
    REQUIRE(is_error(a.set_val_object(0, object)));
}
