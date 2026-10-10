#include "app/ConsoleApp.hpp"

#include "app/DemoScenario.hpp"
#include "app/ReportPrinter.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>


struct EndOfInput : std::exception {};

template <typename T>
T parseNumber(const std::string& text) {
    std::istringstream input(text);
    input.imbue(std::locale::classic());
    T value{};
    input >> value;
    if (!input) {
        throw std::invalid_argument("Enter a valid number");
    }
    input >> std::ws;
    if (!input.eof()) {
        throw std::invalid_argument("Unexpected characters after the number");
    }
    return value;
}


ConsoleApp::ConsoleApp(FleetService& fleet) : fleet_(fleet) {}

std::string ConsoleApp::ask(std::istream& in, std::ostream& out, const std::string& question) {
    out << question << ": ";
    std::string answer;
    if (!std::getline(in, answer)) {
        throw EndOfInput{};
    }
    return answer;
}

int ConsoleApp::askInt(std::istream& in, std::ostream& out, const std::string& question) {
    return parseNumber<int>(ask(in, out, question));
}

double ConsoleApp::askDouble(std::istream& in, std::ostream& out, const std::string& question) {
    return parseNumber<double>(ask(in, out, question));
}

bool ConsoleApp::askOperational(std::istream& in, std::ostream& out) {
    const int flag = askInt(in, out, "Working? (1 = yes, 0 = no)");
    if (flag != 0 && flag != 1) {
        throw std::invalid_argument("Working status must be 0 or 1");
    }
    return flag == 1;
}

void ConsoleApp::receiveTransport(int menuChoice, std::istream& in, std::ostream& out) {
    const auto id = ask(in, out, "Inventory number");
    const auto name = ask(in, out, "Name");
    const auto score = askInt(in, out, "Beginner score (1-10)");
    const bool operational = askOperational(in, out);

    std::unique_ptr<Transport> item;
    if (menuChoice == 1) {
        const auto energy = askDouble(in, out, "Daily energy consumption (kWh)");
        item = std::make_unique<Scooter>(id, name, score, operational, energy);
    } else if (menuChoice == 2) {
        item = std::make_unique<Bicycle>(id, name, score, operational);
    } else {
        const auto energy = askDouble(in, out, "Daily energy consumption (kWh)");
        item = std::make_unique<ElectricBicycle>(id, name, score, operational, energy);
    }

    const auto decision = fleet_.addTransport(std::move(item));
    out << (decision == AdmissionResult::Accepted
                ? "Inspection PASSED. Vehicle added to inventory.\n"
                : "Inspection FAILED. Vehicle rejected.\n");
}

void ConsoleApp::receiveEquipment(int menuChoice, std::istream& in, std::ostream& out) {
    const auto id = ask(in, out, "Inventory number");
    const auto name = ask(in, out, "Name");
    std::unique_ptr<Equipment> item;
    if (menuChoice == 4) {
        item = std::make_unique<Helmet>(id, name);
    } else if (menuChoice == 5) {
        const auto energy = askDouble(in, out, "Daily energy consumption (kWh)");
        item = std::make_unique<DockStation>(id, name, energy);
    } else {
        const auto energy = askDouble(in, out, "Daily energy consumption (kWh)");
        item = std::make_unique<ChargingCabinet>(id, name, energy);
    }
    fleet_.addEquipment(std::move(item));
    out << "Equipment added to inventory.\n";
}

void ConsoleApp::run(std::istream& in, std::ostream& out) {
    out << "Welcome to VyshKat!\n";
    while (true) {
        out << "\n1. Add scooter\n2. Add bicycle\n"
               "3. Add electric bicycle\n4. Add helmet\n"
               "5. Add dock station\n6. Add charging cabinet\n"
               "7. Show all inventory\n8. Daily energy consumption\n"
               "9. Beginner-friendly transport\n10. Run demo scenarios\n"
               "0. Exit\n";
        try {
            const int choice = askInt(in, out, "Menu option");
            if (choice == 0) {
                out << "Goodbye!\n";
                return;
            }
            if (choice >= 1 && choice <= 3) {
                receiveTransport(choice, in, out);
            } else if (choice >= 4 && choice <= 6) {
                receiveEquipment(choice, in, out);
            } else if (choice == 7) {
                ReportPrinter::inventory(fleet_, out);
            } else if (choice == 8) {
                ReportPrinter::energy(fleet_, out);
            } else if (choice == 9) {
                ReportPrinter::beginners(fleet_, out);
            } else if (choice == 10) {
                // Runs in an isolated container, leaves the user's inventory untouched.
                runDemoScenario(out);
            } else {
                out << "Unknown menu option.\n";
            }
        } catch (const EndOfInput&) {
            out << "\nInput ended. Goodbye!\n";
            return;
        } catch (const std::exception& error) {
            out << "Error: " << error.what() << '\n';
        }
    }
}

