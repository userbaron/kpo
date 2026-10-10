#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include "domain/EnergyConsumer.hpp"
#include "domain/InventoryItem.hpp"

#include <string>

class Transport : public InventoryItem {
public:
    Transport(std::string number, std::string name, int beginnerScore, bool operational);
    [[nodiscard]] ItemCategory category() const noexcept final;
    [[nodiscard]] int beginnerScore() const noexcept;
    [[nodiscard]] bool suitableForBeginners() const noexcept;
    [[nodiscard]] bool operational() const noexcept;

private:
    int beginnerScore_;
    bool operational_;
};

class Scooter final : public Transport, public EnergyConsumer {
public:
    Scooter(std::string number, std::string name, int beginnerScore, bool operational, double dailyKWh);
    [[nodiscard]] std::string type() const override;
};

class Bicycle final : public Transport {
public:
    Bicycle(std::string number, std::string name, int beginnerScore, bool operational);
    [[nodiscard]] std::string type() const override;
};

class ElectricBicycle final : public Transport, public EnergyConsumer {
public:
    ElectricBicycle(std::string number, std::string name, int beginnerScore, bool operational, double dailyKWh);
    [[nodiscard]] std::string type() const override;
};

#endif
