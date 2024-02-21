package shortestPathsFloyd_Warshall;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

class MyThread implements Runnable {
    int id;
    int N;
    int P;
    int[][] graph;

    MyThread(int id, int N, int P, int[][] graph) {
        this.id = id;
        this.N = N;
        this.P = P;
        this.graph = graph;
    }

    @Override
    public void run() {
        int start = id * N / P;
        int end = (id + 1) * N / P;
        if ( end > N )
            end = N;

        for (int k = 0; k < 5; k++) {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < 5; j++) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
            try {
                //Resincronizarea thread-urilor pentru urmatorul pas al algoritmului.
                Main.barrier.await();
            } catch (BrokenBarrierException | InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

public class Main {

    public static CyclicBarrier barrier;

    public static void main(String[] args) {
        int M = 9;
        int[][] graph = {{0, 1, M, M, M},
                {1, 0, 1, M, M},
                {M, 1, 0, 1, 1},
                {M, M, 1, 0, M},
                {M, M, 1, M, 0}};

        int[][] graph_Paralel = {{0, 1, M, M, M},
                {1, 0, 1, M, M},
                {M, 1, 0, 1, 1},
                {M, M, 1, 0, M},
                {M, M, 1, M, 0}};

        // Parallelize me (You might want to keep the original code in order to compare)
        for (int k = 0; k < 5; k++) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }

        //Paralelizare:
        int P = 4, N = 5; //dim matrice = 5
        Thread[] threads = new Thread[P];
        barrier = new CyclicBarrier(P);

        //creare threaduri si pornire lor (executare in paralel a tuturor)
        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread(i, N, P, graph_Paralel));
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

        //verificare:
        System.out.println("Paralelizat:");
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph_Paralel[i][j] + " ");
                if( graph_Paralel[i][j] != graph[i][j]) {
                    System.out.println("nu e ok");
                    break;
                }
            }
            System.out.println();
        }
    }
}
