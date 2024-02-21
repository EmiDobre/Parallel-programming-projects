package barber;

public class Client extends Thread {
    private final int id;

    public Client(int id) {
        super();
        this.id = id;
    }

    @Override
    public void run() {
        try {
            Main.chairsSem.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (Main.chairs > 0) {
            Main.chairs--;

            System.out.println("Client " + id + " is waiting for haircut");
            System.out.println("Available seats: " + Main.chairs);

            Main.clientsSem.release();

            Main.chairsSem.release();

            try {
                Main.barberReadySem.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Client " + id + " is served by the barber");

            Main.leftClients[id] = Main.SERVED_CLIENT;

        } else {
            Main.chairsSem.release();
            System.out.println("Client " + id + " left unserved");
            Main.leftClients[id] = Main.UNSERVED_CLIENT;
        }
    }
}
