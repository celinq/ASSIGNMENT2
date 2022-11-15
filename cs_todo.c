#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_PRIORITY -1

#define MAX_TASK_LENGTH 200
#define MAX_CATEGORY_LENGTH 40
#define MAX_STRING_LENGTH 1024

// You *should* #define each command
#define COMMAND_ADD_TASK 'a'
#define COMMAND_PRINT_TASK 'p'
#define COMMAND_INCREASE_TASK 'i'
#define COMMAND_NUM_TASK 'n'
#define COMMAND_COMPLETE_TASK 'c'
#define COMMAND_PRINT_COMPLETED_TASK 'P'
#define COMMAND_EXPECTED_TIME 'e'

enum priority { LOW, MEDIUM, HIGH };

struct task {
    char task_name[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    enum priority priority;

    struct task *next;
};

struct completed_task {
    struct task *task;
    int start_time;
    int finish_time;
    struct completed_task *next;
};

struct todo_list {
    struct task *tasks;
    struct completed_task *completed_tasks;
};

////////////////////////////////////////////////////////////////////////////////
///////////////////// YOUR FUNCTION PROTOTYPES GO HERE /////////////////////////
////////////////////////////////////////////////////////////////////////////////



void command_loop(struct todo_list *todo);
static struct task *create_new_task(
    char task_name[MAX_TASK_LENGTH], 
    char task_category[MAX_CATEGORY_LENGTH], 
    enum priority *prio
);
static struct task *find_task(
    struct todo_list *todo,
    char task[MAX_TASK_LENGTH], 
    char category[MAX_CATEGORY_LENGTH]
);
static struct completed_task *create_new_completed_task(
    struct task *task,
    int start_time,
    int finish_time
);
int average_time(struct todo_list *todo, char category[MAX_CATEGORY_LENGTH]);



////////////////////////////////////////////////////////////////////////////////
//////////////////////// PROVIDED HELPER PROTOTYPES ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void parse_add_task_line(
    char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH], enum priority *prio
);
void parse_task_category_line(
    char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH]
);
void parse_complete_task_line(
    char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH], int *start_time, int *finish_time
);

enum priority string_to_priority(char priority[MAX_STRING_LENGTH]);
void remove_newline(char input[MAX_STRING_LENGTH]);
void trim_whitespace(char input[MAX_STRING_LENGTH]);
void print_one_task(int task_num, struct task *task);
void print_completed_task(struct completed_task *completed_task);

int task_compare(struct task *t1, struct task *t2);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {
    // Stage 1.1
    // You should initialize the `todo` variable below. You will need
    // to use the malloc() function to allocate memory for it!

    printf("Welcome to CS ToDo!\n");

    // Initialize todo, and set tasks and completed tasks to NULL
    struct todo_list *todo = malloc(sizeof(struct todo_list));
    todo->tasks = NULL;
    todo->completed_tasks = NULL;
    
    command_loop(todo);

    printf("All done!\n");
    
    return 0;
}

/**
 * The central loop that executes commands until the program is completed.
 *
 * Parameters:
 *     todo - The todo list to execute commands on.
 *
 * Returns:
 *     Nothing
 */
