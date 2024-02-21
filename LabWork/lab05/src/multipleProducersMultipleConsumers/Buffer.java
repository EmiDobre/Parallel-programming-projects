package multipleProducersMultipleConsumers;

import java.util.logging.Level;
import java.util.logging.Logger;

public class Buffer {
    int a = -1;

    synchronized public void put(int value) {
        while (a != -1) {
            try {
                this.wait();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        a = value;
        this.notifyAll();
    }

    synchronized public int get() {
        while (a == -1) {
            try {
                this.wait();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        int aux = a;
        a = -1;
        this.notifyAll();
        return aux;
    }
}