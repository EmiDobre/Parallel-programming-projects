package task8;

import util.BSTOperations;
import util.BinarySearchTreeNode;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class Main {
    private static <T extends Comparable<T>> int calculateMaximumHeight(BinarySearchTreeNode<T> binarySearchTree) {
        if (binarySearchTree == null) {
            return 0;
        }

        return 1 + Math.max(
                calculateMaximumHeight(binarySearchTree.getRight()),
                calculateMaximumHeight(binarySearchTree.getLeft())
        );
    }

    public static void main(String[] args) throws ExecutionException, InterruptedException  {
        BinarySearchTreeNode<Integer> binarySearchTree = new BinarySearchTreeNode<>(3);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 6);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 9);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 2);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 10);
        binarySearchTree = BSTOperations.insertNode(binarySearchTree, 1);

       // System.out.println(calculateMaximumHeight(binarySearchTree));
        ForkJoinPool forkJoinPool = new ForkJoinPool();
        MaximumHeightCalculator<Integer> calculator = new MaximumHeightCalculator<>(binarySearchTree);
        forkJoinPool.execute(calculator);
        System.out.println(calculator.get());

    }
}

class MaximumHeightCalculator<T extends Comparable<T>> extends RecursiveTask<Integer> {
    private final BinarySearchTreeNode<T> tree;

    public MaximumHeightCalculator(BinarySearchTreeNode<T> tree) {
        this.tree = tree;
    }

    @Override
    protected Integer compute() {
        if (tree == null) {
            return 0;
        }

        MaximumHeightCalculator<T> left = new MaximumHeightCalculator<>(tree.getLeft());
        MaximumHeightCalculator<T> right = new MaximumHeightCalculator<>(tree.getRight());

        left.fork();
        right.fork();

        return 1 + Math.max(left.join(), right.join());
    }
}

