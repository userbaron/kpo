#ifndef INMEMORYINVENTORYREPOSITORY_HPP
#define INMEMORYINVENTORYREPOSITORY_HPP

#include "storage/IInventoryRepository.hpp"

#include <memory>
#include <vector>

class InMemoryInventoryRepository final : public IInventoryRepository {
public:
    [[nodiscard]] bool contains(const std::string& inventoryNumber) const override;
    void save(std::unique_ptr<IInventoryItem> item) override;
    [[nodiscard]] std::vector<const IInventoryItem*> list() const override;

private:
    std::vector<std::unique_ptr<IInventoryItem>> objects_;
};

#endif
