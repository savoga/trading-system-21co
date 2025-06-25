#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>

#include "BinanceWebSocket.hpp"
#include "CommonTypes.hpp"

BinanceWebSocket::BinanceWebSocket() {
        c.init_asio();
        c.set_tls_init_handler([](websocketpp::connection_hdl) {
            return websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(
                websocketpp::lib::asio::ssl::context::tlsv12_client);
        });

        c.set_message_handler([this](connection_hdl, client::message_ptr msg) {
            std::lock_guard<std::mutex> lock(mtx);
            message_buffer.push_back(msg->get_payload());
            if (message_buffer.size() > max_buffer_size) message_buffer.pop_front();
            cv.notify_one();
        });
    }
void BinanceWebSocket::start() {
        std::string uri = "wss://stream.binance.com:9443/ws/btcusdt@depth@100ms";
        websocketpp::lib::error_code ec;
        auto con = c.get_connection(uri, ec);
        if (ec) throw std::runtime_error("Connection error: " + ec.message());
        c.connect(con);
        ws_thread = std::thread([this]() {
            try {
                c.run();
            } catch (const std::exception& e) {
                std::cerr << "WebSocket run() error: " << e.what() << std::endl;
            }
        });
    }
void BinanceWebSocket::join() { ws_thread.join(); }