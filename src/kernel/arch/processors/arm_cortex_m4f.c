

#include <core_arm_cortex_m4f.h>


//--------------------------------------------- Private headers ---------------------------------------------

//The default stack provider. Simply returns the provided stack;
stack_t *default_thread_stack_provider(stack_t *old_stack) { return old_stack;}


//Set the thread's stack pointer;
static inline void core_set_process_stack_pointer(const stack_ptr_t sp);

//Get the thread's stack pointer;
static inline stack_ptr_t core_get_process_stack_pointer();

//Stack the current thread context;
static inline void core_stack_thread_context();

//Unstack the current thread context;
static inline void core_unstack_thread_context();

//The preemption function;
void core_premtion();

//--------------------------------------------- Fields ---------------------------------------------

//The thread's stack;
stack_t *process_stack;

//The thread stack provider; Initialised to the default provider;
stack_t *(*process_stack_provider)(stack_t*) = &default_stack_provider;


//--------------------------------------------- Implementations ---------------------------------------------


/*
 * Notice :
 *
 *
 *  ARM Cortex M4 Stack Frame :
 *
 *      ----------- <- Pre IRQ
 *      XPSR
 *      PC
 *      LR
 *      R12
 *      R3
 *      R2
 *      R1
 *      R0--------- <- Post IRQ
 */

/*
 * core_set_thread_stack_pointer : creates a temporary variable to contain the casted stack pointer,
 *  and injects assembly inline to move the data of the temp into psp;
 */

static inline void core_set_thread_stack_pointer(const stack_ptr_t sp) {
    __asm__ __volatile__ ("msr psp, %0"::"r" (sp));\

}


/*
 * core_get_thread_stack_pointer : injects assembly code in order to move the data of psp into the
 *  provided variable -> sp must be an existing variable name;
 */

static inline stack_ptr_t core_get_thread_stack_pointer() {
    stack_ptr_t ptr = 0;
    __asm__ __volatile__ ("mrs %0, psp" : "=r" ((uint32_t) ptr):);
    return ptr;
}


/*
 * core_stack_thread_context : injects assembly code in order to stack memory that is not
 *  stacked automatically in the process stack;
 *
 *  It moves psp into r0, and then stacks r4-r7, general purposes registers, and finally
 *      stacks s16- s31, floating point unit memory;
 *
 *  You may notice that PSP remains unchanged after the stacking. This is done with purpose, in order to
 *      allow the processor to unstack properly even if core_unstack_context is called;
 */

static inline void core_stack_thread_context() {
    uint32_t scratch;
    __asm__ __volatile__ (\
        "mrs %0, psp \n\t"\
        "stmdb %0!, {r4 - r11}\n\t"\
        "vstmdb %0!, {s16 - s31}" : "=r" (scratch)\
    );
}


/*
 * core_stack_thread_context : injects assembly code in order to stack memory that are not
 *  stacked automatically in the process_t stack;
 *
 *  It moves psp into r0, and then unstacks r4-r7, general purposes memory, and finally
 *      unstacks s16- s31, floating point unit memory;
 *
 *  As core_stack_thread_context didn't alter PSP, this function doesn't either;
 */

static inline void core_unstack_thread_context() {
    uint32_t scratch;
    __asm__ __volatile__ (\
        "mrs %0, psp \n\t"\
        "ldmdb %0!, {r4 - r11} \n\t"\
        "vldmdb %0!, {s16 - s31} " : "=r" (scratch)\
    );
}


/*
 * core_init_stack : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process_t stack pointer, and stacks the thread functions pointers, and the PSR.
 *
 *  Then, it saves the thread index in R12 (next word);
 *
 *  Finally, it leaves space for empty stack frame and saves the stack pointer;
 */

static inline void core_init_stack(void (*function)(), void (*end_loop)(), stack_element_t seed) {
    uint32_t *__core_init_stack_sp__ = core_get_thread_stack_pointer();
    *(__core_init_stack_sp__ - 1) = 0x01000000;
    *(__core_init_stack_sp__ - 2) = (uint32_t) (function);
    *(__core_init_stack_sp__ - 3) = (uint32_t) (end_loop);
    *(__core_init_stack_sp__ - 4) = (uint32_t) (seed);
    __core_init_stack_sp__ -= 8;
    core_set_thread_stack_pointer(__core_init_stack_sp__);
}


/*
 * core_get_process : this function will set thread_id to the value sored in r12, where was stored the thread
 *  index in the core_init_stack macro;
 */

static inline stack_element_t core_get_process() {
    stack_element_t seed = 0;
    __asm__ __volatile__("mov %0, r12": "=r" (seed):);
    return seed;

}


/*
 * The context switcher :
 *
 *  - saves the current context;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

void core_pereemption() {

    //Save the context in the current process stack;
    core_stack_thread_context();
	
    //Save the current process_t's stack pointer, while the process_t hasn't been deleted;
    process_stack->stack_pointer = core_get_process_stack_pointer();
    
	//Provide the old stack and get a new one;
    process_stack = process_stack_provider(process_stack);

    //Update the process stack pointer;
    core_set_process_stack_pointer(_process_stack->stack_pointer);

    //Unstack the context from the previous process_t stack
    core_unstack_thread_context();

}


