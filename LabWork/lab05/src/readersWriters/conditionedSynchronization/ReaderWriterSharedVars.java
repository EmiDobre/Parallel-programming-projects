package readersWriters.conditionedSynchronization;

import java.util.concurrent.Semaphore;

public class ReaderWriterSharedVars {
    // The value to read/write
    volatile int shared_value;
    // TODO: Add semaphores and anything else needed for synchronization
    volatile int readers;
    volatile Semaphore readWrite, mutexNumberOfReaders;

    ReaderWriterSharedVars(int init_shared_value) {
        this.shared_value = init_shared_value;
        this.readers = 0;
        this.readWrite = new Semaphore(1);
        this.mutexNumberOfReaders = new Semaphore(1);
    }

}
