#ifndef INVENTORYITEM_HPP
#define INVENTORYITEM_HPP

#include <string>

enum class ItemCategory { Transport, Equipment };

class IInventoryItem {
public:
    virtual ~IInventoryItem() = default;
    [[nodiscard]] virtual const std::string& inventoryNumber() const noexcept = 0;
    [[nodiscard]] virtual const std::string& name() const noexcept = 0;
    [[nodiscard]] virtual std::string type() const = 0;
    [[nodiscard]] virtual ItemCategory category() const noexcept = 0;
};

class InventoryItem : public IInventoryItem {
public:
    InventoryItem(std::string number, std::string name);
    [[nodiscard]] const std::string& inventoryNumber() const noexcept override;
    [[nodiscard]] const std::string& name() const noexcept override;

private:
    std::string number_;
    std::string name_;
};

#endif
