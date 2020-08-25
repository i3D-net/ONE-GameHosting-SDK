#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/arcus/array.h>
#include <one/arcus/c_api.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/opcode.h>

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

TEST_CASE("message unit tests", "[message]") {
    Message m;
    REQUIRE(m.code() == Opcode::invalid);
    const std::string json = "{\"timeout\":1000}";
    REQUIRE(!is_error(m.init(Opcode::soft_stop_request, {json.c_str(), json.size()})));
    REQUIRE(m.code() == Opcode::soft_stop_request);
    auto p = m.payload();
    Payload copy = p;
    REQUIRE(!p.is_empty());
    int timeout = 0;
    REQUIRE(!is_error(p.val_int("timeout", timeout)));
    REQUIRE(timeout == 1000);
    m.reset();
    REQUIRE(m.code() == Opcode::invalid);
    REQUIRE(m.payload().is_empty());
    REQUIRE(!is_error(m.init(Opcode::soft_stop_request, copy)));
    REQUIRE(m.code() == Opcode::soft_stop_request);
    p = m.payload();
    REQUIRE(!is_error(p.val_int("timeout", timeout)));
    REQUIRE(timeout == 1000);
}

TEST_CASE("message prepare", "[message]") {
    Message m;
    REQUIRE(messages::prepare_error_response(m) == 0);
    REQUIRE(m.code() == Opcode::error_response);
    REQUIRE(m.payload().is_empty() == true);

    m.reset();
    REQUIRE(messages::prepare_soft_stop_request(1000, m) == 0);
    REQUIRE(m.code() == Opcode::soft_stop_request);
    auto p = m.payload();
    REQUIRE(p.is_empty() == false);
    int timeout = 0;
    REQUIRE(!is_error(p.val_int("timeout", timeout)));
    REQUIRE(timeout == 1000);

    m.reset();
    Array allocated;
    REQUIRE(messages::prepare_allocated_request(allocated, m) == 0);
    REQUIRE(m.code() == Opcode::allocated_request);
    p = m.payload();
    REQUIRE(p.is_empty() == false);
    REQUIRE(p.is_val_array("data"));

    m.reset();
    Array meta_data;
    REQUIRE(messages::prepare_meta_data_request(meta_data, m) == 0);
    REQUIRE(m.code() == Opcode::meta_data_request);
    p = m.payload();
    REQUIRE(p.is_empty() == false);
    REQUIRE(p.is_val_array("data"));

    m.reset();
    REQUIRE(messages::prepare_live_state_request(m) == 0);
    REQUIRE(m.code() == Opcode::live_state_request);
    REQUIRE(m.payload().is_empty() == true);

    m.reset();
    const int player = 1;
    const int max_player = 16;
    const std::string name = "test name";
    const std::string map = "test map";
    const std::string mode = "test mode";
    const std::string version = "test version";

    REQUIRE(messages::prepare_live_state_response(player, max_player, name.c_str(),
                                                  map.c_str(), mode.c_str(),
                                                  version.c_str(), m) == 0);
    REQUIRE(m.code() == Opcode::live_state_response);
    p = m.payload();
    REQUIRE(p.is_empty() == false);

    int int_val = 0;
    REQUIRE(!is_error(p.val_int("player", int_val)));
    REQUIRE(int_val == player);
    REQUIRE(!is_error(p.val_int("max_player", int_val)));
    REQUIRE(int_val == max_player);
    std::string val;
    REQUIRE(!is_error(p.val_string("name", val)));
    REQUIRE(val == name);
    REQUIRE(!is_error(p.val_string("map", val)));
    REQUIRE(val == map);
    REQUIRE(!is_error(p.val_string("mode", val)));
    REQUIRE(val == mode);
    REQUIRE(!is_error(p.val_string("version", val)));
    REQUIRE(val == version);
}

