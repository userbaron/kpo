#ifndef CONSOLEAPP_HPP
#define CONSOLEAPP_HPP

#include "services/FleetService.hpp"

#include <iosfwd>
#include <string>

class ConsoleApp final {
public:
    explicit ConsoleApp(FleetService& fleet);
    void run(std::istream& in, std::ostream& out);

private:
    static std::string ask(std::istream& in, std::ostream& out, const std::string& question);
    static int askInt(std::istream& in, std::ostream& out, const std::string& question);
    static double askDouble(std::istream& in, std::ostream& out, const std::string& question);
    static bool askOperational(std::istream& in, std::ostream& out);
    void receiveTransport(int menuChoice, std::istream& in, std::ostream& out);
    void receiveEquipment(int menuChoice, std::istream& in, std::ostream& out);

    FleetService& fleet_;
};

#endif
