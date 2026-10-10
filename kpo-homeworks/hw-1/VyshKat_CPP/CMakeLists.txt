cmake_minimum_required(VERSION 3.16)
project(VyshKat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core
    src/domain/InventoryItem.cpp
    src/domain/EnergyConsumer.cpp
    src/domain/Transport.cpp
    src/domain/Equipment.cpp
    src/storage/InMemoryInventoryRepository.cpp
    src/services/ServiceCenterInspector.cpp
    src/services/FleetService.cpp
    src/app/ReportPrinter.cpp
    src/app/DemoScenario.cpp
    src/app/ConsoleApp.cpp
)
target_include_directories(core PUBLIC include)

add_executable(vyshkat src/main.cpp)
target_link_libraries(vyshkat core)

add_executable(tests tests/tests.cpp)
target_link_libraries(tests core)

enable_testing()
add_test(NAME unit_tests COMMAND tests)
add_test(NAME demo COMMAND vyshkat --demo)