void command_loop(struct todo_list *todo) {
    printf("Enter Command: ");
    char command;
    while (scanf(" %c", &command) == 1) {
        // Stage 1.2
        if (command == COMMAND_ADD_TASK) {
            // Get inputs from the command line (code sampled from spec Stage 1.2)
            char buffer[MAX_STRING_LENGTH];
            fgets(buffer, MAX_STRING_LENGTH, stdin);

            char task_name[MAX_TASK_LENGTH];
            char task_category[MAX_CATEGORY_LENGTH];
            enum priority task_priority;
            parse_add_task_line(buffer, task_name, task_category, &task_priority);
            
            // Create a new task
            struct task *new_task = create_new_task(
                task_name, 
                task_category, 
                &task_priority
            );
            // Add task if todo list is empty
            if (todo->tasks == NULL) {
                todo->tasks = new_task;
            } else {
                // Traverse through tasks in todo, and insert new task at the end
                struct task *temp_task = todo->tasks;
                while (temp_task->next != NULL) {
                    temp_task = temp_task->next;
                }
                temp_task->next = new_task;
            }
        }
        
        // Stage 1.3
        if (command == COMMAND_PRINT_TASK) {
            printf("==== Your ToDo List ====\n");
            // No tasks in todo list
            if (todo->tasks == NULL) {
                printf("All tasks completed, you smashed it!\n");
            } else {
                // Traverse through tasks in todo list and print
                struct task *temp_task = todo->tasks;
                int counter = 1;
                while(temp_task != NULL) {
                    print_one_task(counter, temp_task);
                    temp_task = temp_task->next;
                    counter++;
                }
            }
            printf("====   That's it!   ====\n");
        }
        
        // Stage 1.4
        if (command == COMMAND_INCREASE_TASK) {
            // Get inputs from the command line (code smapled from spec Stage 1.4)
            char buffer[MAX_STRING_LENGTH];
            fgets(buffer, MAX_STRING_LENGTH, stdin);

            char task[MAX_TASK_LENGTH];
            char category[MAX_CATEGORY_LENGTH];
            parse_task_category_line(buffer, task, category);

            // Find the corresponding task within the category
            struct task *temp_task = find_task(todo, task, category);
            if (temp_task != NULL) {
                if (temp_task->priority == 0) {
                    temp_task->priority = 1;
                } else if (temp_task->priority == 1) {
                    temp_task->priority = 2;
                } else if (temp_task->priority == 2) {
                    temp_task->priority = 0;
                }
            }
        }

        // Stage 1.5
        if (command == COMMAND_NUM_TASK) {
            int num_tasks = 0;
            // Traverse through tasks in todo and count
            struct task *temp_task = todo->tasks;
            while (temp_task != NULL) {
                num_tasks++;
                temp_task = temp_task->next;
            }

            // Print how many tasks in todo list
            printf("There are %d items on your list!\n", num_tasks);
        }

        // Stage 2.1
        if (command == COMMAND_COMPLETE_TASK) {
            // Get inputs from the command line (code sampled from spec Stage 2.1)
            char buffer[MAX_STRING_LENGTH];
            fgets(buffer, MAX_STRING_LENGTH, stdin);

            char task[MAX_TASK_LENGTH];
            char category[MAX_CATEGORY_LENGTH];
            int start_time;
            int finish_time;
            parse_complete_task_line(buffer, task, category, &start_time, &finish_time);


            if (find_task(todo, task, category) != NULL) {
                // Create a mew completed task
                struct completed_task *new_completed_task = create_new_completed_task(
                    find_task(todo, task, category),
                    start_time,
                    finish_time
                );

                // Add completed task if todo list is empty
                if (todo->completed_tasks == NULL) {
                    todo->completed_tasks = new_completed_task;
                } else {
                    // Insert new completed task in the beginning of the list
                    new_completed_task->next = todo->completed_tasks;
                    todo->completed_tasks = new_completed_task;
                }

                // Remove task from todo tasks
                // If task is first in todo list (head)
                struct task *temp_task = todo->tasks;
                if (temp_task == find_task(todo, task, category)) {
                    todo->tasks = todo->tasks->next;
                // If task is last in the todo list (tail)
                } else if (find_task(todo, task, category)->next == NULL) {
                    while (temp_task->next->next != NULL) {
                        temp_task = temp_task->next;
                    }
                    temp_task->next = NULL;
                // If task is in middle of todo list
                } else {
                    while (temp_task->next != find_task(todo, task, category)) {
                        if (temp_task->next != NULL) {
                            temp_task = temp_task->next;
                        }
                    }
                    temp_task->next = temp_task->next->next;
                }
            }
        }

        // Stage 2.2
        if (command == COMMAND_PRINT_COMPLETED_TASK) {
            printf("==== Completed Tasks ====\n");
            // No completed tasks in todo list
            if (todo->completed_tasks == NULL) {
                printf("No tasks have been completed today!\n");
            } else {
                struct completed_task *temp_completed_task = todo->completed_tasks;
                while (temp_completed_task != NULL) {
                    print_completed_task(temp_completed_task);
                    temp_completed_task = temp_completed_task->next;
                }
            }
            printf("=========================\n");
        }

        // Stage 2.3
        if (command == COMMAND_EXPECTED_TIME) {
            printf("Expected completion time for remaining tasks:\n\n");
            struct task *current_task = todo->tasks;
            int counter = 1;
            while (current_task != NULL) {
                print_one_task(counter, current_task);
                int time = average_time(todo, current_task->category);
                printf("Expected completion time: %d minutes\n\n", time);
        
                counter++;
                current_task = current_task->next;
            }
        }

        printf("Enter Command: ");
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////// YOUR HELPER FUNCTIONS ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// You should add any helper functions you create here

/**
 * Create a new task and return a pointer to it
 *
 * Parameters:
 *     task_name - The name of the task
 *     task_category - The category of the task
 *     prio - The priority of the task
 *
 * Returns:
 *     struct task *new_task
 */
static struct task *create_new_task(
    char task_name[MAX_TASK_LENGTH], 
    char task_category[MAX_CATEGORY_LENGTH], 
    enum priority *prio
) {
    // Allocate memory for the task and set properties
    struct task *new_task = malloc(sizeof(struct task));
    strcpy(new_task->task_name, task_name);
    strcpy(new_task->category,task_category);
    new_task->priority = *prio;
    // Initialize next task as NULL
    new_task->next = NULL;

    return new_task;
}

/**
 * Find matching task given name and category
 *
 * Parameters:
 *     task - The name of the task
 *     task - The category of the task
 *
 * Returns:
 *     struct task *temp_task - The matching task
*      or NULL if no matching task
 */
static struct task *find_task(
    struct todo_list *todo,
    char task[MAX_TASK_LENGTH], 
    char category[MAX_CATEGORY_LENGTH]
) {
    struct task *temp_task = todo->tasks;
    while (temp_task != NULL) {
        if (strcmp(temp_task->task_name, task) == 0 &&
            strcmp(temp_task->category, category) == 0) {
            return temp_task;
        }
        temp_task = temp_task->next;
    }
    printf("Could not find task '%s' in category '%s'.\n", task, category);
    return NULL;
}

/**
 * Create a new completed task and return a pointer to it
 *
 * Parameters:
 *     task - task struct of the completed task
 *     start_time - time task started in minutes
 *     finish_time - time task finished in minutes
 *
 * Returns:
 *     struct completed_task *new_completed task
 */
static struct completed_task *create_new_completed_task(
    struct task *task,
    int start_time,
    int finish_time
) {
    // Allocated memory for the completed task and set properties
    struct completed_task *new_completed_task = malloc(sizeof(struct completed_task));
    new_completed_task->task = task;
    new_completed_task->start_time = start_time;
    new_completed_task->finish_time = finish_time;
    // Initialize next completed_task as NULL
    new_completed_task->next = NULL;

    return new_completed_task;
}

/**
 * Get average time to complete a task given category
 *
 * Parameters:
 *     task - task struct of the completed task
 *     start_time - time task started in minutes
 *     finish_time - time task finished in minutes
 *
 * Returns:
 *     struct completed_task *new_completed task
 */
int average_time(struct todo_list *todo, char category[MAX_CATEGORY_LENGTH]) {
    // Initialize time = 0
    int total_time = 0;
    int matching_category = 0;
    // Traverse through completed tasks, if category matches, 
    // add the time taken to complete to total
    struct completed_task *temp_completed_task = todo->completed_tasks;
    while (temp_completed_task != NULL) {
        if (strcmp(temp_completed_task->task->category, category) == 0) {
            total_time += (temp_completed_task->finish_time - temp_completed_task->start_time);
            matching_category++;
        }
        temp_completed_task = temp_completed_task->next;
    }
    // If no matching categories, return 100 minutes
    if (matching_category == 0) {
        return 100;
    } else {
        return total_time / matching_category;
    }
}


////////////////////////////////////////////////////////////////////////////////
/////////////////////// PROVIDED HELPER FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * Given a raw string in the format: [string1] [string2] [enum priority]
 * This function will extract the relevant values into the given variables.
 * The function will also remove any newline characters.
 *
 * For example, if given: "finish_assignment_2 assignment2 high"
 * The function will copy the string:
 *     "finish_assignment_2" into the `task_name` array
 * Then copy the string:
 *     "assignment2" into the `task_category` array
 * And finally, copy over the enum:
 *     "high" into the memory that `prio` is pointing at.
 *
 * Parameters:
 *     buffer        - A null terminated string in the following format
 *                     [string1] [string2] [enum priority]
 *     task_name     - A character array for the [string1] to be copied into
 *     task_category - A character array for the [string2] to be copied into
 *     prio          - A pointer to where [enum priority] should be stored
 *
 * Returns:
 *     None
 */
void parse_add_task_line(
    char buffer[MAX_STRING_LENGTH],
    char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH],
    enum priority *prio
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }
    
    // Extract value 3 as string
    char *prio_str = strtok(NULL, " ");
    if (prio_str != NULL) {
        *prio = string_to_priority(prio_str);
    }

    if (
        name_str == NULL ||
        category_str == NULL ||
        prio_str == NULL ||
        *prio == INVALID_PRIORITY
    ) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/*
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * See `parse_add_task_line` for explanation - This function is very similar,
 * with only the exclusion of an `enum priority`.
 */
void parse_task_category_line(
    char buffer[MAX_STRING_LENGTH],
    char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH]
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }

    if (name_str == NULL || category_str == NULL) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/*
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * See `parse_add_task_line` for explanation - This function is very similar,
 * with only the exclusion of an `enum priority` and addition of start/end times
 */
