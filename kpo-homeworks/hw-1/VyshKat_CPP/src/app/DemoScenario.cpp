#include "app/DemoScenario.hpp"

#include "app/AppContainer.hpp"
#include "app/ReportPrinter.hpp"
#include "domain/Equipment.hpp"
#include "domain/Transport.hpp"

#include <memory>
#include <ostream>
#include <stdexcept>

void runDemoScenario(std::ostream& out) {
    AppContainer container;
    auto& fleet = container.fleet();

    out << "=== VYSHKAT DEMO SCENARIOS ===\n";
    ReportPrinter::inventory(fleet, out); // Empty inventory.
    ReportPrinter::energy(fleet, out);
    ReportPrinter::beginners(fleet, out);

    out << "\n[1] Accept working vehicles after inspection.\n";
    auto scooter = std::make_unique<Scooter>("SC-001", "City scooter", 8, true, 1.40);
    out << "SC-001: " << (fleet.addTransport(std::move(scooter)) == AdmissionResult::Accepted
                              ? "ACCEPTED" : "REJECTED") << '\n';
    fleet.addTransport(std::make_unique<Bicycle>("BK-001", "Sport bicycle", 3, true));
    fleet.addTransport(std::make_unique<ElectricBicycle>("EB-001", "Electric bike", 6, true, 0.75));

    out << "\n[2] Broken vehicles are NOT added to the fleet.\n";
    const auto rejected = fleet.addTransport(
        std::make_unique<Scooter>("SC-BAD", "Broken scooter", 10, false, 7.0));
    out << "SC-BAD: " << (rejected == AdmissionResult::Rejected ? "REJECTED" : "ACCEPTED") << '\n';

    out << "\n[3] Register equipment (including energy consumers).\n";
    fleet.addEquipment(std::make_unique<Helmet>("HE-001", "Size M helmet"));
    fleet.addEquipment(std::make_unique<DockStation>("DO-001", "Four-slot dock station", 0.15));
    fleet.addEquipment(std::make_unique<ChargingCabinet>("CH-001", "Charging cabinet", 1.10));

    out << "\n[4] Duplicate inventory numbers are forbidden across all item types.\n";
    try {
        fleet.addEquipment(std::make_unique<Helmet>("SC-001", "Another helmet"));
    } catch (const std::invalid_argument& error) {
        out << "Expected error: " << error.what() << '\n';
    }

    out << "\n[5] Check input validation.\n";
    try {
        fleet.addTransport(std::make_unique<Bicycle>("BK-X", "Too difficult", 11, true));
    } catch (const std::invalid_argument& error) {
        out << "Expected beginner score error: " << error.what() << '\n';
    }
    try {
        fleet.addEquipment(std::make_unique<DockStation>("DO-X", "Invalid station", -2.0));
    } catch (const std::invalid_argument& error) {
        out << "Expected energy consumption error: " << error.what() << '\n';
    }

    out << "\n[6] Final reports: inventory IDs, total energy, beginner-friendly vehicles.\n";
    ReportPrinter::inventory(fleet, out);
    ReportPrinter::energy(fleet, out);
    ReportPrinter::beginners(fleet, out);
    out << "\nDEMO SUMMARY: 6 items in the fleet, 3.40 kWh/day, "
           "2 beginner-friendly vehicles, 1 rejected inspection.\n";
}

