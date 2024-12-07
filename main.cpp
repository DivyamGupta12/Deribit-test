#include "order_manager.h"
#include "websocket_server.h"
#include <thread>
#include <chrono>

int main() {
    OrderManager manager("<API_KEY>", "<API_SECRET>");
    WebSocketServer server(9001);

    std::thread serverThread([&server]() { server.run(); });

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        nlohmann::json orderBook = manager.getOrderBook("BTC-PERPETUAL");
        server.broadcast("BTC-PERPETUAL", orderBook);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "End-to-End Latency: " << duration.count() << " ms\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    serverThread.join();
    return 0;
}