TEST_CASE("message c_api", "[message]") {
    OneMessagePtr m = nullptr;
    REQUIRE(!is_error(one_message_create(&m)));
    REQUIRE(m != nullptr);

    const std::string json = "{\"timeout\":1000}";
    const int original_code = static_cast<int>(Opcode::soft_stop_request);
    REQUIRE(!is_error(one_message_init(m, original_code, json.c_str(), json.size())));
    int code = 0;
    REQUIRE(!is_error(one_message_code(m, &code)));
    REQUIRE(code == original_code);
    REQUIRE(!is_error(one_message_reset(m)));

    const bool boolean = true;
    const int integer = 1;
    const std::string string = "test";
    const Array array;
    const Object object;

    // Check nullptr.
    REQUIRE(is_error(one_message_create(nullptr)));

    unsigned int size = 0;
    const char *data = "{}";
    REQUIRE(is_error(one_message_init(nullptr, code, data, size)));
    REQUIRE(is_error(one_message_init(m, code, nullptr, size)));

    REQUIRE(is_error(one_message_reset(nullptr)));
    REQUIRE(is_error(one_message_code(nullptr, &code)));
    REQUIRE(is_error(one_message_code(m, nullptr)));

    const char key[] = {'a'};
    bool result = false;

    bool val_boolean = false;
    int val_integer = 0;
    Array val_array;
    Object val_object;

    REQUIRE(is_error(one_message_is_val_bool(nullptr, key, &result)));
    REQUIRE(is_error(one_message_is_val_bool(m, nullptr, &result)));
    REQUIRE(is_error(one_message_is_val_bool(m, key, nullptr)));
    REQUIRE(is_error(one_message_is_val_int(nullptr, key, &result)));
    REQUIRE(is_error(one_message_is_val_int(m, nullptr, &result)));
    REQUIRE(is_error(one_message_is_val_int(m, key, nullptr)));
    REQUIRE(is_error(one_message_is_val_string(nullptr, key, &result)));
    REQUIRE(is_error(one_message_is_val_string(m, nullptr, &result)));
    REQUIRE(is_error(one_message_is_val_string(m, key, nullptr)));
    REQUIRE(is_error(one_message_is_val_array(nullptr, key, &result)));
    REQUIRE(is_error(one_message_is_val_array(m, nullptr, &result)));
    REQUIRE(is_error(one_message_is_val_array(m, key, nullptr)));
    REQUIRE(is_error(one_message_is_val_object(nullptr, key, &result)));
    REQUIRE(is_error(one_message_is_val_object(m, nullptr, &result)));
    REQUIRE(is_error(one_message_is_val_object(m, key, nullptr)));

    REQUIRE(is_error(one_message_val_bool(nullptr, key, &val_boolean)));
    REQUIRE(is_error(one_message_val_bool(m, nullptr, &val_boolean)));
    REQUIRE(is_error(one_message_val_bool(m, key, nullptr)));
    REQUIRE(is_error(one_message_val_int(nullptr, key, &val_integer)));
    REQUIRE(is_error(one_message_val_int(m, nullptr, &val_integer)));
    REQUIRE(is_error(one_message_val_int(m, key, nullptr)));
    char *val;
    REQUIRE(is_error(one_message_val_string(nullptr, key, &val, size)));
    REQUIRE(is_error(one_message_val_string(m, nullptr, &val, size)));
    REQUIRE(is_error(one_message_val_string(m, key, nullptr, size)));
    OneArrayPtr a_ptr = (OneArray *)&array;
    REQUIRE(is_error(one_message_val_array(nullptr, key, a_ptr)));
    REQUIRE(is_error(one_message_val_array(m, nullptr, a_ptr)));
    REQUIRE(is_error(one_message_val_array(m, key, nullptr)));
    OneObjectPtr o_ptr = (OneObject *)&object;
    REQUIRE(is_error(one_message_val_object(nullptr, key, o_ptr)));
    REQUIRE(is_error(one_message_val_object(m, nullptr, o_ptr)));
    REQUIRE(is_error(one_message_val_object(m, key, nullptr)));

    REQUIRE(is_error(one_message_set_val_bool(nullptr, key, val_boolean)));
    REQUIRE(is_error(one_message_set_val_bool(m, nullptr, val_boolean)));
    REQUIRE(is_error(one_message_set_val_int(nullptr, key, val_integer)));
    REQUIRE(is_error(one_message_set_val_int(m, nullptr, val_integer)));
    REQUIRE(is_error(one_message_set_val_string(nullptr, key, string.c_str())));
    REQUIRE(is_error(one_message_set_val_string(m, nullptr, string.c_str())));
    REQUIRE(is_error(one_message_set_val_string(m, key, nullptr)));
    REQUIRE(is_error(one_message_set_val_array(nullptr, key, a_ptr)));
    REQUIRE(is_error(one_message_set_val_array(m, nullptr, a_ptr)));
    REQUIRE(is_error(one_message_set_val_array(m, key, nullptr)));
    REQUIRE(is_error(one_message_set_val_object(nullptr, key, o_ptr)));
    REQUIRE(is_error(one_message_set_val_object(m, nullptr, o_ptr)));
    REQUIRE(is_error(one_message_set_val_object(m, key, nullptr)));

    REQUIRE(is_error(one_message_prepare_error_response(nullptr)));
    REQUIRE(is_error(one_message_prepare_live_state_response(1, 12, nullptr, "map",
                                                             "mode", "version", m)));
    REQUIRE(is_error(one_message_prepare_live_state_response(1, 12, "name", nullptr,
                                                             "mode", "version", m)));
    REQUIRE(is_error(one_message_prepare_live_state_response(1, 12, "name", "map",
                                                             nullptr, "version", m)));
    REQUIRE(is_error(one_message_prepare_live_state_response(1, 12, "name", "map", "mode",
                                                             nullptr, m)));
    REQUIRE(is_error(one_message_prepare_live_state_response(1, 12, "name", "map", "mode",
                                                             "version", nullptr)));
    REQUIRE(is_error(one_message_prepare_host_information_request(nullptr)));

    // Check Setters.
    REQUIRE(!is_error(one_message_set_val_bool(m, "bool", boolean)));
    REQUIRE(!is_error(one_message_set_val_int(m, "int", integer)));
    REQUIRE(!is_error(one_message_set_val_string(m, "string", string.c_str())));
    REQUIRE(!is_error(one_message_set_val_array(m, "array", (OneArrayPtr)&array)));
    REQUIRE(!is_error(one_message_set_val_object(m, "object", (OneObjectPtr)&object)));

    // Check Getters.
    result = false;
    REQUIRE(!is_error(one_message_is_val_bool(m, "bool", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_message_is_val_int(m, "int", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_message_is_val_string(m, "string", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_message_is_val_array(m, "array", &result)));
    REQUIRE(result == true);
    REQUIRE(!is_error(one_message_is_val_object(m, "object", &result)));
    REQUIRE(result == true);

    REQUIRE(!is_error(one_message_val_bool(m, "bool", &val_boolean)));
    REQUIRE(boolean == val_boolean);
    REQUIRE(!is_error(one_message_val_int(m, "int", &val_integer)));
    REQUIRE(integer == val_integer);
    size_t val_size = 0;
    REQUIRE(!is_error(one_message_val_string_size(m, "string", &val_size)));
    REQUIRE(val_size == string.size() + 1);
    char *val_string = new char[string.size() + 1];
    REQUIRE(val_string != nullptr);
    REQUIRE(
        !is_error(one_message_val_string(m, "string", &val_string, string.size() + 1)));
    REQUIRE(string == std::string(val_string));
    delete[] val_string;
    REQUIRE(!is_error(one_message_val_array(m, "array", (OneArrayPtr)&val_array)));
    REQUIRE(array.get() == val_array.get());
    REQUIRE(!is_error(one_message_val_object(m, "object", (OneObjectPtr)&val_object)));
    REQUIRE(object.get() == val_object.get());

    REQUIRE(!is_error(one_message_reset(m)));

    REQUIRE(!is_error(one_message_prepare_error_response(m)));
    REQUIRE(!is_error(one_message_prepare_live_state_response(1, 16, "name", "map",
                                                              "mode", "version", m)));
    REQUIRE(!is_error(one_message_prepare_host_information_request(m)));

    one_message_destroy(&m);
    REQUIRE(m == nullptr);
    one_message_destroy(nullptr);
}
