#pragma once

#include <boost/lockfree/queue.hpp>
#include <matchingengine/protocol/info/error.h>
#include <matchingengine/protocol/info/info.h>
#include <variant>

namespace tcp
{
    struct InitCommand {
        core::ConnectionId id;
        core::Username username;
    };

    struct InfoCommand {
        core::ConnectionId id;
        core::protocol::info::Info message;
    };

    struct ErrorCommand {
        core::ConnectionId id;
        core::protocol::info::Error message;
    };

    struct StopCommand {
        core::ConnectionId id;
    };

    using Command =
        std::variant<InitCommand, InfoCommand, ErrorCommand, StopCommand>;
} // namespace tcp
