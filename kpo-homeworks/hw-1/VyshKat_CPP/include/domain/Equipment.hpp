#ifndef EQUIPMENT_HPP
#define EQUIPMENT_HPP

#include "domain/EnergyConsumer.hpp"
#include "domain/InventoryItem.hpp"

#include <string>

class Equipment : public InventoryItem {
public:
    Equipment(std::string number, std::string name);
    [[nodiscard]] ItemCategory category() const noexcept final;
};

class Helmet final : public Equipment {
public:
    Helmet(std::string number, std::string name);
    [[nodiscard]] std::string type() const override;
};

class DockStation final : public Equipment, public EnergyConsumer {
public:
    DockStation(std::string number, std::string name, double dailyKWh);
    [[nodiscard]] std::string type() const override;
};

class ChargingCabinet final : public Equipment, public EnergyConsumer {
public:
    ChargingCabinet(std::string number, std::string name, double dailyKWh);
    [[nodiscard]] std::string type() const override;
};

#endif
