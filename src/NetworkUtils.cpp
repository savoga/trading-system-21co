#include "CommonTypes.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

json fetch_snapshot() {
    namespace ssl = boost::asio::ssl;
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;

    net::io_context ioc;
    ssl::context ctx(ssl::context::tlsv12_client);
    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve("api.binance.com", "443");
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req(http::verb::get, "/api/v3/depth?symbol=BTCUSDT&limit=5000", 11);
    req.set(http::field::host, "api.binance.com");
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(stream, req);
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    try {
        stream.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "TLS shutdown warning: " << e.what() << "\n";
    }
    return json::parse(res.body());
}

