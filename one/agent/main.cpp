#include <chrono>
#include <thread>

#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/object.h>
#include <one/arcus/types.h>

#include <one/agent/agent.h>
#include <one/agent/log.h>

using namespace std::chrono;
using namespace i3d::one;

void sleep(int ms) {
    std::this_thread::sleep_for(milliseconds(ms));
}

int main(int argc, char **argv) {
    log_info("-----------------------");
    log_info("agent startup");

    const int default_port = 19001;
    int port = default_port;
    bool stressTest = false;

    if (argc >= 2) {
        port = strtol(argv[1], nullptr, 10);

        if (port <= 0) {
            log_error("invalid port provided");
            return 1;
        }

        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "--stress") == 0) {
                stressTest = true;
                break;
            }
        }
    }

    const String address = "127.0.0.1";

    Agent agent;
    auto err = agent.init(address.c_str(), port);
    if (is_error(err)) {
        log_error("failed to init agent.");
        return 1;
    }

    log_info("agent is initialized.");
    log_info("running update loop.");

    auto log_status = [](Client::Status status) {
        String status_str = Client::status_to_string(status);
        log_info("status: " + status_str);
    };
    auto status = agent.client().status();
    log_status(status);

    // Soft stop behavior.
    const auto time_zero = steady_clock::time_point(steady_clock::duration::zero());
    steady_clock::time_point time_to_send_soft_stop = time_zero;
    bool did_send_soft_stop = false;

    int messages_counter = 0;

    while (true) {
        sleep(stressTest ? 1 : 100);

        if (agent.client().status() == Client::Status::ready) {
            if (stressTest) {
                Array metadata;
                metadata.push_back_bool(false);
                metadata.push_back_int(123);
                metadata.push_back_string("Fake data");
                
                agent.send_metadata(metadata);
            } else {
                // Trigger a soft stop when game enters ready state.
                if (time_to_send_soft_stop == time_zero) {
                    time_to_send_soft_stop = steady_clock::now() + seconds(10);
                } else if (!did_send_soft_stop) {
                    if (steady_clock::now() > time_to_send_soft_stop) {
                        agent.send_soft_stop(5);
                        did_send_soft_stop = true;
                    }
                }

                // Randomly tell the game server to become allocated.
                bool shouldSend = std::rand() / ((RAND_MAX + 1u) / 50) == 0;
                if (shouldSend) {
                    switch (messages_counter) {
                        case 0: {
                            Array array;
                            Object players;
                            players.set_val_string("key", "players");
                            players.set_val_string("value", "16");
                            Object duration;
                            duration.set_val_string("key", "duration");
                            duration.set_val_string("value", "20");

                            array.push_back_object(players);
                            array.push_back_object(duration);

                            log_info("sending allocated");
                            agent.send_allocated(array);

                            break;
                        }
                        default: {
                            Array metadata;
                            Array data;
                            data.push_back_bool(false);
                            data.push_back_int(123);
                            data.push_back_string("Fake data");
                            Object header;
                            header.set_val_string("key", "Header");
                            header.set_val_bool("valid", true);
                            header.set_val_int("message_number", messages_counter + 1);
                            header.set_val_array("data", data);

                            metadata.push_back_object(header);

                            log_info("sending metadata");
                            agent.send_metadata(metadata);

                            break;
                        }
                    }

                    ++messages_counter;
                }
            }
        } else {
            time_to_send_soft_stop = time_zero;
            did_send_soft_stop = false;
            messages_counter = 0;
        }

        err = agent.update();
        if (is_error(err)) {
            log_info(error_text(err));
            continue;
        }

        auto old_status = status;
        status = agent.client().status();
        if (status != old_status) {
            log_status(status);
        }
    }

    log_info("-----------------------");
    log_info("agent has been shutdown");
    return 0;
}
