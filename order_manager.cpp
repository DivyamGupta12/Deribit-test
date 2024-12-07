#include "order_manager.h"
#include "C:\Users\Support\Desktop\vsc\goqderibit\curl-8.11.0_4-win64-mingw\include\curl\curl.h"
#include <stdexcept>
#include <iostream>
#include <chrono>

OrderManager::OrderManager(const std::string& apiKey, const std::string& apiSecret)
    : apiKey(apiKey), apiSecret(apiSecret) {}

nlohmann::json OrderManager::sendRequest(const std::string& endpoint, const nlohmann::json& payload) {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to initialize CURL");

    std::string url = baseUrl + endpoint;
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    std::string payloadStr = payload.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* ptr, size_t size, size_t nmemb, void* userdata) {
        ((std::string*)userdata)->append((char*)ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return nlohmann::json::parse(response);
}

nlohmann::json OrderManager::placeOrder(const std::string& instrument, const std::string& orderType, double amount, double price) {
    auto start = std::chrono::high_resolution_clock::now();

    nlohmann::json result = sendRequest("/private/buy", {
        {"instrument_name", instrument},
        {"type", orderType},
        {"amount", amount},
        {"price", price}
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Order Placement Latency: " << duration.count() << " ms\n";

    return result;
}

nlohmann::json OrderManager::cancelOrder(const std::string& orderId) {
    auto start = std::chrono::high_resolution_clock::now();

    nlohmann::json result = sendRequest("/private/cancel", {{"order_id", orderId}});

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Cancel Order Latency: " << duration.count() << " ms\n";

    return result;
}

nlohmann::json OrderManager::modifyOrder(const std::string& orderId, double newPrice) {
    auto start = std::chrono::high_resolution_clock::now();

    nlohmann::json result = sendRequest("/private/edit", {
        {"order_id", orderId},
        {"price", newPrice}
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Modify Order Latency: " << duration.count() << " ms\n";

    return result;
}

nlohmann::json OrderManager::getOrderBook(const std::string& instrument) {
    auto start = std::chrono::high_resolution_clock::now();

    nlohmann::json result = sendRequest("/public/get_order_book", {{"instrument_name", instrument}});

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Order Book Fetch Latency: " << duration.count() << " ms\n";

    return result;
}

nlohmann::json OrderManager::getCurrentPositions() {
    auto start = std::chrono::high_resolution_clock::now();

    nlohmann::json result = sendRequest("/private/get_positions", {});

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Get Current Positions Latency: " << duration.count() << " ms\n";

    return result;
}
