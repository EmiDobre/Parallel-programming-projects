package barber;

public class Barber extends Thread {
    @Override
    public void run() {
        do {
            try {
                Main.clientsSem.acquire();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }

            try {
                Main.chairsSem.acquire();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }

            Main.chairs++;

            Main.barberReadySem.release();
            Main.chairsSem.release();

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Barber served client");

        } while (checkIfThereAnyClients());
    }

    private boolean checkIfThereAnyClients() {
        int count = 0;
        for (var client: Main.leftClients) {
            if (client == 0) {
                count++;
            }
        }

        return count != 0;
    }
}
