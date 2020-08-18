#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/arcus/array.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/internal/rapidjson/document.h>

#include <string>

using namespace one;

TEST_CASE("payload unit tests", "[payload]") {
    Payload p;
    Array a;
    a.push_back_int(1);
    Object o;
    o.set_val_int("int", 1);
    REQUIRE(p.is_empty());

    // Test setters.

    REQUIRE(!is_error(p.set_val_bool("bool", true)));

    // Wrong type
    {
        REQUIRE(is_error(p.set_val_int("bool", 1)));
        REQUIRE(is_error(p.set_val_string("bool", "string")));
        REQUIRE(is_error(p.set_val_array("bool", a)));
        REQUIRE(is_error(p.set_val_object("bool", o)));
    }

    REQUIRE(!is_error(p.set_val_int("int", 1)));

    // Wrong type
    {
        REQUIRE(is_error(p.set_val_bool("int", true)));
        REQUIRE(is_error(p.set_val_string("int", "string")));
        REQUIRE(is_error(p.set_val_array("int", a)));
        REQUIRE(is_error(p.set_val_object("int", o)));
    }

    REQUIRE(!is_error(p.set_val_string("string", "string")));

    // Wrong type
    {
        REQUIRE(is_error(p.set_val_bool("string", true)));
        REQUIRE(is_error(p.set_val_int("string", 1)));
        REQUIRE(is_error(p.set_val_array("string", a)));
        REQUIRE(is_error(p.set_val_object("string", o)));
    }

    REQUIRE(!is_error(p.set_val_array("array", a)));

    // Wrong type
    {
        REQUIRE(is_error(p.set_val_bool("array", true)));
        REQUIRE(is_error(p.set_val_int("array", 1)));
        REQUIRE(is_error(p.set_val_string("array", "string")));
        REQUIRE(is_error(p.set_val_object("array", o)));
    }

    REQUIRE(!is_error(p.set_val_object("object", o)));

    // Wrong type
    {
        REQUIRE(is_error(p.set_val_bool("object", true)));
        REQUIRE(is_error(p.set_val_int("object", 1)));
        REQUIRE(is_error(p.set_val_string("object", "string")));
        REQUIRE(is_error(p.set_val_array("object", a)));
    }

    // Test getters.
    bool bool_val = false;
    int int_val = 0;
    std::string string_val = "";
    Array array_val;
    Object object_val;

    REQUIRE(!is_error(p.val_bool("bool", bool_val)));
    REQUIRE(bool_val == true);
    REQUIRE(!is_error(p.val_int("int", int_val)));
    REQUIRE(int_val == 1);
    REQUIRE(!is_error(p.val_string("string", string_val)));
    REQUIRE(string_val == "string");
    REQUIRE(!is_error(p.val_array("array", array_val)));
    REQUIRE(array_val.get() == a.get());
    REQUIRE(!is_error(p.val_object("object", object_val)));
    REQUIRE(object_val.get() == o.get());

    // Wrong type.
    REQUIRE(is_error(p.val_bool("int", bool_val)));
    REQUIRE(is_error(p.val_bool("string", bool_val)));
    REQUIRE(is_error(p.val_bool("array", bool_val)));
    REQUIRE(is_error(p.val_bool("object", bool_val)));

    REQUIRE(is_error(p.val_int("bool", int_val)));
    REQUIRE(is_error(p.val_int("string", int_val)));
    REQUIRE(is_error(p.val_int("array", int_val)));
    REQUIRE(is_error(p.val_int("object", int_val)));

    REQUIRE(is_error(p.val_string("bool", string_val)));
    REQUIRE(is_error(p.val_string("int", string_val)));
    REQUIRE(is_error(p.val_string("array", string_val)));
    REQUIRE(is_error(p.val_string("object", string_val)));

    REQUIRE(is_error(p.val_array("bool", array_val)));
    REQUIRE(is_error(p.val_array("int", array_val)));
    REQUIRE(is_error(p.val_array("string", array_val)));
    REQUIRE(is_error(p.val_array("object", array_val)));

    REQUIRE(is_error(p.val_object("bool", object_val)));
    REQUIRE(is_error(p.val_object("int", object_val)));
    REQUIRE(is_error(p.val_object("string", object_val)));
    REQUIRE(is_error(p.val_object("array", object_val)));

    // Check types.
    REQUIRE(p.is_val_bool("bool"));
    REQUIRE(!p.is_val_bool("int"));
    REQUIRE(!p.is_val_bool("string"));
    REQUIRE(!p.is_val_bool("array"));
    REQUIRE(!p.is_val_bool("object"));

    REQUIRE(!p.is_val_int("bool"));
    REQUIRE(p.is_val_int("int"));
    REQUIRE(!p.is_val_int("string"));
    REQUIRE(!p.is_val_int("array"));
    REQUIRE(!p.is_val_int("object"));

    REQUIRE(!p.is_val_string("bool"));
    REQUIRE(!p.is_val_string("int"));
    REQUIRE(p.is_val_string("string"));
    REQUIRE(!p.is_val_string("array"));
    REQUIRE(!p.is_val_string("object"));

    REQUIRE(!p.is_val_array("bool"));
    REQUIRE(!p.is_val_array("int"));
    REQUIRE(!p.is_val_array("string"));
    REQUIRE(p.is_val_array("array"));
    REQUIRE(!p.is_val_array("object"));

    REQUIRE(!p.is_val_object("bool"));
    REQUIRE(!p.is_val_object("int"));
    REQUIRE(!p.is_val_object("string"));
    REQUIRE(!p.is_val_object("array"));
    REQUIRE(p.is_val_object("object"));

    // Check clear.
    REQUIRE(!p.is_empty());
    p.clear();
    REQUIRE(p.is_empty());

    const std::string json = "{\"A\":1}";
    REQUIRE(!is_error(p.from_json({json.c_str(), json.size()})));
    const std::string result = p.to_json();
    REQUIRE(json == result);
}

TEST_CASE("message unit tests", "[message]") {}
