#include "CSVReader.h"
#include <iostream>
#include <fstream>

// Constructor for CSVReader
CSVReader::CSVReader()
{
    // Empty constructor: no initialization needed here
}

// Reads a CSV file and returns a vector of OrderBookEntry objects
std::vector<OrderBookEntry> CSVReader::readCSV(std::string csvFilename)
{
    std::vector<OrderBookEntry> entries; // To store the entries from the CSV file

    std::ifstream csvFile{csvFilename}; // Open the CSV file
    std::string line;
    if (csvFile.is_open()) // Check if the file has been successfully opened
    {
        while(std::getline(csvFile, line)) // Read each line of the CSV file
        {
            try {
                OrderBookEntry obe = stringsToOBE(tokenise(line, ',')); // Convert the line to OrderBookEntry
                entries.push_back(obe); // Add the entry to the vector
            } catch(const std::exception& e) {
                std::cout << "CSVReader::readCSV bad data" << std::endl; // Handle any parsing errors
            }
        }
    }

    std::cout << "CSVReader::readCSV read " << entries.size() << " entries" << std::endl; // Print the number of entries read
    return entries; // Return the vector of entries
}

// Splits a given line into tokens based on the specified separator
std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator)
{
    std::vector<std::string> tokens; // To store the tokens
    signed int start, end;
    std::string token;
    start = csvLine.find_first_not_of(separator, 0);
    do {
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) break; // Stop if end of line or empty token
        if (end >= 0) token = csvLine.substr(start, end - start); // Extract the token
        else token = csvLine.substr(start, csvLine.length() - start);
        tokens.push_back(token); // Add the token to the vector
        start = end + 1; // Move start to the next character after the separator
    } while(end > 0);

    return tokens; // Return the vector of tokens
}

// Converts a vector of strings to an OrderBookEntry object
OrderBookEntry CSVReader::stringsToOBE(std::vector<std::string> tokens)
{
    double price, amount;

    if (tokens.size() != 5) // Check if the line has exactly 5 tokens
    {
        std::cout << "Bad line " << std::endl;
        throw std::exception{}; // Throw an exception if not
    }

    try {
        price = std::stod(tokens[3]); // Convert the price to a double
        amount = std::stod(tokens[4]); // Convert the amount to a double
    } catch(const std::exception& e) {
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[3] << std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[4] << std::endl;
        throw; // Re-throw the exception if conversion fails
    }

    OrderBookEntry obe{price, amount, tokens[0], tokens[1], OrderBookEntry::stringToOrderBookType(tokens[2])};
    return obe; // Construct and return the OrderBookEntry object
}
// Alternative method to create an OrderBookEntry from individual string components
OrderBookEntry CSVReader::stringsToOBE(std::string priceString, 
                                       std::string amountString, 
                                       std::string timestamp, 
                                       std::string product, 
                                       OrderBookType orderType)
{
    double price, amount;
    try {
        price = std::stod(priceString); // Convert the price string to a double
        amount = std::stod(amountString); // Convert the amount string to a double
    } catch(const std::exception& e) {
        std::cout << "CSVReader::stringsToOBE Bad float! " << priceString << std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << amountString << std::endl;
        throw; // Throw exception if conversion fails
    }
    OrderBookEntry obe{price, amount, timestamp, product, orderType};
    return obe; // Construct and return the OrderBookEntry object
}