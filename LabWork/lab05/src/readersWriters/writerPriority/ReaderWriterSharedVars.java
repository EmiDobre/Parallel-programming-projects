package readersWriters.writerPriority;

import java.util.concurrent.Semaphore;

public class ReaderWriterSharedVars {
    // The value to read/write
    volatile int shared_value;
    // TODO: Add semaphores and anything else needed for synchronization
    volatile int readers;
    volatile int writers;

    volatile int waitingReaders = 0;
    volatile int waitingWriters = 0;

    volatile Semaphore semWriters, semReaders, enter;

    ReaderWriterSharedVars(int init_shared_value) {
        this.shared_value = init_shared_value;
        this.readers = 0;
        this.writers = 0;

        this.waitingReaders = 0;
        this.waitingWriters = 0;

        this.semWriters = new Semaphore(0);
        this.semReaders = new Semaphore(0);
        this.enter = new Semaphore(1);
    }

}
