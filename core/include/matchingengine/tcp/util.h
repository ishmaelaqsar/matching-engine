#pragma once

#include <boost/lockfree/queue.hpp>
#include <variant>

#include <matchingengine/protocol/info/error.h>
#include <matchingengine/protocol/info/info.h>

namespace tcp
{
        using InfoMessages = std::variant<core::protocol::info::Info, core::protocol::info::Error>;

        struct InfoCommand
        {
                std::pair<core::ConnectionId, InfoMessages> id_message;
        };

        struct InitCommand
        {
                std::tuple<core::ConnectionId, std::string_view, core::protocol::info::Info> id_username;
        };

        struct StopCommand
        {
                core::ConnectionId connection_id;
        };

        using Messages = std::variant<InfoCommand, StopCommand>;
} // namespace tcp
