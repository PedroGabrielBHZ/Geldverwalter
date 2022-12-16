#ifndef TRANSACTOR
#define TRANSACTOR

#include "Transaction.h"
#include <future>
#include <vector>
#include <deque>
#include <mutex>
#include <memory>
#include <unordered_map>

/// @brief Transaction status
enum ProgramState
{
    STOP
};

/// @brief Message queue that receives and sends messages asynchronously.
/// @tparam T generic message template
template <class T>
class MessageQueue
{
public:
    void send(T &&message);
    T receive();
    int getSize() { return this->_queue.size(); }

private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

class WaitingTransactions
{
public:
    // getters / setters
    std::unordered_map<std::string, float> getAssets() { return this->_assets; };

    // typical behaviour methods
    void processFirstTransaction();
    void pushBack(std::shared_ptr<Transaction> transaction, std::promise<void> &&promise);
    std::unordered_map<std::string, float> _assets;

private:
    std::vector<std::shared_ptr<Transaction>> _transactions;
    std::vector<std::promise<void>> _promises;
    std::mutex _mutex;
};

class Transactor
{
public:
    Transactor();
    ~Transactor();
    void processTransaction();
    void listen();
    void transact(std::string sender, std::string recipient, float amount);
    std::unordered_map<std::string, float> giveBook() { return this->_waitingTransactions._assets; }
    void receiveBook(std::unordered_map<std::string, float> book)
    {
        this->_waitingTransactions._assets = book;
    }
    int getNumberOfTransactions() { return this->_n_trx; }

private:
    int _n_trx;
    int _processedTransactions;
    std::mutex _mutex;
    WaitingTransactions _waitingTransactions;
    std::shared_ptr<MessageQueue<ProgramState>> _messages;
    std::future<void> receiveTransaction(std::shared_ptr<Transaction> t);
};

#endif