void parse_complete_task_line(
    char buffer[MAX_STRING_LENGTH],
    char task_name[MAX_TASK_LENGTH],
    char task_category[MAX_CATEGORY_LENGTH],
    int *start_time,
    int *finish_time
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }
    
    // Extract value 2 as string
    char *start_str = strtok(NULL, " ");
    if (start_str != NULL) {
        *start_time = atoi(start_str);
    }
    
    // Extract value 2 as string
    char *finish_str = strtok(NULL, " ");
    if (finish_str != NULL) {
        *finish_time = atoi(finish_str);
    }

    if (
        name_str == NULL ||
        category_str == NULL ||
        start_str == NULL ||
        finish_str == NULL
    ) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/**
 * Helper Function
 * You should not need to change this function.
 *
 * Given a raw string, will return the corresponding `enum priority`,
 * or INVALID_PRIORITY if the string doesn't correspond with the enums.
 *
 * Parameters:
 *     priority - string representing the corresponding `enum priority` value
 * Returns:
 *     enum priority
 */
enum priority string_to_priority(char priority[MAX_STRING_LENGTH]) {
    if (strcmp(priority, "low") == 0) {
        return LOW;
    } else if (strcmp(priority, "medium") == 0) {
        return MEDIUM;
    } else if (strcmp(priority, "high") == 0) {
        return HIGH;
    }

