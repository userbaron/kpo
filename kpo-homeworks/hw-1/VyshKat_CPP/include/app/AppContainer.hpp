#ifndef APPCONTAINER_HPP
#define APPCONTAINER_HPP

#include "services/FleetService.hpp"
#include "services/ServiceCenterInspector.hpp"
#include "storage/InMemoryInventoryRepository.hpp"

class AppContainer final {
public:
    FleetService& fleet() noexcept { return fleet_; }
    [[nodiscard]] const FleetService& fleet() const noexcept { return fleet_; }

private:
    InMemoryInventoryRepository repository_;
    ServiceCenterInspector inspector_;
    FleetService fleet_{repository_, inspector_};
};

#endif
