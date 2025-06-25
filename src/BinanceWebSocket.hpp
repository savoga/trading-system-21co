#pragma once

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

#include "CommonTypes.hpp"

#include <iostream>
#include <deque>
#include <map>
#include <mutex>

class BinanceWebSocket {
public:
    std::deque<std::string> message_buffer;
    std::mutex mtx;
    std::condition_variable cv;
    BinanceWebSocket();
    void start();
    void join();
private:
    client c;
    std::thread ws_thread;
    const size_t max_buffer_size = 1000;
};