    return INVALID_PRIORITY;
}

/**
 * Helper Function
 * You should not need to change this function.
 *
 * Given an priority and a character array, fills the array with the
 * corresponding string version of the priority.
 *
 * Parameters:
 *     prio - the `enum priority` to convert from
 *     out  - the array to populate with the string version of `prio`.
 * Returns:
 *     Nothing
 */
void priority_to_string(enum priority prio, char out[MAX_STRING_LENGTH]) {
    if (prio == LOW) {
        strcpy(out, "LOW");
    } else if (prio == MEDIUM) {
        strcpy(out, "MEDIUM");
    } else if (prio == HIGH) {
        strcpy(out, "HIGH");
    } else {
        strcpy(out, "Provided priority was invalid");
    }
}

/*
 * Helper Function
 * You should not need to change this function.
 *
 * Given a raw string will remove and first newline it sees.
 * The newline character wil be replaced with a null terminator ('\0')
 *
 * Parameters:
 *     input - The string to remove the newline from
 *
 * Returns:
 *     Nothing
 */
void remove_newline(char input[MAX_STRING_LENGTH]) {
    // Find the newline or end of string
    int index = 0;
    while (input[index] != '\n' && input[index] != '\0') {
        index++;
    }
    // Goto the last position in the array and replace with '\0'
    // Note: will have no effect if already at null terminator
    input[index] = '\0';
}

