#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <string>
#include <nlohmann/json.hpp>

class OrderManager {
public:
    OrderManager(const std::string& apiKey, const std::string& apiSecret);

    nlohmann::json placeOrder(const std::string& instrument, const std::string& orderType, double amount, double price);
    nlohmann::json cancelOrder(const std::string& orderId);
    nlohmann::json modifyOrder(const std::string& orderId, double newPrice);
    nlohmann::json getOrderBook(const std::string& instrument);
    nlohmann::json getCurrentPositions();

private:
    std::string apiKey;
    std::string apiSecret;
    const std::string baseUrl = "https://test.deribit.com/api/v2";

    nlohmann::json sendRequest(const std::string& endpoint, const nlohmann::json& payload);
};

#endif // ORDER_MANAGER_H
