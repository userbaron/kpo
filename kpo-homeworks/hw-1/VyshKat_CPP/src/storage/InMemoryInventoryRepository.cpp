#include "storage/InMemoryInventoryRepository.hpp"

#include <stdexcept>
#include <utility>

bool InMemoryInventoryRepository::contains(const std::string& inventoryNumber) const {
    for (const auto& item : objects_) {
        if (item->inventoryNumber() == inventoryNumber) {
            return true;
        }
    }
    return false;
}

void InMemoryInventoryRepository::save(std::unique_ptr<IInventoryItem> item) {
    if (!item) {
        throw std::invalid_argument("Cannot save a null item");
    }
    if (contains(item->inventoryNumber())) {
        throw std::invalid_argument("Duplicate inventory number: " + item->inventoryNumber());
    }
    objects_.push_back(std::move(item));
}

std::vector<const IInventoryItem*> InMemoryInventoryRepository::list() const {
    std::vector<const IInventoryItem*> result;
    result.reserve(objects_.size());
    for (const auto& item : objects_) {
        result.push_back(item.get());
    }
    return result;
}

