#include "domain/InventoryItem.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

bool blank(const std::string& value) {
    return std::all_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
}

InventoryItem::InventoryItem(std::string number, std::string name)
    : number_(std::move(number)), name_(std::move(name)) {
    if (blank(number_)) {
        throw std::invalid_argument("Inventory number cannot be empty");
    }
    if (blank(name_)) {
        throw std::invalid_argument("Name cannot be empty");
    }
}

const std::string& InventoryItem::inventoryNumber() const noexcept { return number_; }
const std::string& InventoryItem::name() const noexcept { return name_; }

