/*INCLUDES ************************************************************/
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "util/atomic.h"

/* PRIVATE VARIABLES **************************************************/
static taskDescriptor *taskList = NULL;		   //< list of scheduled tasks
volatile static systemTime_t milli_second = 0; //< time keeping variable for the clock

/*FUNCTION DEFINITION *************************************************/
static void scheduler_update(void)
{
	taskDescriptor *current_pointer = taskList;
	milli_second++;
	while (current_pointer)
	{
		current_pointer->expire--; //< decrement expire every 1ms

		if (current_pointer->expire == 0)
		{
			current_pointer->expire = current_pointer->period; //< for periodic tasks, expire value gets re-initialized
			current_pointer->execute = 1;					   //< the task is ready to execute
		}

		current_pointer = current_pointer->next;
	}
}

void scheduler_init() //< timer 2 initialization and running scheduler_update as timer2 callback
{
	timer2_start();
	timer2_setCallback(scheduler_update);
}

void scheduler_run()
{
	taskDescriptor *current_task, *to_delete;

	while (1)
	{
		current_task = taskList;

		while (current_task)
		{
			if (current_task->execute == 1)
			{
				current_task->task(current_task->param);
				current_task->execute = 0; //< resetting execute flag, so that task can wait again until expire is zero
			}
			if (current_task->period == 0) //< delete tasks that will run once i.e period is zero
			{
				to_delete = current_task;
				scheduler_remove(to_delete);
			}
			current_task = current_task->next;
		}
	}
}

bool scheduler_add(taskDescriptor *toAdd) //< adds the new task in the beginning of the tasklist
{
	taskDescriptor *new_node = toAdd;
	bool task_status = false;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) //< interrupt is disabled until new task is added
	{
		new_node->next = taskList;
		taskList = new_node;

		if (taskList)
		{
			if (new_node->period == 0)
			{
				new_node->execute = 1; //< non periodic tasks can start execution once
			}
			else
			{
				new_node->execute = 0;
			}
			task_status = true;
		}
	}
	return task_status;
}

void scheduler_remove(taskDescriptor *toRemove)
{
	taskDescriptor *current;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) //< interrupt is disabled while task is being removed
	{
		if (toRemove == taskList) //< removing the first node in the tasklist
		{
			current = taskList->next;
			free(taskList);
			taskList = current;
		}
		else //< if task to be removed is not in the first node
		{
			current = taskList;
			while (current)
			{
				if (current->next == toRemove) //< if task to be removed in the second node
				{
					current->next = toRemove->next;
					free(toRemove);
				}
				else
				{
					current = current->next; //< keep iterating through the tasklist
				}
			}
		}
	}
}

taskDescriptor *create_new_task(task_t task, uint16_t period, uint16_t expire, void *param) //< create new tasks based on parameter passed from main
{
	taskDescriptor *new_task = NULL;
	new_task = (taskDescriptor *)malloc(sizeof(taskDescriptor));

	if (new_task != NULL)
	{
		new_task->next = NULL;
		new_task->task = task;
		new_task->param = param;
		new_task->period = period;
		new_task->expire = expire;
		new_task->execute = 0;
	}
	return new_task; //< using scheduler_add function new task can be added at an appropriate position
}

/******* Sets the system time or returns it for the alarm clock FSM *********/
systemTime_t scheduler_getTime()
{
	return milli_second;
}

void scheduler_setTime(systemTime_t time)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		milli_second = time;
	}
}
