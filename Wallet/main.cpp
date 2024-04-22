#include "Wallet.h" // Include the header file for the Wallet class
#include <iostream> // Include the standard input/output stream library
#include "MerkelMain.h" // Include the header file for the MerkelMain class

// Main function: Entry point of the program
int main()
{   
    MerkelMain app{}; // Create an instance of the MerkelMain class
    app.init(); // Initialize the application
    
    // Since there's no return statement, it implicitly returns 0, indicating successful completion
}
