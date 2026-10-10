#ifndef SERVICECENTERINSPECTOR_HPP
#define SERVICECENTERINSPECTOR_HPP

#include "services/ITechnicalInspector.hpp"

class ServiceCenterInspector final : public ITechnicalInspector {
public:
    [[nodiscard]] bool approve(const Transport& transport) const override;
};

#endif
