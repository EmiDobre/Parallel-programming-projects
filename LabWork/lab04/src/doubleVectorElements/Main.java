package doubleVectorElements;
class MyThread implements Runnable {
    int id;
    static int v[];
    int N;
    int P;

    MyThread(int id, int N, int[] v, int P) {
        this.id = id;
        this.N = N;
        this.v = v;
        this.P = P;
    }

    @Override
    public void run() {
        int start = id * N / P;
        int end = (id + 1) * N / P;
        if ( end > N )
            end = N;
        for (int i = start; i < end; i++) {
            v[i] = v[i] * 2;
        }
    }
}
public class Main {
    public static void main(String[] args) {
        int N = 100000013;
        int[] v = new int[N];
        int P = 4; // the program should work for any P <= N

        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        // Parallelize me using P threads
        Thread[] threads = new Thread[P];

        //creare threaduri si pornire lor (executare in paralel a tuturor)
        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread(i, N, v, P));
            threads[i].start();
        }

        //threadurile se opresc unu cate unu
        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }

}
