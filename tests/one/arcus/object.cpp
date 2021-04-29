#include <catch.hpp>
#include <tests/one/arcus/util.h>

#include <one/arcus/c_api.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/object.h>
#include <one/arcus/types.h>

using namespace i3d::one;

TEST_CASE("key scope test", "[object]") {
    OneObjectPtr ptr;
    REQUIRE(!one_is_error(one_object_create(&ptr)));
    for (int i = 0; i < 500; i++) {
        // Add the key vals but scoped so that the memory for the keys are freed.
        {
            REQUIRE(
                !one_is_error(one_object_set_val_int(ptr, std::string("q").c_str(), 2)));
            REQUIRE(
                !one_is_error(one_object_set_val_int(ptr, std::string("w").c_str(), 3)));
        }
        int val;
        {
            REQUIRE(
                !one_is_error(one_object_val_int(ptr, std::string("q").c_str(), &val)));
            REQUIRE(
                !one_is_error(one_object_val_int(ptr, std::string("w").c_str(), &val)));
        }
    }
    one_object_destroy(ptr);
}

TEST_CASE("object unit tests", "[object]") {
    Object o;
    REQUIRE(o.is_empty());

    const bool boolean = true;
    const int integer = 1;
    const String string = "string";
    const Array array;
    const Object object;

    // Check setters.
    REQUIRE(!is_error(o.set_val_bool("bool", boolean)));

    // Wrong type
    {
        REQUIRE(is_error(o.set_val_int("bool", integer)));
        REQUIRE(is_error(o.set_val_string("bool", string)));
        REQUIRE(is_error(o.set_val_array("bool", array)));
        REQUIRE(is_error(o.set_val_object("bool", object)));
    }

    REQUIRE(!is_error(o.set_val_int("int", integer)));

    // Wrong type
    {
        REQUIRE(is_error(o.set_val_bool("int", boolean)));
        REQUIRE(is_error(o.set_val_string("int", string)));
        REQUIRE(is_error(o.set_val_array("int", array)));
        REQUIRE(is_error(o.set_val_object("int", object)));
    }

    REQUIRE(!is_error(o.set_val_string("string", string)));

    // Wrong type
    {
        REQUIRE(is_error(o.set_val_bool("string", boolean)));
        REQUIRE(is_error(o.set_val_int("string", integer)));
        REQUIRE(is_error(o.set_val_array("string", array)));
        REQUIRE(is_error(o.set_val_object("string", object)));
    }

    REQUIRE(!is_error(o.set_val_array("array", array)));

    // Wrong type
    {
        REQUIRE(is_error(o.set_val_bool("array", boolean)));
        REQUIRE(is_error(o.set_val_int("array", integer)));
        REQUIRE(is_error(o.set_val_string("array", string)));
        REQUIRE(is_error(o.set_val_object("array", object)));
    }

    REQUIRE(!is_error(o.set_val_object("object", object)));

    // Wrong type
    {
        REQUIRE(is_error(o.set_val_bool("object", boolean)));
        REQUIRE(is_error(o.set_val_int("object", integer)));
        REQUIRE(is_error(o.set_val_string("object", string)));
        REQUIRE(is_error(o.set_val_array("object", array)));
    }

    REQUIRE(!o.is_empty());

    // Check types.
    REQUIRE(o.is_val_bool("bool"));
    REQUIRE(!o.is_val_int("bool"));
    REQUIRE(!o.is_val_string("bool"));
    REQUIRE(!o.is_val_array("bool"));
    REQUIRE(!o.is_val_object("bool"));

    REQUIRE(!o.is_val_bool("int"));
    REQUIRE(o.is_val_int("int"));
    REQUIRE(!o.is_val_string("int"));
    REQUIRE(!o.is_val_array("int"));
    REQUIRE(!o.is_val_object("int"));

    REQUIRE(!o.is_val_bool("string"));
    REQUIRE(!o.is_val_int("string"));
    REQUIRE(o.is_val_string("string"));
    REQUIRE(!o.is_val_array("string"));
    REQUIRE(!o.is_val_object("string"));

    REQUIRE(!o.is_val_bool("array"));
    REQUIRE(!o.is_val_int("array"));
    REQUIRE(!o.is_val_string("array"));
    REQUIRE(o.is_val_array("array"));
    REQUIRE(!o.is_val_object("array"));

    REQUIRE(!o.is_val_bool("object"));
    REQUIRE(!o.is_val_int("object"));
    REQUIRE(!o.is_val_string("object"));
    REQUIRE(!o.is_val_array("object"));
    REQUIRE(o.is_val_object("object"));

    // Check getters.
    bool val_boolean = false;
    int val_integer = 1;
    String val_string = "";
    Array val_array;
    Object val_object;

    REQUIRE(!is_error(o.val_bool("bool", val_boolean)));
    REQUIRE(val_boolean == boolean);
    REQUIRE(is_error(o.val_int("bool", val_integer)));
    REQUIRE(is_error(o.val_string("bool", val_string)));
    REQUIRE(is_error(o.val_array("bool", val_array)));
    REQUIRE(is_error(o.val_object("bool", val_object)));

    REQUIRE(is_error(o.val_bool("int", val_boolean)));
    REQUIRE(!is_error(o.val_int("int", val_integer)));
    REQUIRE(val_integer == integer);
    REQUIRE(is_error(o.val_string("int", val_string)));
    REQUIRE(is_error(o.val_array("int", val_array)));
    REQUIRE(is_error(o.val_object("int", val_object)));

    REQUIRE(is_error(o.val_bool("string", val_boolean)));
    REQUIRE(is_error(o.val_int("string", val_integer)));
    size_t size = 0;
    REQUIRE(!is_error(o.val_string_size("string", size)));
    REQUIRE(size == string.size());
    REQUIRE(!is_error(o.val_string("string", val_string)));

    REQUIRE(val_string == string);
    REQUIRE(is_error(o.val_array("string", val_array)));
    REQUIRE(is_error(o.val_object("string", val_object)));

    REQUIRE(is_error(o.val_bool("array", val_boolean)));
    REQUIRE(is_error(o.val_int("array", val_integer)));
    REQUIRE(is_error(o.val_string("array", val_string)));
    REQUIRE(!is_error(o.val_array("array", val_array)));
    REQUIRE(val_array.get() == array.get());
    REQUIRE(is_error(o.val_object("array", val_object)));

    REQUIRE(is_error(o.val_bool("object", val_boolean)));
    REQUIRE(is_error(o.val_int("object", val_integer)));
    REQUIRE(is_error(o.val_string("object", val_string)));
    REQUIRE(is_error(o.val_array("object", val_array)));
    REQUIRE(!is_error(o.val_object("object", val_object)));
    REQUIRE(val_object.get() == object.get());

    // Check invald key.

    REQUIRE(!o.is_val_bool("invalid"));
    REQUIRE(!o.is_val_int("invalid"));
    REQUIRE(!o.is_val_string("invalid"));
    REQUIRE(!o.is_val_array("invalid"));
    REQUIRE(!o.is_val_object("invalid"));

    REQUIRE(is_error(o.val_bool("invalid", val_boolean)));
    REQUIRE(is_error(o.val_int("invalid", val_integer)));
    REQUIRE(is_error(o.val_string_size("invalid", size)));
    REQUIRE(is_error(o.val_string("invalid", val_string)));
    REQUIRE(is_error(o.val_array("invalid", val_array)));
    REQUIRE(is_error(o.val_object("invalid", val_object)));

    // Checks.

    Object copy(o);
    REQUIRE(copy.get() == o.get());

    Object assignment;
    assignment = o;
    REQUIRE(assignment.get() == o.get());

    o.clear();
    REQUIRE(o.is_empty());

    REQUIRE(!is_error(o.set_val_bool("bool", true)));
    REQUIRE(!is_error(o.remove_key("bool")));
    REQUIRE(is_error(o.remove_key("bool")));
}

