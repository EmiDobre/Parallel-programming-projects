package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;

public class Buffer {
    
    Queue<Integer> queue;
    public static Semaphore consumers;
    public static Semaphore producers;
    
    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
        consumers = new Semaphore(0);
        producers = new Semaphore(size);
    }

	public void put(int value) {
        try {
            producers.acquire();
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        synchronized (queue) {
            queue.add(value);
        }
        consumers.release();
	}

	public int get() {
        try {
            consumers.acquire();
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }

        int a = -1;
        synchronized (queue) {
            Integer result = queue.poll();
            if (result != null) {
                a = result;
            }
        }
        producers.release();

        return a;
	}
}
