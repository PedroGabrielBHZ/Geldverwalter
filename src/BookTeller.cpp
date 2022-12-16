#include <fstream>
#include <iostream>

#include "BookTeller.h"

BookTeller::BookTeller(std::string book_address) : _book_address(book_address){};

// Update the database with new licence plates from file
void BookTeller::readBook()
{
    // Initialize asset book
    this->_book.clear();

    // Initialize filestream
    std::fstream book(this->_book_address);
    if (!book)
    {
        std::cerr << "[ERROR] Could not open book." << std::endl;
        return;
    }

    // Read assets line by line and close book.
    std::string account;
    float assets;
    while (book >> account >> assets)
    {
        this->_book[account] = assets;
    }
    book.close();

    std::cout << "[INFO] Book successfully read." << std::endl;
}

void BookTeller::writeBook()
{
    // Initialize filestream
    std::fstream book(this->_book_address, std::fstream::out | std::fstream::trunc);
    if (!book)
    {
        std::cerr << "[ERROR] Could not open book." << std::endl;
        return;
    }

    // Process each line of the file
    for (auto i : this->_book)
    {
        book << i.first << " " << i.second << std::endl;
    }

    std::cout << "[INFO] Book successfully written." << std::endl;
}
