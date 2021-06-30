#include <catch.hpp>
#include <tests/one/arcus/util.h>

#include <one/arcus/error.h>
#include <one/arcus/array.h>
#include <one/arcus/c_api.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/opcode.h>
#include <one/arcus/types.h>

using namespace i3d::one;

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
    String string_val = "";
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

    const String json = "{\"A\":1}";
    REQUIRE(!is_error(p.from_json({json.c_str(), json.size()})));
    const String result = p.to_json();
    REQUIRE(json == result);

    p.clear();
    REQUIRE(!p.set_val_root_object(o));
    int int_copy = 0;
    REQUIRE(!is_error(p.val_int("int", int_copy)));
    REQUIRE(int_copy == 1);
    Object o_copy;
    REQUIRE(!p.val_root_object(o_copy));
    REQUIRE(o.get() == o_copy.get());
}

TEST_CASE("message unit tests", "[message]") {
    Message m;
    REQUIRE(m.code() == Opcode::invalid);
    const String json = "{\"timeout\":1000}";
    REQUIRE(!is_error(m.init(Opcode::soft_stop, {json.c_str(), json.size()})));
    REQUIRE(m.code() == Opcode::soft_stop);
    auto p = m.payload();
    Payload copy = p;
    REQUIRE(!p.is_empty());
    int timeout = 0;
    REQUIRE(!is_error(p.val_int("timeout", timeout)));
    REQUIRE(timeout == 1000);
    m.reset();
    REQUIRE(m.code() == Opcode::invalid);
    REQUIRE(m.payload().is_empty());
    REQUIRE(!is_error(m.init(Opcode::soft_stop, copy)));
    REQUIRE(m.code() == Opcode::soft_stop);
    p = m.payload();
    REQUIRE(!is_error(p.val_int("timeout", timeout)));
    REQUIRE(timeout == 1000);
}

TEST_CASE("message prepare", "[message]") {
    Message m;

    {  // soft_stop
        m.reset();
        REQUIRE(!is_error(messages::prepare_soft_stop(1000, m)));
        REQUIRE(m.code() == Opcode::soft_stop);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);
        int timeout = 0;
        REQUIRE(!is_error(p.val_int("timeout", timeout)));
        REQUIRE(timeout == 1000);
    }

    {  // allocated
        m.reset();
        Array allocated;
        REQUIRE(!is_error(messages::prepare_allocated(allocated, m)));
        REQUIRE(m.code() == Opcode::allocated);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);
        REQUIRE(p.is_val_array("data"));
    }

    {  // metadata
        m.reset();
        Array metadata;
        REQUIRE(!is_error(messages::prepare_metadata(metadata, m)));
        REQUIRE(m.code() == Opcode::metadata);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);
        REQUIRE(p.is_val_array("data"));
    }

    {  // reverse metadata
        m.reset();
        Array reverse_metadata;
        REQUIRE(!is_error(messages::prepare_reverse_metadata(reverse_metadata, m)));
        REQUIRE(m.code() == Opcode::reverse_metadata);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);
        REQUIRE(p.is_val_array("data"));
    }

    {  // live_state
        m.reset();
        const int players = 1;
        const int max_players = 16;
        const String name = "test name";
        const String map = "test map";
        const String mode = "test mode";
        const String version = "test version";

        REQUIRE(!is_error(messages::prepare_live_state(players, max_players, name.c_str(),
                                                       map.c_str(), mode.c_str(),
                                                       version.c_str(), nullptr, m)));
        REQUIRE(m.code() == Opcode::live_state);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);

        int int_val = 0;
        REQUIRE(!is_error(p.val_int("players", int_val)));
        REQUIRE(int_val == players);
        REQUIRE(!is_error(p.val_int("maxPlayers", int_val)));
        REQUIRE(int_val == max_players);
        String val;
        REQUIRE(!is_error(p.val_string("name", val)));
        REQUIRE(val == name);
        REQUIRE(!is_error(p.val_string("map", val)));
        REQUIRE(val == map);
        REQUIRE(!is_error(p.val_string("mode", val)));
        REQUIRE(val == mode);
        REQUIRE(!is_error(p.val_string("version", val)));
        REQUIRE(val == version);
    }

    {  // host_information
        m.reset();

        Object object;

        REQUIRE(!is_error(messages::prepare_host_information(object, m)));
        REQUIRE(m.code() == Opcode::host_information);
        auto p = m.payload();
        REQUIRE(p.is_empty() == true);
    }

    {  // application_instance_information
        m.reset();

        Object object;

        REQUIRE(!is_error(messages::prepare_application_instance_information(object, m)));
        REQUIRE(m.code() == Opcode::application_instance_information);
        auto p = m.payload();
        REQUIRE(p.is_empty() == true);
    }

    {  // application_instance_status
        m.reset();

        const int status = 4;

        REQUIRE(!is_error(messages::prepare_application_instance_status(status, m)));
        REQUIRE(m.code() == Opcode::application_instance_status);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);

        int s = 0;
        REQUIRE(!is_error(p.val_int("status", s)));
        REQUIRE(s == status);
    }

    {  // custom command
        m.reset();
        Array custom_command;
        REQUIRE(!is_error(messages::prepare_custom_command(custom_command, m)));
        REQUIRE(m.code() == Opcode::custom_command);
        auto p = m.payload();
        REQUIRE(p.is_empty() == false);
        REQUIRE(p.is_val_array("data"));
    }
}