TEST_CASE("object c_api", "[object]") {
    OneObjectPtr o = nullptr;
    REQUIRE(!is_error(one_object_create(&o)));
    REQUIRE(o != nullptr);

    unsigned int size = 0;
    const char *key = {"key"};
    bool val_bool = false;
    int val_int = 1;
    String val_string = "";
    Array array;
    Object object;
    Array val_array;
    Object val_object;

    OneArrayPtr a_ptr = (OneArray *)&val_array;
    OneObjectPtr o_ptr = (OneObject *)&val_object;

    // Checking nullptr.
    REQUIRE(is_error(one_object_create(nullptr)));

    bool result = false;

    REQUIRE(is_error(one_object_is_val_bool(nullptr, key, &result)));
    REQUIRE(is_error(one_object_is_val_bool(o, nullptr, &result)));
    REQUIRE(is_error(one_object_is_val_bool(o, key, nullptr)));
    REQUIRE(is_error(one_object_is_val_int(nullptr, key, &result)));
    REQUIRE(is_error(one_object_is_val_int(o, nullptr, &result)));
    REQUIRE(is_error(one_object_is_val_int(o, key, nullptr)));
    REQUIRE(is_error(one_object_is_val_string(nullptr, key, &result)));
    REQUIRE(is_error(one_object_is_val_string(o, nullptr, &result)));
    REQUIRE(is_error(one_object_is_val_string(o, key, nullptr)));
    REQUIRE(is_error(one_object_is_val_array(nullptr, key, &result)));
    REQUIRE(is_error(one_object_is_val_array(o, nullptr, &result)));
    REQUIRE(is_error(one_object_is_val_array(o, key, nullptr)));
    REQUIRE(is_error(one_object_is_val_object(nullptr, key, &result)));
    REQUIRE(is_error(one_object_is_val_object(o, nullptr, &result)));
    REQUIRE(is_error(one_object_is_val_object(o, key, nullptr)));

    REQUIRE(is_error(one_object_val_bool(nullptr, key, &val_bool)));
    REQUIRE(is_error(one_object_val_bool(o, nullptr, &val_bool)));
    REQUIRE(is_error(one_object_val_bool(o, key, nullptr)));
    REQUIRE(is_error(one_object_val_int(nullptr, key, &val_int)));
    REQUIRE(is_error(one_object_val_int(o, nullptr, &val_int)));
    REQUIRE(is_error(one_object_val_int(o, key, nullptr)));
    REQUIRE(is_error(one_object_val_string_size(nullptr, key, &size)));
    REQUIRE(is_error(one_object_val_string_size(o, nullptr, &size)));
    REQUIRE(is_error(one_object_val_string_size(o, key, nullptr)));
    const size_t val_size = 100;
    char val[val_size];
    REQUIRE(is_error(one_object_val_string(nullptr, key, val, val_size)));
    REQUIRE(is_error(one_object_val_string(o, nullptr, val, val_size)));
    REQUIRE(is_error(one_object_val_string(o, key, nullptr, size)));
    REQUIRE(is_error(one_object_val_array(nullptr, key, a_ptr)));
    REQUIRE(is_error(one_object_val_array(o, nullptr, a_ptr)));
    REQUIRE(is_error(one_object_val_array(o, key, nullptr)));
    REQUIRE(is_error(one_object_val_object(nullptr, key, o_ptr)));
    REQUIRE(is_error(one_object_val_object(o, nullptr, o_ptr)));
    REQUIRE(is_error(one_object_val_object(o, key, nullptr)));

    REQUIRE(is_error(one_object_set_val_bool(nullptr, key, val_bool)));
    REQUIRE(is_error(one_object_set_val_bool(o, nullptr, val_bool)));
    REQUIRE(is_error(one_object_set_val_int(nullptr, key, val_int)));
    REQUIRE(is_error(one_object_set_val_int(o, nullptr, val_int)));
    REQUIRE(is_error(one_object_set_val_string(nullptr, key, val_string.c_str())));
    REQUIRE(is_error(one_object_set_val_string(o, nullptr, val_string.c_str())));
    REQUIRE(is_error(one_object_set_val_string(o, key, nullptr)));
    REQUIRE(is_error(one_object_set_val_array(nullptr, key, a_ptr)));
    REQUIRE(is_error(one_object_set_val_array(o, nullptr, a_ptr)));
    REQUIRE(is_error(one_object_set_val_array(o, key, nullptr)));
    REQUIRE(is_error(one_object_set_val_object(nullptr, key, o_ptr)));
    REQUIRE(is_error(one_object_set_val_object(o, nullptr, o_ptr)));
    REQUIRE(is_error(one_object_set_val_object(o, key, nullptr)));

    const String string_value = "toto";
    const size_t string_value_size = string_value.size();

    REQUIRE(!is_error(one_object_set_val_bool(o, "bool", false)));
    REQUIRE(!is_error(one_object_set_val_int(o, "int", 2)));
    REQUIRE(!is_error(one_object_set_val_string(o, "string", "toto")));
    REQUIRE(!is_error(one_object_set_val_array(o, "array", (OneArray *)&val_array)));
    REQUIRE(!is_error(one_object_set_val_object(o, "object", (OneObject *)&val_object)));

    result = false;
    REQUIRE(!is_error(one_object_is_val_bool(o, "bool", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_object_is_val_int(o, "int", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_object_is_val_string(o, "string", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_object_is_val_array(o, "array", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_object_is_val_object(o, "object", &result)));
    REQUIRE(result == true);

    REQUIRE(!is_error(one_object_val_bool(o, "bool", &val_bool)));
    REQUIRE(val_bool == false);
    REQUIRE(!is_error(one_object_val_int(o, "int", &val_int)));
    REQUIRE(val_int == 2);
    REQUIRE(!is_error(one_object_val_string_size(o, "string", &size)));
    REQUIRE(size == string_value_size);
    const size_t val_string_bis_size = 100;
    char val_string_bis[val_string_bis_size];
    REQUIRE(is_error(
        one_object_val_string(o, "string", val_string_bis, string_value_size - 1)));
    REQUIRE(
        !is_error(one_object_val_string(o, "string", val_string_bis, string_value_size)));
    REQUIRE(!is_error(
        one_object_val_string(o, "string", val_string_bis, val_string_bis_size)));
    REQUIRE(!is_error(
        one_object_val_string(o, "string", val_string_bis, val_string_bis_size)));
    REQUIRE(String(val_string_bis, string_value_size) == string_value);
    REQUIRE(!is_error(one_object_val_array(o, "array", (OneArray *)&val_array)));
    REQUIRE(val_array.get() == array.get());
    REQUIRE(!is_error(one_object_val_object(o, "object", (OneObject *)&val_object)));
    REQUIRE(val_object.get() == object.get());

    one_object_destroy(o);
    one_object_destroy(nullptr);
}
