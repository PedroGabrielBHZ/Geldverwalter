#include <thread>
#include <memory>
#include "Transactor.h"
#include "BookTeller.h"

int main()
{
    // Spawn Book Teller.
    BookTeller book_teller = BookTeller("../database/assets");
    book_teller.readBook();

    // Spawn Transactor and receive book from Book Teller.
    Transactor transactor = Transactor();
    transactor.receiveBook(std::move(book_teller.giveBook()));

    // Listen to transactions async, finish execution when all transactions are processed.
    auto thread_listen = std::async(std::launch::async,
                                    &Transactor::listen,
                                    &transactor);

    // Spawn X mock transactions.
    for (int i = 0; i < transactor.getNumberOfTransactions() / 2; i++)
    {
        transactor.transact("a", "b", 1);
        transactor.transact("b", "a", 1);
    }

    // Await end of execution.
    thread_listen.wait();

    // Give Transactor's book to Book Teller, write final assets.
    book_teller.receiveBook(std::move(transactor.giveBook()));
    book_teller.writeBook();

    // End main execution.
    return 0;
}