/*
 * Helper Function
 * You likely do not need to change this function.
 *
 * Given a raw string, will remove any whitespace that appears at the start or
 * end of said string.
 *
 * Parameters:
 *     input - The string to trim whitespace from
 *
 * Returns:
 *     Nothing
 */
void trim_whitespace(char input[MAX_STRING_LENGTH]) {
    remove_newline(input);
    
    int lower;
    for (lower = 0; input[lower] == ' '; ++lower);
    
    int upper;
    for (upper = strlen(input) - 1; input[upper] == ' '; --upper);
    
    for (int base = lower; base <= upper; ++base) {
        input[base - lower] = input[base];
    }

    input[upper - lower + 1] = '\0';
}

/**
 * Helper Function
 * You SHOULD NOT change this function.
 *
 * Given a task, prints it out in the format specified in the assignment.
 *
 * Parameters:
 *     task_num - The position of the task within a todo list
 *     task     - The task in question to print
 *
 * Returns:
 *     Nothing
 */
void print_one_task(int task_num, struct task *task) {
    char prio_str[MAX_STRING_LENGTH];
    priority_to_string(task->priority, prio_str);

    printf(
        "  %02d. %-30.30s [ %s ] %s\n",
        task_num, task->task_name, task->category, prio_str
    );

    int i = 30;
    while (i < strlen(task->task_name)) {
        printf("      %.30s\n", task->task_name + i);
        i += 30;
    }
}

/**
 * Helper Function
 * You SHOULD NOT change this function.
 *
 * Given a completed task, prints it out in the format specified in the
 * assignment.
 *
 * Parameters:
 *     completed_task - The task in question to print
 *
 * Returns:
 *     Nothing
 */
void print_completed_task(struct completed_task *completed_task) {
    int start_hour = completed_task->start_time / 60;
    int start_minute = completed_task->start_time % 60;
    int finish_hour = completed_task->finish_time / 60;
    int finish_minute = completed_task->finish_time % 60;
    
    printf(
        "  %02d:%02d-%02d:%02d | %-30.30s [ %s ]\n",
        start_hour, start_minute, finish_hour, finish_minute,
        completed_task->task->task_name, completed_task->task->category
    );

    int i = 30;
    while (i < strlen(completed_task->task->task_name)) {
        printf("      %.30s\n", (completed_task->task->task_name + i));
        i += 30;
    }
}

/**
 * Compares two tasks by precedence of category -> priority -> name and returns
 * an integer referring to their relative ordering
 * 
 * Parameters:
 *     t1 - The first task to compare
 *     t2 - The second task to compare
 *
 * Returns:
 *     a negative integer if t1 belongs before t2
 *     a positive integer if t1 belongs after t2
 *     0 if the tasks are identical (This should never happen in your program)
 */
int task_compare(struct task *t1, struct task *t2) {
    int category_diff = strcmp(t1->category, t2->category);
    if (category_diff != 0) {
        return category_diff;
    }
    
    int priority_diff = t2->priority - t1->priority;
    if (priority_diff != 0) {
        return priority_diff;
    }
    
    return strcmp(t1->task_name, t2->task_name);
}
