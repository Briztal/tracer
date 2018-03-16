
#include "../../../Kernel/TaskScheduler/task_state_t.h"

#include "../../../DataStructures/Trees/Tree.h"

typedef task_state_t(*gcode_task)();

typedef Tree<char, gcode_task> GCodeTree;