package oneProducerOneConsumer;

public class Buffer {
    private int a = -1;

    synchronized public void put(int value) {
        if (a != -1) {
            try {
                // wait for the consumer to consume
                this.wait();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        a = value;
        this.notify();
    }

    synchronized public int get() {
        if (a == -1) {
            try {
                this.wait();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        int aux = a;
        a = -1;
        this.notify();
        return aux;
    }
}
