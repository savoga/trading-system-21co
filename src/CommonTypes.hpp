#pragma once

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_

#include <nlohmann/json.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

using json = nlohmann::json;
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
using websocketpp::connection_hdl;