#include "Transactor.h"
#include <memory>
#include <future>
#include <thread>
#include <iostream>
#include <string>
#include <algorithm>

/// @brief Receive new message.
/// @tparam T message
/// @return new message
template <typename T>
T MessageQueue<T>::receive()
{
    // lock access
    std::unique_lock<std::mutex> lock(this->_mutex);

    // wait for new messages
    this->_condition.wait(lock, [this]
                          { return !this->_queue.empty(); });

    // pull message from the queue
    auto message = std::move(this->_queue.back());

    // clear queue
    this->_queue.pop_back();

    // return new message
    return message;
}

/// @brief Send new message.
/// @tparam T message
/// @param message message to be sent
template <typename T>
void MessageQueue<T>::send(T &&message)
{
    // create a lock guard to the resource
    std::lock_guard<std::mutex> lock(this->_mutex);

    // push message into the queue
    this->_queue.push_back(std::move(message));

    // notify new message to the condition variable
    this->_condition.notify_one();
}

Transactor::Transactor()
{
    // get number of transactions from the user
    std::cout << "How many transactions would you like me to process? ";
    std::cin >> this->_n_trx;
    std::cout << "Very well. Processing." << std::endl;

    // initialize transactions counter
    this->_processedTransactions = 0;

    // initialize message queue
    this->_messages = std::make_shared<MessageQueue<ProgramState>>();
}

Transactor::~Transactor() {}

void WaitingTransactions::pushBack(std::shared_ptr<Transaction> transaction, std::promise<void> &&promise)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _transactions.push_back(transaction);
    _promises.push_back(std::move(promise));
}

void WaitingTransactions::processFirstTransaction()
{
    std::lock_guard<std::mutex> lock(_mutex);

    // get entries from the front of both queues
    auto firstPromise = _promises.begin();
    auto firstTransaction = _transactions.begin();

    // process transaction: this could be fancy, but we really do not care.
    std::string sender = firstTransaction->get()->getSender();
    std::string recipient = firstTransaction->get()->getRecipient();
    float amount = firstTransaction->get()->getAmount();
    this->_assets[sender] += amount;
    this->_assets[recipient] -= amount;

    // fulfill promise and send signal back that permission to enter has been granted
    firstPromise->set_value();

    // remove front elements from both queues
    _transactions.erase(firstTransaction);
    _promises.erase(firstPromise);
}

std::future<void> Transactor::receiveTransaction(std::shared_ptr<Transaction> t)
{
    std::promise<void> prmsTransactionReceived;
    std::future<void> ftrTransactionReceived = prmsTransactionReceived.get_future();
    auto f = std::async(std::launch::async,
                        &WaitingTransactions::pushBack,
                        &this->_waitingTransactions,
                        t, std::move(prmsTransactionReceived));
    return ftrTransactionReceived;
}

void Transactor::processTransaction()
{
    auto f = std::async(std::launch::async,
                        &WaitingTransactions::processFirstTransaction,
                        &this->_waitingTransactions);
}

void Transactor::listen()
{
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    auto received_status = _messages->receive();
    int exec_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();

    // lock standard output channel
    std::lock_guard<std::mutex> lock(this->_mutex);
    std::cout << "Execution time (ms): " << exec_time << std::endl;
    std::cout << this->_processedTransactions << " processed transactions." << std::endl;
}

void Transactor::transact(std::string sender, std::string recipient, float amount)
{
    // Spawn transaction and get completion future by pushing it into queue
    std::shared_ptr<Transaction> t = std::make_shared<Transaction>(sender, recipient, amount);
    std::future<void> trx_completed_future = this->receiveTransaction(t);

    // Process transaction
    this->processTransaction();

    // Wait and count transaction completion
    trx_completed_future.wait();
    this->_processedTransactions++;
    if (this->_processedTransactions == this->_n_trx)
    {
        // send off execution end message
        auto f_send = std::async(std::launch::async,
                                 &MessageQueue<ProgramState>::send,
                                 this->_messages, std::move(ProgramState::STOP));
        f_send.wait();
    }
}
