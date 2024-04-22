#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>

/** Construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename); // Load the orders from the specified CSV file
}

/** Return a vector of all known products in the dataset */
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;
    std::map<std::string,bool> prodMap; // Use a map to track unique products

    for (OrderBookEntry& e : orders) // Iterate through all orders
    {
        prodMap[e.product] = true; // Map each product name to true (existence)
    }
    
    // Convert the map keys to a vector of strings (product names)
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products; // Return the list of unique product names
}

/** Return vector of Orders according to the sent filters */
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                                 std::string product, 
                                                 std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders) // Filter orders based on type, product, and timestamp
    {
        if (e.orderType == type && e.product == product && e.timestamp == timestamp)
        {
            orders_sub.push_back(e); // Add matching orders to the subset vector
        }
    }
    return orders_sub; // Return the filtered orders
}

/** Return the highest price from a vector of orders */
double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price; // Start with the first order's price as the maximum
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max) max = e.price; // Update max if a higher price is found
    }
    return max; // Return the highest price
}

/** Return the lowest price from a vector of orders */
double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price; // Start with the first order's price as the minimum
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min) min = e.price; // Update min if a lower price is found
    }
    return min; // Return the lowest price
}

/** Get the earliest timestamp from the order book */
std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp; // Return the timestamp of the first order
}

/** Get the next timestamp after the given one, or loop back to the first */
std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break; // Stop at the first timestamp greater than the current one
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp; // Loop back to the first timestamp if no next timestamp is found
    }
    return next_timestamp;
}

/** Insert a new order into the order book and sort by timestamp */
void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order); // Add the new order to the end of the list
    // Sort the orders by timestamp using a defined comparator
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

/** Match ask and bid orders for a product at a specific timestamp */
std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);

    std::vector<OrderBookEntry> sales; // List to store matched sales

    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << "OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales; // Exit early if no asks or bids
    }

    // Sort asks and bids by price to facilitate matching
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

    // Display the highest and lowest asks and bids
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    // Match asks to bids
    for (OrderBookEntry& ask : asks)
    {
        for (OrderBookEntry& bid : bids)
        {
            if (bid.price >= ask.price) // Check for a price match
            {
                OrderBookEntry sale{ask.price, 0, timestamp, product, OrderBookType::asksale};

                if (bid.username == "simuser" || ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = bid.price >= ask.price ? OrderBookType::bidsale : OrderBookType::asksale;
                }

                // Determine how much of the ask and bid can be fulfilled
                if (bid.amount == ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = 0;
                    break; // Complete match, move to the next ask
                }
                else if (bid.amount > ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount -= ask.amount;
                    break; // Partial match, adjust bid and move to next ask
                }
                else if (bid.amount < ask.amount)
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount -= bid.amount;
                    bid.amount = 0;
                    continue; // Partial match, adjust ask and check next bid
                }
            }
        }
    }
    return sales; // Return all matched sales
}

