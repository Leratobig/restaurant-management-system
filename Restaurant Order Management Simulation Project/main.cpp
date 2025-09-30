#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <map>
#include <fstream>
#include <ctime>

using namespace std;

// Menu items with prices
map<string, double> menu = {
    {"Burger", 35.99},
    {"Pizza",  55.99},
    {"Pasta",  28.99},
    {"Salad",  10.99},
    {"Steak",  120.99},
    {"Chicken Wings", 45.99},
    {"Soda",   15.49},
    {"Coffee", 18.49},
    {"Tea",    20.99},
    {"Dessert", 10.20}
};

// Order types
enum class OrderType { SIT_IN, TAKE_OUT };

// Order class to represent a customer order
class Order {
private:
    static int nextId;
    int id;
    OrderType type;
    vector<string> items;
    vector<int> quantities;
    double amount;
    int waiterId;
    string customerName;
    string timestamp;

public:
    Order(OrderType t, int wId, string custName) : type(t), waiterId(wId), customerName(custName) {
        id = nextId++;
        amount = 0.0;

        // Get current time
        time_t now = time(0);
        timestamp = ctime(&now);
        timestamp = timestamp.substr(0, timestamp.length()-1); // Remove newline
    }

    void addItem(string item, int quantity) {
        items.push_back(item);
        quantities.push_back(quantity);
        amount += menu[item] * quantity;
    }

    void displayOrder() const {
        cout << "\nOrder ID: " << id << " | Customer: " << customerName;
        cout << " | Type: " << (type == OrderType::SIT_IN ? "Sit-in" : "Take-out");
        cout << " | Total: R" << fixed << setprecision(2) << amount << endl;
        cout << "Time: " << timestamp << endl;
        cout << "Items: ";
        for (size_t i = 0; i < items.size(); i++) {
            cout << quantities[i] << "x " << items[i] << " (R" << menu.at(items[i]) * quantities[i] << ")";
            if (i < items.size() - 1) cout << ", ";
        }
        cout << endl;
    }

    double getAmount() const { return amount; }
    int getWaiterId() const { return waiterId; }
    OrderType getType() const { return type; }
    string getCustomerName() const { return customerName; }
    string getTimestamp() const { return timestamp; }
    vector<string> getItems() const { return items; }
    vector<int> getQuantities() const { return quantities; }
};

int Order::nextId = 1;

// Waiter class
class Waiter {
private:
    int id;
    string name;
    double totalSales;

public:
    Waiter(int i, string n) : id(i), name(n), totalSales(0) {}

    void addSale(double amount) { totalSales += amount; }

    int getId() const { return id; }
    string getName() const { return name; }
    double getTotalSales() const { return totalSales; }
    double getCommission() const { return totalSales * 0.15; }
};

// Function prototypes
void displayMenu();
int getValidatedInput(const string& prompt, int min, int max);
void generateDailyReport(const vector<Order>& orders, const vector<Waiter>& waiters);
void saveCustomerData(const vector<Order>& orders);

int main() {
    // Create waiters
    vector<Waiter> waiters = {
        Waiter(1, "Angel Dlamini"),
        Waiter(2, "Thabo Nkosi"),
        Waiter(3, "Peace Manana"),
        Waiter(4, "Aviwe Dlomo")
    };

    vector<Order> orders;

    cout << "==============================================" << endl;
    cout << "     RESTAURANT MANAGEMENT SYSTEM" << endl;
    cout << "==============================================" << endl;

    // Main loop
    while (true) {
        cout << "\nOptions:\n";
        cout << "1. Create New Order\n";
        cout << "2. View All Orders\n";
        cout << "3. Generate Daily Report\n";
        cout << "4. Save Customer Data to File\n";
        cout << "5. Exit\n";

        int choice = getValidatedInput("Enter your choice (1-5): ", 1, 5);

        if (choice == 1) {
            // Create new order
            cout << "\n--- CREATE NEW ORDER ---\n";

            // Customer name
            string customerName;
            cout << "Enter customer name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, customerName);

            // Order type
            cout << "Order type:\n";
            cout << "1. Sit-in\n";
            cout << "2. Take-out\n";
            int typeChoice = getValidatedInput("Select order type (1-2): ", 1, 2);
            OrderType orderType = (typeChoice == 1) ? OrderType::SIT_IN : OrderType::TAKE_OUT;

            // Select waiter (for ALL orders)
            cout << "\nAvailable Waiters:\n";
            for (const auto& waiter : waiters) {
                cout << waiter.getId() << ". " << waiter.getName() << endl;
            }
            int waiterId = getValidatedInput("Select waiter ID: ", 1, static_cast<int>(waiters.size()));

            // Create order
            Order newOrder(orderType, waiterId, customerName);

            // Add items
            cout << "\n--- ADD ITEMS TO ORDER ---\n";
            displayMenu();

            while (true) {
                int itemCount = 1;
                for (const auto& item : menu) {
                    cout << itemCount++ << ". " << item.first << " - R" << item.second << endl;
                }
                cout << itemCount << ". Finish order\n";

                int itemChoice = getValidatedInput("Select menu item: ", 1, static_cast<int>(menu.size()) + 1);

                if (itemChoice == static_cast<int>(menu.size()) + 1) break;

                auto it = menu.begin();
                advance(it, itemChoice - 1);
                string selectedItem = it->first;

                int quantity = getValidatedInput("Enter quantity: ", 1, 10);
                newOrder.addItem(selectedItem, quantity);
                cout << "Added " << quantity << "x " << selectedItem << " to order.\n";
            }

            // Save order
            orders.push_back(newOrder);

            // Update waiter sales (for all orders)
            for (auto& waiter : waiters) {
                if (waiter.getId() == waiterId) {
                    waiter.addSale(newOrder.getAmount());
                    break;
                }
            }

            cout << "\nOrder created successfully!\n";
            newOrder.displayOrder();

        } else if (choice == 2) {
            cout << "\n--- ALL ORDERS ---\n";
            if (orders.empty()) {
                cout << "No orders yet.\n";
            } else {
                for (const auto& order : orders) {
                    order.displayOrder();
                }
            }

        } else if (choice == 3) {
            generateDailyReport(orders, waiters);

        } else if (choice == 4) {
            saveCustomerData(orders);
            cout << "Customer data saved to file!\n";

        } else if (choice == 5) {
            cout << "Exiting system. Thank You!!\n";
            break;
        }
    }

    return 0;
}

