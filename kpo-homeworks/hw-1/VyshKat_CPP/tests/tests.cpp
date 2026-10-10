#include "app/AppContainer.hpp"
#include "app/ConsoleApp.hpp"
#include "app/DemoScenario.hpp"
#include "app/ReportPrinter.hpp"
#include "domain/Equipment.hpp"
#include "domain/Transport.hpp"
#include "services/FleetService.hpp"
#include "services/ServiceCenterInspector.hpp"
#include "storage/InMemoryInventoryRepository.hpp"

#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>



void fail(const std::string& why, int line) {
    throw std::runtime_error("line " + std::to_string(line) + ": " + why);
}
#define CHECK(expr) do { if (!(expr)) fail("Check failed: " #expr, __LINE__); } while (false)
#define CHECK_EQ(a, b) do { if (!((a) == (b))) fail("Not equal: " #a " and " #b, __LINE__); } while (false)
#define CHECK_NEAR(a, b) do { if (std::fabs((a) - (b)) > 1e-9) fail("Not close: " #a " and " #b, __LINE__); } while (false)

template<class Exception, class Function>
void expectException(Function&& function) {
    try {
        function();
    } catch (const Exception&) {
        return;
    }
    throw std::runtime_error("Expected an exception, but none was thrown");
}

struct FakeInspector final : ITechnicalInspector {
    explicit FakeInspector(bool allow) : allowed(allow) {}
    bool approve(const Transport& transport) const override {
        ++calls;
        lastId = transport.inventoryNumber();
        return allowed;
    }
    bool allowed;
    mutable int calls = 0;
    mutable std::string lastId;
};

// A wholly new transport kind compiled only into the tests: the fleet doesn't need modification.
class SolarMoped final : public Transport, public IEnergyConsumer {
public:
    SolarMoped() : Transport("SOL-1", "Solar-powered moped", 9, true) {}
    std::string type() const override { return "Solar moped"; }
    double dailyEnergyKWh() const noexcept override { return 2.25; }
};

// Example of future inventory expansion which also requires no fleet changes.
class SmartLock final : public Equipment, public IEnergyConsumer {
public:
    SmartLock() : Equipment("LOCK-1", "Smart lock") {}
    std::string type() const override { return "Smart lock"; }
    double dailyEnergyKWh() const noexcept override { return 0.2; }
};

struct Runner {
    int passed = 0;
    int failed = 0;
    void test(const std::string& name, const std::function<void()>& body) {
        try {
            body();
            ++passed;
            std::cout << "[OK]   " << name << '\n';
        } catch (const std::exception& e) {
            ++failed;
            std::cerr << "[FAIL] " << name << " -> " << e.what() << '\n';
        }
    }
};


