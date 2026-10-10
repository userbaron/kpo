#include "domain/Equipment.hpp"

#include <utility>

Equipment::Equipment(std::string number, std::string name)
    : InventoryItem(std::move(number), std::move(name)) {}
ItemCategory Equipment::category() const noexcept { return ItemCategory::Equipment; }

Helmet::Helmet(std::string number, std::string name)
    : Equipment(std::move(number), std::move(name)) {}
std::string Helmet::type() const { return "Helmet"; }

DockStation::DockStation(std::string number, std::string name, double dailyKWh)
    : Equipment(std::move(number), std::move(name)), EnergyConsumer(dailyKWh) {}
std::string DockStation::type() const { return "Dock station"; }

ChargingCabinet::ChargingCabinet(std::string number, std::string name, double dailyKWh)
    : Equipment(std::move(number), std::move(name)), EnergyConsumer(dailyKWh) {}
std::string ChargingCabinet::type() const { return "Charging cabinet"; }