// Generate comprehensive daily report
void generateDailyReport(const vector<Order>& orders, const vector<Waiter>& waiters) {
    cout << "\n--- DAILY REPORT ---\n";
    if (orders.empty()) {
        cout << "No orders to report.\n";
        return;
    }

    // Totals
    int sitInOrders = 0, takeOutOrders = 0;
    double sitInSales = 0.0, takeOutSales = 0.0;

    for (const auto& order : orders) {
        if (order.getType() == OrderType::SIT_IN) {
            sitInOrders++;
            sitInSales += order.getAmount();
        } else {
            takeOutOrders++;
            takeOutSales += order.getAmount();
        }
    }

    double totalSales = sitInSales + takeOutSales;

    // Report
    cout << fixed << setprecision(2);
    cout << "\n1. ORDER COUNTS:\n";
    cout << "   Sit-in:   " << sitInOrders << endl;
    cout << "   Take-out: " << takeOutOrders << endl;
    cout << "   Total:    " << sitInOrders + takeOutOrders << endl;

    cout << "\n2. SALES:\n";
    cout << "   Sit-in:   R" << sitInSales << endl;
    cout << "   Take-out: R" << takeOutSales << endl;
    cout << "   Total:    R" << totalSales << endl;

    cout << "\n3. WAITER PERFORMANCE:\n";
    for (const auto& waiter : waiters) {
        cout << "   " << waiter.getName() << " - Sales: R" << waiter.getTotalSales()
             << " | Commission (15%): R" << waiter.getCommission() << endl;
    }

    cout << "\n4. TOP SELLING ITEMS:\n";
    map<string, int> itemCounts;
    map<string, double> itemRevenue;

    for (const auto& order : orders) {
        vector<string> items = order.getItems();
        vector<int> quantities = order.getQuantities();

        for (size_t i = 0; i < items.size(); i++) {
            itemCounts[items[i]] += quantities[i];
            itemRevenue[items[i]] += menu[items[i]] * quantities[i];
        }
    }

    // Sort items by revenue
    vector<pair<string, double>> sortedItems;
    for (const auto& item : itemRevenue) {
        sortedItems.push_back({item.first, item.second});
    }



// Create a multimap with custom comparator for descending order
multimap<double, string, greater<double>> sortedByRevenue;
for (const auto& item : itemRevenue) {
    sortedByRevenue.insert({item.second, item.first});
}

for (const auto& entry : sortedByRevenue) {
    const string& itemName = entry.second;
    cout << "   " << itemName << ": " << itemCounts[itemName]
         << " sold, Revenue: R" << entry.first << endl;
}
}

// Save customer data to file
void saveCustomerData(const vector<Order>& orders) {
    ofstream outFile("customer_data.txt");

    if (!outFile) {
        cout << "Error creating file!\n";
        return;
    }

    outFile << "RESTAURANT CUSTOMER DATA REPORT\n";
    outFile << "Generated on: " << __DATE__ << " " << __TIME__ << "\n\n";

    for (const auto& order : orders) {
        outFile << "Order ID: " << order.getWaiterId() << "\n";
        outFile << "Customer: " << order.getCustomerName() << "\n";
        outFile << "Type: " << (order.getType() == OrderType::SIT_IN ? "Sit-in" : "Take-out") << "\n";
        outFile << "Time: " << order.getTimestamp() << "\n";
        outFile << "Total: R" << fixed << setprecision(2) << order.getAmount() << "\n";

        vector<string> items = order.getItems();
        vector<int> quantities = order.getQuantities();

        outFile << "Items: ";
        for (size_t i = 0; i < items.size(); i++) {
            outFile << quantities[i] << "x " << items[i];
            if (i < items.size() - 1) outFile << ", ";
        }
        outFile << "\n----------------------------------------\n";
    }

    outFile.close();
}

// Show menu
void displayMenu() {
    cout << "\n========== MENU ==========\n";
    int count = 1;
    for (const auto& item : menu) {
        cout << count++ << ". " << left << setw(15) << item.first
             << right << " R " << fixed << setprecision(2) << item.second << endl;
    }
    cout << "==========================\n";
}

// Input validation
int getValidatedInput(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            return value;
        }
        cout << "Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
