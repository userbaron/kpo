#ifndef IINVENTORYREPOSITORY_HPP
#define IINVENTORYREPOSITORY_HPP

#include "domain/InventoryItem.hpp"

#include <memory>
#include <string>
#include <vector>

class IInventoryRepository {
public:
    virtual ~IInventoryRepository() = default;
    [[nodiscard]] virtual bool contains(const std::string& inventoryNumber) const = 0;
    virtual void save(std::unique_ptr<IInventoryItem> item) = 0;
    [[nodiscard]] virtual std::vector<const IInventoryItem*> list() const = 0;
};

#endif
