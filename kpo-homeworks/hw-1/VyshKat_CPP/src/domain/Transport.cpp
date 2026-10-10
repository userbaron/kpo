#include "domain/Transport.hpp"

#include <stdexcept>
#include <utility>

Transport::Transport(std::string number, std::string name, int beginnerScore, bool operational)
    : InventoryItem(std::move(number), std::move(name)),
      beginnerScore_(beginnerScore), operational_(operational) {
    if (beginnerScore_ < 1 || beginnerScore_ > 10) {
        throw std::invalid_argument("Beginner score must be between 1 and 10");
    }
}

ItemCategory Transport::category() const noexcept { return ItemCategory::Transport; }
int Transport::beginnerScore() const noexcept { return beginnerScore_; }
bool Transport::suitableForBeginners() const noexcept { return beginnerScore_ >= 6; }
bool Transport::operational() const noexcept { return operational_; }

Scooter::Scooter(std::string number, std::string name, int beginnerScore, bool operational, double dailyKWh)
    : Transport(std::move(number), std::move(name), beginnerScore, operational), EnergyConsumer(dailyKWh) {}
std::string Scooter::type() const { return "Scooter"; }

Bicycle::Bicycle(std::string number, std::string name, int beginnerScore, bool operational)
    : Transport(std::move(number), std::move(name), beginnerScore, operational) {}
std::string Bicycle::type() const { return "Bicycle"; }

ElectricBicycle::ElectricBicycle(std::string number, std::string name, int beginnerScore,
                                 bool operational, double dailyKWh)
    : Transport(std::move(number), std::move(name), beginnerScore, operational), EnergyConsumer(dailyKWh) {}
std::string ElectricBicycle::type() const { return "Electric bicycle"; }

