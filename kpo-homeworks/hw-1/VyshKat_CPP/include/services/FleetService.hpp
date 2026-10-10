#ifndef FLEETSERVICE_HPP
#define FLEETSERVICE_HPP

#include "domain/Equipment.hpp"
#include "domain/Transport.hpp"
#include "services/ITechnicalInspector.hpp"
#include "storage/IInventoryRepository.hpp"

#include <memory>
#include <vector>

enum class AdmissionResult { Accepted, Rejected };

class FleetService {
public:
    FleetService(IInventoryRepository& repository, const ITechnicalInspector& inspector);

    AdmissionResult addTransport(std::unique_ptr<Transport> transport);
    void addEquipment(std::unique_ptr<Equipment> equipment);

    [[nodiscard]] std::vector<const IInventoryItem*> inventory() const;
    [[nodiscard]] std::vector<const Transport*> beginnerFriendlyTransport() const;
    [[nodiscard]] double totalDailyEnergyKWh() const;

private:
    void ensureUnique(const IInventoryItem& item) const;

    IInventoryRepository& repository_;
    const ITechnicalInspector& inspector_;
};

#endif
