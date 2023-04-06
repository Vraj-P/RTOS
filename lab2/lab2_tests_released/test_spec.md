### T210 task creation, max task supported test
* Description: This test checks if maximum number of tasks can be created and no more.

This testcase execute the following steps:

* Create maximum number of tasks possible. Check if the creation of tasks is successful.
* Create more tasks than the kernel supports. Check if the creation of tasks fails.
* Let the tasks created exit and then check the state of the exited tasks.
* Try to create a task with stack size greater than that available in the system. Check if the creation of tasks fails.

### T220 task priority setting and pre-emption test
* Description: This test checks the execution sequence of tasks when the task priorities are getting changed.

This testcase execute the following steps:

* Create a number of tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.

### T231
* Description: This test checks the execution sequence of tasks when the task priorities are getting changed.

This testcase execute the following steps:

* Create a number of tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.

### T232
* Description: This test checks the priority of a task after tsk_set_prio has been called on the task.

This testcase execute the following steps:

* Create tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.
* Check the priority of the task whose priority was changed.

### T232
* Description: This test checks the priority of a task after tsk_set_prio has been called on the task.

This testcase execute the following steps:

* Create tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.
* Check the priority of the task whose priority was changed.

### T233
* Description: This test initializes the kernel with four tasks of different priorities and checks the execution sequence after the tasks exit.

This testcase execute the following steps:

* Initialize the kernel with 4 tasks with priorities: HIGH, MEDIUM, LOW and LOWEST.
* Let the tasks exit and at the last task, check the execution sequence.

### T234
* Description: This test initializes the kernel with maximum number of tasks possible and checks if the number of tasks is as it should be.

This testcase execute the following steps:

* Initialize the kernel with 9 tasks.
* Check the number of non-dormant tasks in the system.
* Let all the tasks exit except one.
* Check the number of non-dormant tasks in the system.

### T240
* Description: This test checks the execution sequence of tasks when the task priorities are getting changed.

This testcase execute the following steps:

* Create a number of tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.

### T250
* Description: This test checks the execution sequence of tasks when the task priorities are getting changed.

This testcase execute the following steps:

* Create a number of tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.

### T251
* Description: This test checks the execution sequence of tasks when the task priorities are getting changed.

This testcase execute the following steps:

* Create a number of tasks.
* Change the priorities of the tasks.
* Check the execution sequence of the tasks on priority change and task yield.

### T260
* Description: This test checks the error handling of the kernel.

This testcase does the following checks:

* Check if the kernel returns a memory address from IRAM1 on memory allocation request by a task.
* Check if the user stack of a task resides on IRAM2.
* Check if the attempt to change priority of a NULL task to some other invalid priority fails.
* Check the errno in case of failure of system calls.
* Check if system calls with invalid arguments fail.
