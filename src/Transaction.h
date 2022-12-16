#ifndef TRANSACTION
#define TRANSACTION

#include <string>
#include <chrono>

class Transaction
{
public:
    Transaction() = default;
    Transaction(std::string sender, std::string recipient, float amount);
    ~Transaction();
    std::string getSender();
    std::string getRecipient();
    float getAmount();

private:
    float _amount;
    std::string _sender;
    std::string _recipient;
};

#endif
