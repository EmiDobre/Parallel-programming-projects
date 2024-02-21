package task7;

import util.BSTOperations;
import util.BinarySearchTreeNode;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {
    private static <T extends Comparable<T>> void searchValue(BinarySearchTreeNode<T> binarySearchTree, T value) {
        if (binarySearchTree != null) {
            if (value.equals(binarySearchTree.getValue())) {
                System.out.println("Found value: " + binarySearchTree.getValue());
            } else if (binarySearchTree.getValue().compareTo(value) > 0) {
                searchValue(binarySearchTree.getLeft(), value);
            } else {
                searchValue(binarySearchTree.getRight(), value);
            }
        }
    }

    public static void main(String[] args)  throws ExecutionException, InterruptedException {
        BinarySearchTreeNode<Integer> binarySearchTree = new BinarySearchTreeNode<>(3);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 6);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 9);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 2);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 10);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 1);

       // searchValue(binarySearchTree, 5)

        ExecutorService tpe = Executors.newCachedThreadPool();
        CompletableFuture<Integer> completableFuture = new CompletableFuture<>();
        AtomicInteger counter = new AtomicInteger(0);
        counter.incrementAndGet();

        tpe.submit(new SearchValue<>(tpe, binarySearchTree, 5, counter, completableFuture));

        var result = completableFuture.get();
        if (result == null) {
            System.out.println("Value was not found");
        } else {
            System.out.println("Value was found: " + result);
        }
    }
}

class SearchValue<T extends Comparable<T>> implements Runnable {
    private final ExecutorService tpe;
    private final BinarySearchTreeNode<T> tree;
    private final T value;
    private final AtomicInteger counter;
    private final CompletableFuture<T> completableFuture;

    public SearchValue(
            ExecutorService tpe,
            BinarySearchTreeNode<T> tree,
            T value,
            AtomicInteger counter, CompletableFuture<T> completableFuture
    ) {
        this.tpe = tpe;
        this.tree = tree;
        this.value = value;
        this.counter = counter;
        this.completableFuture = completableFuture;
    }

    @Override
    public void run() {
        if (tree != null) {
            if (value.equals(tree.getValue())) {
                completableFuture.complete(tree.getValue());
                tpe.shutdown();
            } else if (tree.getValue().compareTo(value) > 0) {
                counter.incrementAndGet();
                Runnable t = new SearchValue<>(tpe, tree.getLeft(), value, counter, completableFuture);
                tpe.submit(t);
            } else {
                counter.incrementAndGet();
                Runnable t = new SearchValue<>(tpe, tree.getRight(), value, counter, completableFuture);
                tpe.submit(t);
            }
        }

        int left = counter.decrementAndGet();
        if (left == 0) {
            completableFuture.complete(null);
            tpe.shutdown();
        }
    }
}

