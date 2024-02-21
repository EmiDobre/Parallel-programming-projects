/* Implement this class. */

import javax.swing.text.html.parser.Parser;
import java.util.Comparator;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.PriorityBlockingQueue;

public class MyHost extends Host {
    private BlockingQueue<Task> taskQueue = new PriorityBlockingQueue<>(11, new Comparator<Task>() {
        @Override
        public int compare(Task o1, Task o2) {
            if(o1.getPriority() == o2.getPriority()) {
                return o1.getStart() - o2.getStart();
            }
            return o2.getPriority() - o1.getPriority();
        }
    });

    Task runningTask = null;
    long timeLeft;
    Boolean running = true;

    public void changePreemptible() {
        if ( !taskQueue.isEmpty() ) {
            try {
                Task nextTask = taskQueue.take();
                if ( nextTask.getPriority() > runningTask.getPriority() ) {
                    //1: opresc task curent - execut next
                    taskQueue.put(runningTask);
                    runningTask = nextTask;
                    timeLeft = runningTask.getLeft();
                } else {
                    //2: continui executia normal, pun la loc nextTask
                    taskQueue.put(nextTask);
                }
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }

        } //altfel continui executia normal
    }

    @Override
    public void run() {
        while (running) {
            while (!taskQueue.isEmpty()) {
                try {
                    runningTask = taskQueue.take();
                    timeLeft = runningTask.getLeft();
                    while (timeLeft >= 0) {
                        //1)In timpul executiei verific daca ajunge alt task
                        //cu prio mai mare:
                        if ( runningTask.isPreemptible() )
                            changePreemptible();

                        //2)Executie:
                        Thread.sleep(100);
                        timeLeft -= 100;
                        runningTask.setLeft(timeLeft);
                    }
                    runningTask.finish();
                } catch (InterruptedException e) {

                }
            }
        }
    }

    @Override
    public void addTask(Task task) {
        try {
            taskQueue.put(task);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int getQueueSize() {
        if ( runningTask != null )
            return taskQueue.size() + 1;
        else
            return  taskQueue.size();
    }

    @Override
    public long getWorkLeft() {
        long workLeft = 0;
        for (Task task : taskQueue ) {
            workLeft += task.getLeft();
        }
        if ( runningTask != null )
            workLeft += runningTask.getLeft();
        return workLeft;
    }

    @Override
    public void shutdown() {
        running = false;
    }
}
