/* Implement this class. */

import java.util.List;

public class MyDispatcher extends Dispatcher {
    public MyDispatcher(SchedulingAlgorithm algorithm, List<Host> hosts) {
        super(algorithm, hosts);
    }
    int id = -1;
    @Override
    public void addTask(Task task) {
        if( algorithm == SchedulingAlgorithm.ROUND_ROBIN ) {
            int nrNode = (id + 1) % hosts.size();
            hosts.get(nrNode).addTask(task);
            id = nrNode;
        }
        if ( algorithm == SchedulingAlgorithm.SIZE_INTERVAL_TASK_ASSIGNMENT) {
            if ( task.getType() == TaskType.SHORT ) {
                hosts.get(0).addTask(task);
            }
            if ( task.getType() == TaskType.MEDIUM ) {
                hosts.get(1).addTask(task);
            }
            if ( task.getType() == TaskType.LONG ) {
                hosts.get(2).addTask(task);
            }
        }
        if ( algorithm == SchedulingAlgorithm.SHORTEST_QUEUE ) {
            int minID = 0;
            for ( int i = 0; i < hosts.size(); i++ ) {
                if ( hosts.get(i).getQueueSize() < hosts.get(minID).getQueueSize()) {
                    minID = i;
                }
            }
            hosts.get(minID).addTask(task);
        }

        if ( algorithm == SchedulingAlgorithm.LEAST_WORK_LEFT ) {
            int minID = 0;
            for ( int i = 0; i < hosts.size(); i++ ) {
                if ( hosts.get(i).getWorkLeft() < hosts.get(minID).getWorkLeft() ) {
                    minID = i;
                }
            }
            hosts.get(minID).addTask(task);
        }
    }
}
