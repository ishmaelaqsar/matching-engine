#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>

namespace orderbook
{
        class TCPClient
        {
        public:
                TCPClient(boost::asio::io_context &io_context, const std::string &host, const unsigned short port) :
                        f_endpoint({boost::asio::ip::make_address(host), port}),
                        f_io_context(io_context),
                        f_socket(boost::asio::ip::tcp::socket(io_context)) {}

                void connect();

                void disconnect();

                [[nodiscard]] bool is_connected() const;

                explicit operator bool() const;

                void write(std::string message);

                size_t read(const boost::asio::mutable_buffer &buffer);

                std::string read_string(size_t max_bytes = 1024);

        private:
                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::socket f_socket;
                bool f_connected = false;
        };

        inline void TCPClient::connect()
        {
                try {
                        f_socket.connect(f_endpoint);
                        f_connected = true;
                } catch (const boost::system::system_error &e) {
                        f_connected = false;
                        throw std::runtime_error("Connection failed: " + std::string(e.what()));
                }
        }

        inline void TCPClient::disconnect()
        {
                if (f_connected) {
                        boost::system::error_code ec;
                        f_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                        f_socket.close(ec);
                        f_connected = false;
                }
        }

        inline bool TCPClient::is_connected() const
        {
                return f_connected;
        }

        inline TCPClient::operator bool() const
        {
                return f_connected;
        }

        inline void TCPClient::write(std::string message)
        {
                if (!f_connected) throw std::runtime_error("TCPClient not connected");
                f_socket.send(boost::asio::buffer(message));
        }

        inline size_t TCPClient::read(const boost::asio::mutable_buffer &buffer)
        {
                if (!f_connected) throw std::runtime_error("Not connected");
                return f_socket.read_some(buffer);
        }

        inline std::string TCPClient::read_string(const size_t max_bytes)
        {
                if (!f_connected) throw std::runtime_error("Not connected");

                std::vector<char> buffer(max_bytes);
                size_t len = f_socket.read_some(boost::asio::buffer(buffer));
                return {buffer.data(), len};
        }
}

#endif //CLIENT_H
