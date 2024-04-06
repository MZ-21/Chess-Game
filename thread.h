#ifndef THREAD_H
#define THREAD_H

#include <stdlib.h>
#include <time.h>
#include <list>
#include <iostream>
#include <string>
#include <thread>
#include "Blockable.h"

void ThreadFunction(void * me);

class Thread {
    friend void ThreadFunction(void * me);
private:
    std::thread theThread;
    int exitTimeout;

protected:
    Sync::Event terminationEvent;

public:
    // Constructor declaration
    Thread(int exitTimeout = 1000);
    
    // Destructor declaration
    virtual ~Thread();

    // Delete the copy constructor and copy assignment operator
    Thread(Thread const &) = delete;
    Thread &operator=(Thread const &) = delete;

    // Override this function in your threads
    virtual long ThreadMain(void) = 0;

    // Method to start the thread
    void Start();
};

#endif // THREAD_H
