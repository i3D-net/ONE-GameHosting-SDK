#include <catch.hpp>
#include <util.h>

#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/object.h>

#include <string>

using namespace one;

TEST_CASE("object unit tests", "[object]") {
    Object o;
    REQUIRE(o.is_empty());

    const bool boolean = true;
    const int integer = 1;
    const std::string string = "string";
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
    int val_interger = 1;
    std::string val_string = "";
    Array val_arrray;
    Object val_object;

    REQUIRE(!is_error(o.val_bool("bool", val_boolean)));
    REQUIRE(val_boolean == boolean);
    REQUIRE(is_error(o.val_int("bool", val_interger)));
    REQUIRE(is_error(o.val_string("bool", val_string)));
    REQUIRE(is_error(o.val_array("bool", val_arrray)));
    REQUIRE(is_error(o.val_object("bool", val_object)));

    REQUIRE(is_error(o.val_bool("int", val_boolean)));
    REQUIRE(!is_error(o.val_int("int", val_interger)));
    REQUIRE(val_interger == integer);
    REQUIRE(is_error(o.val_string("int", val_string)));
    REQUIRE(is_error(o.val_array("int", val_arrray)));
    REQUIRE(is_error(o.val_object("int", val_object)));

    REQUIRE(is_error(o.val_bool("string", val_boolean)));
    REQUIRE(is_error(o.val_int("string", val_interger)));
    REQUIRE(!is_error(o.val_string("string", val_string)));
    REQUIRE(val_string == string);
    REQUIRE(is_error(o.val_array("string", val_arrray)));
    REQUIRE(is_error(o.val_object("string", val_object)));

    REQUIRE(is_error(o.val_bool("array", val_boolean)));
    REQUIRE(is_error(o.val_int("array", val_interger)));
    REQUIRE(is_error(o.val_string("array", val_string)));
    REQUIRE(!is_error(o.val_array("array", val_arrray)));
    REQUIRE(val_arrray.get() == array.get());
    REQUIRE(is_error(o.val_object("array", val_object)));

    REQUIRE(is_error(o.val_bool("object", val_boolean)));
    REQUIRE(is_error(o.val_int("object", val_interger)));
    REQUIRE(is_error(o.val_string("object", val_string)));
    REQUIRE(is_error(o.val_array("object", val_arrray)));
    REQUIRE(!is_error(o.val_object("object", val_object)));
    REQUIRE(val_object.get() == object.get());

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
