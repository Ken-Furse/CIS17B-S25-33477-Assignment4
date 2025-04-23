#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>

// Custom Exception: DuplicateItemException
class DuplicateItemException : public std::runtime_error {
public:
    DuplicateItemException(const std::string& msg) : std::runtime_error(msg) {}
};

// Custom Exception: ItemNotFoundException
class ItemNotFoundException : public std::runtime_error {
public:
    ItemNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
};

// StoredItem class
class StoredItem {
private:
    std::string id;
    std::string description;
    std::string location;

public:
    StoredItem(std::string id, std::string desc, std::string loc)
        : id(std::move(id)), description(std::move(desc)), location(std::move(loc)) {
    }

    std::string getId() const { return id; }
    std::string getDescription() const { return description; }
    std::string getLocation() const { return location; }
};

// StorageManager class
class StorageManager {
private:
    std::unordered_map<std::string, std::shared_ptr<StoredItem>> itemById;
    std::map<std::string, std::shared_ptr<StoredItem>> itemByDescription;

public:
    void addItem(const std::shared_ptr<StoredItem>& item) {
        if (itemById.count(item->getId()) > 0) {
            throw DuplicateItemException("Item with ID " + item->getId() + " already exists!");
        }
        itemById[item->getId()] = item;
        itemByDescription[item->getDescription()] = item;
        std::cout << "Adding item: " << item->getId() << " - " << item->getDescription() << std::endl;
    }

    std::shared_ptr<StoredItem> findById(const std::string& id) const {
        std::unordered_map<std::string, std::shared_ptr<StoredItem>>::const_iterator it = itemById.find(id);
        if (it == itemById.end()) {
            throw ItemNotFoundException("Item with ID " + id + " not found!");
        }
        return it->second;
    }

    void removeItem(const std::string& id) {
        std::unordered_map<std::string, std::shared_ptr<StoredItem>>::iterator it = itemById.find(id);
        if (it == itemById.end()) {
            throw ItemNotFoundException("Item with ID " + id + " not found!");
        }
        std::string desc = it->second->getDescription();
        itemById.erase(it);
        itemByDescription.erase(desc);
        std::cout << "Removed item with ID " << id << std::endl;
    }

    void listItemsByDescription() const {
        std::cout << "Items in Description Order:" << std::endl;
        std::map<std::string, std::shared_ptr<StoredItem>>::const_iterator it;
        for (it = itemByDescription.begin(); it != itemByDescription.end(); ++it) {
            const std::shared_ptr<StoredItem>& item = it->second;
            std::cout << "- " << item->getDescription() << ": " << item->getLocation() << std::endl;
        }
    }
};

// Test: Adding a duplicate item
void testDuplicateAddition(StorageManager& manager) {
    try {
        std::shared_ptr<StoredItem> duplicateItem = std::make_shared<StoredItem>("ITEM001", "LED Light", "Aisle 3, Shelf 1");
        std::cout << "Attempting to add ITEM001 again..." << std::endl;
        manager.addItem(duplicateItem);
    }
    catch (const DuplicateItemException& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}

// Test: Finding a non-existent item
void testItemNotFound(StorageManager& manager) {
    try {
        std::cout << "Removing ITEM003..." << std::endl;
        manager.removeItem("ITEM003");
    }
    catch (const ItemNotFoundException& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}

// Main function
int main() {
    StorageManager manager;

    std::shared_ptr<StoredItem> item1 = std::make_shared<StoredItem>("ITEM001", "LED Light", "Aisle 3, Shelf 1");
    std::shared_ptr<StoredItem> item2 = std::make_shared<StoredItem>("ITEM002", "Fan Motor", "Aisle 2, Shelf 5");

    // Add items
    manager.addItem(item1);
    manager.addItem(item2);

    // Test adding duplicate
    testDuplicateAddition(manager);

    // Find and print an item
    try {
        std::cout << "Retrieving ITEM002..." << std::endl;
        std::shared_ptr<StoredItem> foundItem = manager.findById("ITEM002");
        std::cout << "Found: " << foundItem->getDescription()
            << " at " << foundItem->getLocation() << std::endl;
    }
    catch (const ItemNotFoundException& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }

    // Test removing non-existent item
    testItemNotFound(manager);

    // List items by description
    manager.listItemsByDescription();

    return 0;
}
