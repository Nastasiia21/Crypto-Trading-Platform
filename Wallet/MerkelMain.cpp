#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"

// Constructor for the MerkelMain class
MerkelMain::MerkelMain()
{
    // Constructor body is empty as no initialization is done here
}

// Initial setup function for the MerkelMain class
void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();  // Set the current time to the earliest time available in the order book

    wallet.insertCurrency("BTC", 10);  // Insert initial currency into the wallet

    while(true)  // Enter an infinite loop to continuously interact with the user
    {
        printMenu();  // Display the main menu
        input = getUserOption();  // Get user input
        processUserOption(input);  // Process the user input
    }
}

// Prints the main menu to the console
void MerkelMain::printMenu()
{
    std::cout << "1: Print help " << std::endl;  // Option to print help
    std::cout << "2: Print exchange stats" << std::endl;  // Option to print exchange stats
    std::cout << "3: Make an offer " << std::endl;  // Option to make an offer
    std::cout << "4: Make a bid " << std::endl;  // Option to make a bid
    std::cout << "5: Print wallet " << std::endl;  // Option to print wallet contents
    std::cout << "6: Continue " << std::endl;  // Option to move to the next timeframe
    std::cout << "============== " << std::endl;
    std::cout << "Current time is: " << currentTime << std::endl;  // Displays the current time
}

// Prints help information
void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

// Prints market statistics based on the current state of the order book
void MerkelMain::printMarketStats()
{
    for (const std::string& p : orderBook.getKnownProducts())  // Loop through each product known to the order book
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);  // Get ask orders for the current time and product
        std::cout << "Product: " << p << std::endl;  // Print the product name
        std::cout << "Asks seen: " << entries.size() << std::endl;  // Print the number of ask entries
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;  // Print the highest ask price
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;  // Print the lowest ask price
    }
}

// Handles user input to create an ask order
void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);  // Get the full input line from user

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');  // Tokenise the input string
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;  // Print error if input format is incorrect
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(tokens[1], tokens[2], currentTime, tokens[0], OrderBookType::ask);  // Create an OrderBookEntry from tokens
            obe.username = "simuser";  // Set the username for the order
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);  // Insert the order into the order book
            }
            else {
                std::cout << "Wallet has insufficient funds. " << std::endl;  // Warn if wallet has insufficient funds
            }
        } catch (const std::exception& e) {
            std::cout << "MerkelMain::enterAsk Bad input" << std::endl;  // Handle any exceptions from creating an order
        }
    }
}

// Handles user input to create a bid order
void MerkelMain::enterBid()
{
    std::cout << "Make a bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);  // Get the full input line from user

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');  // Tokenise the input string
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;  // Print error if input format is incorrect
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(tokens[1], tokens[2], currentTime, tokens[0], OrderBookType::bid);  // Create an OrderBookEntry from tokens
            obe.username = "simuser";  // Set the username for the order
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);  // Insert the order into the order book
            }
            else {
                std::cout << "Wallet has insufficient funds. " << std::endl;  // Warn if wallet has insufficient funds
            }
        } catch (const std::exception& e) {
            std::cout << "MerkelMain::enterBid Bad input" << std::endl;  // Handle any exceptions from creating an order
        }
    }
}

// Prints the contents of the wallet
void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;  // Use Wallet's toString method to get string representation
}

// Advances to the next timeframe in the simulation
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);  // Match asks and bids for the product
        std::cout << "Sales: " << sales.size() << std::endl;  // Print the number of sales
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;  // Print details of each sale
            if (sale.username == "simuser")
            {
                wallet.processSale(sale);  // Update wallet based on the sale
            }
        }
    }
    currentTime = orderBook.getNextTime(currentTime);  // Move to the next available time frame
}

// Gets a user option from standard input
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-6" << std::endl;
    std::getline(std::cin, line);  // Get the full input line from user
    try {
        userOption = std::stoi(line);  // Convert input to an integer
    } catch (const std::exception& e) {
        // Input was not an integer, handle the error
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;  // Return the user-selected option
}

// Processes the user-selected option
void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // Handle bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) // Option 1: Print help
    {
        printHelp();
    }
    if (userOption == 2) // Option 2: Print market stats
    {
        printMarketStats();
    }
    if (userOption == 3) // Option 3: Enter an ask
    {
        enterAsk();
    }
    if (userOption == 4) // Option 4: Enter a bid
    {
        enterBid();
    }
    if (userOption == 5) // Option 5: Print wallet
    {
        printWallet();
    }
    if (userOption == 6) // Option 6: Go to next timeframe
    {
        gotoNextTimeframe();
    }
}
