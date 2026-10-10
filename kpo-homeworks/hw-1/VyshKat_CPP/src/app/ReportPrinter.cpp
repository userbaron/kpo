#include "app/ReportPrinter.hpp"

#include <iomanip>
#include <ostream>

void ReportPrinter::inventory(const FleetService& fleet, std::ostream& out) {
    const auto items = fleet.inventory();
    out << "\n=== Inventory (" << items.size() << " items) ===\n";
    if (items.empty()) {
        out << "The fleet is empty.\n";
        return;
    }
    for (const auto* item : items) {
        out << "[" << item->inventoryNumber() << "] " << item->type() << ": " << item->name();
        if (const auto* transport = dynamic_cast<const Transport*>(item)) {
            out << " | beginner score: " << transport->beginnerScore() << "/10";
        }
        out << '\n';
    }
}

void ReportPrinter::energy(const FleetService& fleet, std::ostream& out) {
    const auto previousFlags = out.flags();
    const auto previousPrecision = out.precision();
    out << "Total daily energy consumption: " << std::fixed << std::setprecision(2)
        << fleet.totalDailyEnergyKWh() << " kWh\n";
    out.flags(previousFlags);
    out.precision(previousPrecision);
}

void ReportPrinter::beginners(const FleetService& fleet, std::ostream& out) {
    out << "\n=== Beginner-friendly transport (score >= 6) ===\n";
    const auto items = fleet.beginnerFriendlyTransport();
    if (items.empty()) {
        out << "No suitable transport available.\n";
        return;
    }
    for (const auto* item : items) {
        out << "[" << item->inventoryNumber() << "] " << item->type() << ": "
            << item->name() << " (" << item->beginnerScore() << "/10)\n";
    }
}

