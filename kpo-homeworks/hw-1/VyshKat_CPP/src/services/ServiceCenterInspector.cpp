#include "services/ServiceCenterInspector.hpp"
#include "domain/Transport.hpp"

bool ServiceCenterInspector::approve(const Transport& transport) const {
    return transport.operational();
}

