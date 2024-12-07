#include "websocket_server.h"
#include <iostream>
#include <algorithm>

WebSocketServer::WebSocketServer(int port) : port(port) {}

void WebSocketServer::run() {
    uWS::App().ws<PerSocketData>("/*", {
        .open = [](auto* ws) {
            std::cout << "Client connected\n";
        },
        .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
            nlohmann::json msg = nlohmann::json::parse(message);
            handleMessage(ws, msg);
        },
        .close = [](auto* ws, int /*code*/, std::string_view /*message*/) {
            std::cout << "Client disconnected\n";
        }
    }).listen(port, [this](auto* token) {
        if (token) {
            std::cout << "WebSocket server listening on port " << port << "\n";
        } else {
            std::cerr << "Failed to listen on port " << port << "\n";
        }
    }).run();
}

void WebSocketServer::handleMessage(uWS::WebSocket<false, true>* ws, const nlohmann::json& msg) {
    std::string action = msg["action"];
    std::string symbol = msg["symbol"];

    std::lock_guard<std::mutex> lock(subscriptionMutex);
    if (action == "subscribe") {
        subscriptions[symbol].push_back(ws);
    } else if (action == "unsubscribe") {
        auto& clients = subscriptions[symbol];
        clients.erase(std::remove(clients.begin(), clients.end(), ws), clients.end());
    }
}

void WebSocketServer::broadcast(const std::string& symbol, const nlohmann::json& data) {
    auto start = std::chrono::high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(subscriptionMutex);
    auto it = subscriptions.find(symbol);
    if (it != subscriptions.end()) {
        for (auto* ws : it->second) {
            ws->send(data.dump(), uWS::OpCode::TEXT);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "WebSocket Propagation Latency: " << duration.count() << " ms\n";
}
