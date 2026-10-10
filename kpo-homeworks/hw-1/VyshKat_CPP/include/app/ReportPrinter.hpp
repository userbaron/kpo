#ifndef REPORTPRINTER_HPP
#define REPORTPRINTER_HPP

#include "services/FleetService.hpp"
#include <iosfwd>

class ReportPrinter final {
public:
    static void inventory(const FleetService& fleet, std::ostream& out);
    static void energy(const FleetService& fleet, std::ostream& out);
    static void beginners(const FleetService& fleet, std::ostream& out);
};

#endif
