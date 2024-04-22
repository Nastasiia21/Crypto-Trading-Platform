#include "OrderBookEntry.h"

// Constructor for creating an OrderBookEntry object with initial values for each member variable.
OrderBookEntry::OrderBookEntry(double _price, 
                               double _amount, 
                               std::string _timestamp, 
                               std::string _product, 
                               OrderBookType _orderType, 
                               std::string _username)
: price(_price),         // Initialize price with _price
  amount(_amount),       // Initialize amount with _amount
  timestamp(_timestamp), // Initialize timestamp with _timestamp
  product(_product),     // Initialize product with _product
  orderType(_orderType), // Initialize orderType with _orderType
  username(_username)    // Initialize username with _username
{
    // Constructor body is empty as all initialization is done in the initializer list.
}

// Converts a string to an OrderBookType enumeration. The string represents the type of order: "ask" or "bid".
OrderBookType OrderBookEntry::stringToOrderBookType(std::string s)
{
  if (s == "ask") 
  {
    return OrderBookType::ask;  // Return 'ask' type if string is "ask"
  }
  if (s == "bid")
  {
    return OrderBookType::bid;  // Return 'bid' type if string is "bid"
  }
  return OrderBookType::unknown; // Return 'unknown' type if the string does not match expected values
}