int main() {
    Runner r;

    r.test("Inventory number and name are stored", [] {
        Helmet item("H-1", "Helmet");
        CHECK_EQ(item.inventoryNumber(), "H-1");
        CHECK_EQ(item.name(), "Helmet");
    });
    r.test("Empty ID and name are forbidden", [] {
        expectException<std::invalid_argument>([] { Helmet h("", "Helmet"); });
        expectException<std::invalid_argument>([] { Helmet h("  \t", "Helmet"); });
        expectException<std::invalid_argument>([] { Helmet h("H1", "  "); });
    });
    r.test("Beginner score boundaries: 1, 5, 6, 10", [] {
        Bicycle a("a", "a", 1, true), b("b", "b", 5, true),
                c("c", "c", 6, true), d("d", "d", 10, true);
        CHECK(!a.suitableForBeginners());
        CHECK(!b.suitableForBeginners());
        CHECK(c.suitableForBeginners());
        CHECK(d.suitableForBeginners());
        CHECK_EQ(c.beginnerScore(), 6);
    });
    r.test("Scores below 1 or above 10 are forbidden", [] {
        expectException<std::invalid_argument>([] { Bicycle b("1", "a", 0, true); });
        expectException<std::invalid_argument>([] { Bicycle b("1", "a", 11, true); });
    });
    r.test("Energy use accepts zero and positive values", [] {
        Scooter s("s", "s", 6, true, 0);
        DockStation d("d", "d", 0.125);
        CHECK_NEAR(s.dailyEnergyKWh(), 0);
        CHECK_NEAR(d.dailyEnergyKWh(), 0.125);
    });
    r.test("Negative, infinite and NaN energy values are forbidden", [] {
        expectException<std::invalid_argument>([] { Scooter s("s", "s", 6, true, -0.1); });
        expectException<std::invalid_argument>([] {
            ChargingCabinet c("c", "c", std::numeric_limits<double>::infinity());
        });
        expectException<std::invalid_argument>([] {
            DockStation d("d", "d", std::numeric_limits<double>::quiet_NaN());
        });
    });
    r.test("Transport types and categories", [] {
        Scooter scooter("s", "s", 6, true, 1);
        Bicycle bike("b", "b", 7, false);
        ElectricBicycle electric("e", "e", 8, true, 2);
        CHECK_EQ(scooter.type(), "Scooter");
        CHECK_EQ(bike.type(), "Bicycle");
        CHECK_EQ(electric.type(), "Electric bicycle");
        CHECK_EQ(scooter.category(), ItemCategory::Transport);
        CHECK(!bike.operational());
        CHECK_NEAR(electric.dailyEnergyKWh(), 2);
    });
    r.test("Equipment types and categories", [] {
        Helmet h("h", "h");
        DockStation d("d", "d", 0.5);
        ChargingCabinet c("c", "c", 1.5);
        CHECK_EQ(h.type(), "Helmet");
        CHECK_EQ(d.type(), "Dock station");
        CHECK_EQ(c.type(), "Charging cabinet");
        CHECK_EQ(h.category(), ItemCategory::Equipment);
        CHECK_NEAR(c.dailyEnergyKWh(), 1.5);
    });
    r.test("Empty storage: list and contains", [] {
        InMemoryInventoryRepository repo;
        CHECK(repo.list().empty());
        CHECK(!repo.contains("X"));
    });
    r.test("Repository owns items and preserves order", [] {
        InMemoryInventoryRepository repo;
        repo.save(std::make_unique<Helmet>("H-1", "Helmet"));
        repo.save(std::make_unique<Bicycle>("B-1", "Bicycle", 7, true));
        CHECK(repo.contains("H-1"));
        CHECK(!repo.contains("not here"));
        CHECK_EQ(repo.list().size(), 2u);
        CHECK_EQ(repo.list().at(0)->inventoryNumber(), "H-1");
        CHECK_EQ(repo.list().at(1)->inventoryNumber(), "B-1");
    });
    r.test("Repository rejects null pointers", [] {
        InMemoryInventoryRepository repo;
        expectException<std::invalid_argument>([&] { repo.save(nullptr); });
    });
    r.test("Repository rejects duplicate IDs across types", [] {
        InMemoryInventoryRepository repo;
        repo.save(std::make_unique<Helmet>("I-1", "Helmet"));
        expectException<std::invalid_argument>([&] {
            repo.save(std::make_unique<Bicycle>("I-1", "Bicycle", 6, true));
        });
        CHECK_EQ(repo.list().size(), 1u);
    });
    r.test("Real inspector accepts working vehicles", [] {
        ServiceCenterInspector inspector;
        Bicycle a("a", "a", 6, true), b("b", "b", 6, false);
        CHECK(inspector.approve(a));
        CHECK(!inspector.approve(b));
    });
    r.test("Service accepts working vehicles", [] {
        AppContainer app;
        CHECK_EQ(app.fleet().addTransport(std::make_unique<Bicycle>("b", "b", 6, true)),
                 AdmissionResult::Accepted);
        CHECK_EQ(app.fleet().inventory().size(), 1u);
    });
    r.test("Service does not register rejected vehicles", [] {
        AppContainer app;
        CHECK_EQ(app.fleet().addTransport(std::make_unique<Scooter>("s", "s", 9, false, 4)),
                 AdmissionResult::Rejected);
        CHECK(app.fleet().inventory().empty());
        CHECK_NEAR(app.fleet().totalDailyEnergyKWh(), 0);
    });
    r.test("DI allows inspector to reject working vehicles", [] {
        InMemoryInventoryRepository repo;
        FakeInspector fake(false);
        FleetService service(repo, fake);
        CHECK_EQ(service.addTransport(std::make_unique<Bicycle>("B-1", "b", 8, true)),
                 AdmissionResult::Rejected);
        CHECK_EQ(fake.calls, 1);
        CHECK_EQ(fake.lastId, "B-1");
        CHECK(repo.list().empty());
    });
    r.test("DI lets inspector decide even for broken vehicles", [] {
        InMemoryInventoryRepository repo;
        FakeInspector fake(true);
        FleetService service(repo, fake);
        CHECK_EQ(service.addTransport(std::make_unique<Bicycle>("B-2", "b", 8, false)),
                 AdmissionResult::Accepted);
        CHECK_EQ(fake.calls, 1);
        CHECK_EQ(repo.list().size(), 1u);
    });
    r.test("Inspection happens before storage", [] {
        struct ObservingInspector final : ITechnicalInspector {
            explicit ObservingInspector(const IInventoryRepository& r) : repo(r) {}
            bool approve(const Transport& transport) const override {
                CHECK(!repo.contains(transport.inventoryNumber()));
                return true;
            }
            const IInventoryRepository& repo;
        };
        InMemoryInventoryRepository repo;
        ObservingInspector inspector(repo);
        FleetService fleet(repo, inspector);
        fleet.addTransport(std::make_unique<Bicycle>("id", "bike", 6, true));
        CHECK(repo.contains("id"));
    });
    r.test("Service checks duplicates before inspection", [] {
        InMemoryInventoryRepository repo;
        FakeInspector inspector(true);
        FleetService fleet(repo, inspector);
        fleet.addTransport(std::make_unique<Bicycle>("id", "bike", 6, true));
        expectException<std::invalid_argument>([&] {
            fleet.addTransport(std::make_unique<Scooter>("id", "scooter", 8, true, 2));
        });
        CHECK_EQ(inspector.calls, 1);
        CHECK_EQ(repo.list().size(), 1u);
    });
    r.test("Service rejects null transport and equipment", [] {
        AppContainer app;
        expectException<std::invalid_argument>([&] { app.fleet().addTransport(nullptr); });
        expectException<std::invalid_argument>([&] { app.fleet().addEquipment(nullptr); });
    });
    r.test("Service registers a helmet", [] {
        AppContainer app;
        app.fleet().addEquipment(std::make_unique<Helmet>("H-1", "Helmet"));
        CHECK_EQ(app.fleet().inventory().size(), 1u);
        CHECK_EQ(app.fleet().inventory()[0]->inventoryNumber(), "H-1");
    });
    r.test("Duplicate ID between vehicle and equipment is forbidden", [] {
        AppContainer app;
        app.fleet().addEquipment(std::make_unique<Helmet>("same", "helmet"));
        expectException<std::invalid_argument>([&] {
            app.fleet().addTransport(std::make_unique<Bicycle>("same", "bike", 6, true));
        });
        CHECK_EQ(app.fleet().inventory().size(), 1u);
    });
    r.test("Energy report sums consumers and ignores other items", [] {
        AppContainer app;
        app.fleet().addTransport(std::make_unique<Scooter>("s", "s", 5, true, 1.25));
        app.fleet().addTransport(std::make_unique<ElectricBicycle>("e", "e", 7, true, 0.75));
        app.fleet().addTransport(std::make_unique<Bicycle>("b", "b", 7, true));
        app.fleet().addEquipment(std::make_unique<Helmet>("h", "h"));
        app.fleet().addEquipment(std::make_unique<ChargingCabinet>("c", "c", 2.00));
        app.fleet().addEquipment(std::make_unique<DockStation>("d", "d", 0.50));
        CHECK_NEAR(app.fleet().totalDailyEnergyKWh(), 4.50);
    });
    r.test("Beginner report includes only accepted vehicles with score >= 6", [] {
        AppContainer app;
        app.fleet().addTransport(std::make_unique<Scooter>("s1", "s", 5, true, 1));
        app.fleet().addTransport(std::make_unique<Scooter>("s2", "s", 6, true, 1));
        app.fleet().addTransport(std::make_unique<Bicycle>("b", "b", 10, false));
        app.fleet().addEquipment(std::make_unique<Helmet>("h", "h"));
        const auto suitable = app.fleet().beginnerFriendlyTransport();
        CHECK_EQ(suitable.size(), 1u);
        CHECK_EQ(suitable.at(0)->inventoryNumber(), "s2");
    });
    r.test("New vehicle type works without FleetService changes", [] {
        AppContainer app;
        app.fleet().addTransport(std::make_unique<SolarMoped>());
        CHECK_NEAR(app.fleet().totalDailyEnergyKWh(), 2.25);
        CHECK_EQ(app.fleet().beginnerFriendlyTransport().size(), 1u);
    });
    r.test("New energy-consuming equipment is counted automatically", [] {
        AppContainer app;
        app.fleet().addEquipment(std::make_unique<SmartLock>());
        CHECK_NEAR(app.fleet().totalDailyEnergyKWh(), 0.20);
    });
    r.test("Reports for an empty fleet", [] {
        AppContainer app;
        std::ostringstream out;
        ReportPrinter::inventory(app.fleet(), out);
        ReportPrinter::beginners(app.fleet(), out);
        ReportPrinter::energy(app.fleet(), out);
        CHECK(out.str().find("The fleet is empty") != std::string::npos);
        CHECK(out.str().find("No suitable transport available") != std::string::npos);
        CHECK(out.str().find("0.00") != std::string::npos);
    });
    r.test("Inventory report contains every inventory number", [] {
        AppContainer app;
        app.fleet().addTransport(std::make_unique<Bicycle>("BI-12", "Mountain", 10, true));
        app.fleet().addEquipment(std::make_unique<Helmet>("HE-44", "Protective"));
        std::ostringstream out;
        ReportPrinter::inventory(app.fleet(), out);
        ReportPrinter::beginners(app.fleet(), out);
        CHECK(out.str().find("BI-12") != std::string::npos);
        CHECK(out.str().find("HE-44") != std::string::npos);
        CHECK(out.str().find("beginner score: 10/10") != std::string::npos);
        CHECK(out.str().find("10/10)") != std::string::npos);
    });
    r.test("Energy formatting restores stream settings", [] {
        AppContainer app;
        std::ostringstream out;
        out << std::scientific << std::setprecision(4);
        const auto flags = out.flags();
        const auto precision = out.precision();
        ReportPrinter::energy(app.fleet(), out);
        CHECK_EQ(out.flags(), flags);
        CHECK_EQ(out.precision(), precision);
    });
    r.test("Demo contains success and error scenarios", [] {
        std::ostringstream out;
        runDemoScenario(out);
        const auto str = out.str();
        CHECK(str.find("6 items in the fleet") != std::string::npos);
        CHECK(str.find("3.40 kWh") != std::string::npos);
        CHECK(str.find("SC-BAD: REJECTED") != std::string::npos);
        CHECK(str.find("Duplicate inventory number") != std::string::npos);
        CHECK(str.find("Beginner score") != std::string::npos);
        CHECK(str.find("Energy consumption must") != std::string::npos);
        CHECK(str.find("[HE-001]") != std::string::npos);
    });
    r.test("CLI: register all six item types and show three reports", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in(
            "1\nS1\nScooter\n6\n1\n1.25\n"
            "2\nB1\nBicycle\n5\n1\n"
            "3\nE1\nElectric bicycle\n9\n1\n0.75\n"
            "4\nH1\nHelmet\n"
            "5\nD1\nDock\n0.25\n"
            "6\nC1\nCabinet\n1.00\n"
            "7\n8\n9\n0\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK_EQ(app.fleet().inventory().size(), 6u);
        CHECK_NEAR(app.fleet().totalDailyEnergyKWh(), 3.25);
        CHECK_EQ(app.fleet().beginnerFriendlyTransport().size(), 2u);
        CHECK(out.str().find("Goodbye") != std::string::npos);
        CHECK(out.str().find("Total daily energy consumption: 3.25") != std::string::npos);
    });
    r.test("CLI: inspection rejection and negative energy", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in("2\nB1\nBroken\n10\n0\n"
                              "5\nD1\nDock\n-2\n"
                              "0\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK(app.fleet().inventory().empty());
        CHECK(out.str().find("FAILED") != std::string::npos);
        CHECK(out.str().find("Error: Energy consumption must") != std::string::npos);
    });
    r.test("CLI: invalid menu option and malformed numbers", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in("abc\n12abc\n99\n0\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK(out.str().find("valid number") != std::string::npos);
        CHECK(out.str().find("Unexpected characters") != std::string::npos);
        CHECK(out.str().find("Unknown menu option") != std::string::npos);
    });
    r.test("CLI: invalid working flag 2", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in("2\nB2\nBicycle\n6\n2\n0\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK(out.str().find("Working status must be 0 or 1") != std::string::npos);
    });
    r.test("CLI: invalid beginner score and duplicate IDs", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in("2\nB1\nBicycle\n11\n1\n"
                              "4\nH1\nHelmet\n"
                              "4\nH1\nHelmet 2\n0\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK(out.str().find("between 1 and 10") != std::string::npos);
        CHECK(out.str().find("Duplicate") != std::string::npos);
        CHECK_EQ(app.fleet().inventory().size(), 1u);
    });
    r.test("CLI: early EOF during input exits cleanly", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in("1\nS1\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK(out.str().find("Input ended") != std::string::npos);
    });
    r.test("CLI: option 10 runs an isolated demo", [] {
        AppContainer app;
        ConsoleApp console(app.fleet());
        std::istringstream in("10\n0\n");
        std::ostringstream out;
        console.run(in, out);
        CHECK(out.str().find("DEMO SUMMARY") != std::string::npos);
        CHECK(app.fleet().inventory().empty());
    });

    std::cout << "\nRESULT: " << r.passed << " passed, " << r.failed << " failed\n";
    return r.failed == 0 ? 0 : 1;
}
