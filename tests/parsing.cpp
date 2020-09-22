#include <catch.hpp>
#include <util.h>

#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/object.h>
#include <one/game/parsing.h>

using namespace game;
using namespace i3d::one;

TEST_CASE("ancillary message payload parsing", "[parsing]") {
    {  // Allocated payload.
        const std::string map_str = "islands_large";
        const std::string max_players_str = "16";

        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", map_str);
        REQUIRE(!is_error(err));

        Object max_players;
        err = max_players.set_val_string("key", "maxPlayers");
        REQUIRE(!is_error(err));
        err = max_players.set_val_string("value", max_players_str);
        REQUIRE(!is_error(err));

        Array data;
        OneArrayPtr ptr = (OneArray *)&data;
        OneServerWrapper::AllocatedData allocated;

        auto callback = [&](const size_t total_number_of_keys, const std::string &key,
                            const std::string &value) {
            if (total_number_of_keys != 2) {
                L_ERROR("got total number of keys(" +
                        std::to_string(total_number_of_keys) + ") expected 2 instead");
                return false;
            }

            if (key == "map") {
                allocated.map = value;
                return true;
            }

            if (key == "maxPlayers") {
                allocated.max_players = value;
                return true;
            }

            L_ERROR("key(" + key + ") is not handled");
            return false;
        };

        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));
        data.push_back_object(map);
        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));
        data.push_back_object(max_players);
        REQUIRE(Parsing::extract_key_value_payload(ptr, callback));
        REQUIRE(allocated.map == map_str);
        REQUIRE(allocated.max_players == max_players_str);
        data.push_back_object(max_players);
        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));
    }

    {  // MetaData payload.
        const std::string map_str = "islands_large";
        const std::string mode_str = "BR";
        const std::string type_str = "squads";

        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", map_str);
        REQUIRE(!is_error(err));

        Object mode;
        err = mode.set_val_string("key", "mode");
        REQUIRE(!is_error(err));
        err = mode.set_val_string("value", mode_str);
        REQUIRE(!is_error(err));

        Object type;
        err = type.set_val_string("key", "type");
        REQUIRE(!is_error(err));
        err = type.set_val_string("value", type_str);
        REQUIRE(!is_error(err));

        Array data;
        OneArrayPtr ptr = (OneArray *)&data;
        OneServerWrapper::MetaDataData meta_data;
        auto callback = [&](const size_t total_number_of_keys, const std::string &key,
                            const std::string &value) {
            if (total_number_of_keys != 3) {
                L_ERROR("got total number of keys(" +
                        std::to_string(total_number_of_keys) + ") expected 3 instead");
                return false;
            }

            if (key == "map") {
                meta_data.map = value;
                return true;
            }

            if (key == "mode") {
                meta_data.mode = value;
                return true;
            }

            if (key == "type") {
                meta_data.type = value;
                return true;
            }

            L_ERROR("key(" + key + ") is not handled");
            return false;
        };
        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));

        data.push_back_object(map);
        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));

        data.push_back_object(mode);
        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));

        data.push_back_object(type);
        REQUIRE(Parsing::extract_key_value_payload(ptr, callback));
        REQUIRE(meta_data.map == map_str);
        REQUIRE(meta_data.mode == mode_str);
        REQUIRE(meta_data.type == type_str);
        data.push_back_object(type);
        REQUIRE(!Parsing::extract_key_value_payload(ptr, callback));
    }
}
