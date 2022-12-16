#ifndef BOOKTELLER
#define BOOKTELLER

#include <string>
#include <unordered_map>

class BookTeller
{
public:
    BookTeller(std::string book_address);
    void readBook();
    void writeBook();
    void receiveBook(std::unordered_map<std::string, float> book) { this->_book = book; }
    std::unordered_map<std::string, float> giveBook() { return this->_book; }

private:
    std::string _book_address;
    std::unordered_map<std::string, float> _book;
};

#endif
