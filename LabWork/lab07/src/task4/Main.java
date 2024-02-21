package task4;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class Main {
    static int[][] graph = {{0, 1}, {0, 4}, {0, 5}, {1, 0}, {1, 2}, {1, 6}, {2, 1}, {2, 3}, {2, 7},
            {3, 2}, {3, 4}, {3, 8}, {4, 0}, {4, 3}, {4, 9}, {5, 0}, {5, 7}, {5, 8}, {6, 1},
            {6, 8}, {6, 9}, {7, 2}, {7, 5}, {7, 9}, {8, 3}, {8, 5}, {8, 6}, {9, 4}, {9, 6},
            {9, 7}};

    static void getPath(ArrayList<Integer> partialPath, int destination) {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
            return;
        }

        // se verifica nodurile pentru a evita ciclarea in graf
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                getPath(newPartialPath, destination);
            }
        }
    }

    public static void main(String[] args) {
        ArrayList<Integer> partialPath = new ArrayList<>();
        // se vor calcula toate caile de la nodul 0 la nodul 3 in cadrul grafului
        partialPath.add(0);

        ForkJoinPool fjp = new ForkJoinPool(4);
        fjp.invoke(new MyTask(partialPath, 3));
        fjp.shutdown();

//        getPath(partialPath, 3);
    }
}

class MyTask extends RecursiveTask<Void> {
    private final ArrayList<Integer> path;
    private final int end;

    public MyTask(ArrayList<Integer> path, int end) {
        this.path = path;
        this.end = end;
    }

    @Override
    protected Void compute() {
        if (path.get(path.size() - 1) == end) {
            System.out.println(path);
            return null;
        } else {
            int lastNode = path.get(path.size() - 1);
            List<MyTask> tasks = new ArrayList<>();
            for (int[] ints : Main.graph) {
                if (ints[0] == lastNode && !path.contains(ints[1])) {
                    ArrayList<Integer> newPath = new ArrayList<>(path);
                    newPath.add(ints[1]);
                    MyTask t = new MyTask(newPath, end);
                    tasks.add(t);
                    t.fork();
                }
            }
            for (MyTask t : tasks) {
                t.join();
            }
        }

        return null;
    }
}