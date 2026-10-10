#include "app/AppContainer.hpp"
#include "app/ConsoleApp.hpp"
#include "app/DemoScenario.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--demo") {
        runDemoScenario(std::cout);
        return 0;
    }
    if (argc != 1) {
        std::cerr << "Usage: vyshkat [--demo]\n";
        return 1;
    }
    AppContainer container;
    ConsoleApp app(container.fleet());
    app.run(std::cin, std::cout);
    return 0;
}
