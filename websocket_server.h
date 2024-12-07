#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <uwebsockets/App.h>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <chrono>

class WebSocketServer {
public:
    explicit WebSocketServer(int port);
    void run();

    void broadcast(const std::string& symbol, const nlohmann::json& data);

private:
    int port;
    std::mutex subscriptionMutex;
    std::unordered_map<std::string, std::vector<uWS::WebSocket<false, true>*>> subscriptions;

    void handleMessage(uWS::WebSocket<false, true>* ws, const nlohmann::json& msg);
};

#endif // WEBSOCKET_SERVER_H
