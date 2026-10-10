#include "services/FleetService.hpp"
#include "domain/EnergyConsumer.hpp"

#include <stdexcept>
#include <utility>

FleetService::FleetService(IInventoryRepository& repository, const ITechnicalInspector& inspector)
    : repository_(repository), inspector_(inspector) {}

void FleetService::ensureUnique(const IInventoryItem& item) const {
    if (repository_.contains(item.inventoryNumber())) {
        throw std::invalid_argument("Duplicate inventory number: " + item.inventoryNumber());
    }
}

AdmissionResult FleetService::addTransport(std::unique_ptr<Transport> transport) {
    if (!transport) {
        throw std::invalid_argument("Transport is null");
    }
    ensureUnique(*transport);
    if (!inspector_.approve(*transport)) {
        return AdmissionResult::Rejected;
    }
    repository_.save(std::move(transport));
    return AdmissionResult::Accepted;
}

void FleetService::addEquipment(std::unique_ptr<Equipment> equipment) {
    if (!equipment) {
        throw std::invalid_argument("Equipment is null");
    }
    ensureUnique(*equipment);
    repository_.save(std::move(equipment));
}

std::vector<const IInventoryItem*> FleetService::inventory() const {
    return repository_.list();
}

std::vector<const Transport*> FleetService::beginnerFriendlyTransport() const {
    std::vector<const Transport*> result;
    for (const IInventoryItem* item : repository_.list()) {
        const auto* transport = dynamic_cast<const Transport*>(item);
        if (transport != nullptr && transport->suitableForBeginners()) {
            result.push_back(transport);
        }
    }
    return result;
}

double FleetService::totalDailyEnergyKWh() const {
    double total = 0.0;
    for (const IInventoryItem* item : repository_.list()) {
        const auto* consumer = dynamic_cast<const IEnergyConsumer*>(item);
        if (consumer != nullptr) {
            total += consumer->dailyEnergyKWh();
        }
    }
    return total;
}

