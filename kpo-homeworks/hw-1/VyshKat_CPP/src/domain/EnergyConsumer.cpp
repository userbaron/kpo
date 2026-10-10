#include "domain/EnergyConsumer.hpp"

#include <cmath>
#include <stdexcept>

EnergyConsumer::EnergyConsumer(double dailyKWh) : dailyKWh_(dailyKWh) {
    if (!std::isfinite(dailyKWh_) || dailyKWh_ < 0.0) {
        throw std::invalid_argument("Energy consumption must be a finite non-negative number");
    }
}

double EnergyConsumer::dailyEnergyKWh() const noexcept { return dailyKWh_; }

