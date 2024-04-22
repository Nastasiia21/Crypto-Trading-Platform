#include "Wallet.h"
#include <iostream>
#include "CSVReader.h"

// Default constructor for the Wallet class
Wallet::Wallet()
{
    // Constructor body is empty as there's no initial setup required
}

// Inserts or adds to the currency amount in the wallet
void Wallet::insertCurrency(std::string type, double amount)
{
    double balance;
    if (amount < 0)
    {
        throw std::exception{}; // Throw an exception if a negative amount is attempted to be added
    }
    if (currencies.count(type) == 0) // Check if the currency type already exists in the wallet
    {
        balance = 0; // Initialize balance to zero if currency type does not exist
    }
    else
    {
        balance = currencies[type]; // Get the current balance if it exists
    }
    balance += amount; // Add the specified amount to the balance
    currencies[type] = balance; // Update the currency balance in the wallet
}

// Removes a specified amount of currency from the wallet
bool Wallet::removeCurrency(std::string type, double amount)
{
    if (amount < 0)
    {
        return false; // Return false if trying to remove a negative amount (invalid operation)
    }
    if (currencies.count(type) == 0) // Check if the currency exists in the wallet
    {
        return false; // Return false if the currency does not exist
    }
    else if (containsCurrency(type, amount)) // Check if the wallet contains enough of the currency
    {
        currencies[type] -= amount; // Deduct the amount from the wallet
        return true; // Return true indicating successful removal
    }
    return false; // Return false if there isn't enough currency to remove
}

// Checks if the wallet contains at least a certain amount of a currency
bool Wallet::containsCurrency(std::string type, double amount)
{
    if (currencies.count(type) == 0) // Check if the currency exists
        return false;
    return currencies[type] >= amount; // Check if the balance is greater than or equal to the amount needed
}

// Returns a string representation of the wallet showing all currencies and their amounts
std::string Wallet::toString()
{
    std::string s;
    for (auto& pair : currencies)
    {
        s += pair.first + " : " + std::to_string(pair.second) + "\n"; // Format each currency type and amount into a string
    }
    return s;
}

// Determines if a particular order can be fulfilled based on the currency amounts in the wallet
bool Wallet::canFulfillOrder(OrderBookEntry order)
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    if (order.orderType == OrderBookType::ask) // If the order is an ask
    {
        double amount = order.amount;
        std::string currency = currs[0];
        return containsCurrency(currency, amount); // Check if there is enough currency to fulfill the ask
    }
    else if (order.orderType == OrderBookType::bid) // If the order is a bid
    {
        double amount = order.amount * order.price;
        std::string currency = currs[1];
        return containsCurrency(currency, amount); // Check if there is enough currency to cover the bid price
    }
    return false;
}

// Processes a completed sale, adjusting the wallet's balances accordingly
void Wallet::processSale(OrderBookEntry& sale)
{
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    if (sale.orderType == OrderBookType::asksale) // If the sale resulted from an ask
    {
        double outgoingAmount = sale.amount;
        std::string outgoingCurrency = currs[0];
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount; // Increase incoming currency
        currencies[outgoingCurrency] -= outgoingAmount; // Decrease outgoing currency
    }
    else if (sale.orderType == OrderBookType::bidsale) // If the sale resulted from a bid
    {
        double incomingAmount = sale.amount;
        std::string incomingCurrency = currs[0];
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount; // Increase incoming currency
        currencies[outgoingCurrency] -= outgoingAmount; // Decrease outgoing currency
    }
}

// Overload of the output stream operator to print the wallet contents using Wallet::toString()
std::ostream& operator<<(std::ostream& os, Wallet& wallet)
{
    os << wallet.toString();
    return os;
}
