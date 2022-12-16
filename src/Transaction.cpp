#include "Transaction.h"

#include <string>
#include <chrono>
#include <iostream>

/// @brief Default constructor.
Transaction::Transaction(std::string sender, std::string recipient, float amount)
    : _sender(sender), _recipient(recipient), _amount(amount) {}

/// @brief Default destructor.
Transaction::~Transaction() {}

// Getters
std::string Transaction::getSender() { return this->_sender; }
std::string Transaction::getRecipient() { return this->_recipient; }
float Transaction::getAmount() { return this->_amount; }