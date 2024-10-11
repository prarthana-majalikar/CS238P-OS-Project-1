JIT Compiled Expression Evaluator (October 11, 2024)

Overview:
Designed the jitc.c file, which includes the following methods:

- jitc_compile : Forks a new process to compile a C file into a shared object using GCC, handles errors in compilation, and logs failures if the child process doesn't execute successfully.
- jitc_open: Loads the compiled shared object using dlopen, and returns a handle for further operations.
- jitc_lookup : Retrieves the function symbol "evaluate" from the shared object using dlsym and handles errors if the symbol cannot be found.
- jitc_close : Closes the shared object handle using dlclose and frees allocated memory.


Implemented the sigmoid(double x) function that resides in the main program but is called from the generated C code to transform the final value of the expression (i.e., sigmoid(expression-value)). 
Passed the address of the sigmoid function(present in main) while calling the evaluate function through the function pointer returned by jitc_lookup. This allowed the dynamically loaded evaluate function to receive and use the sigmoid function address and call it at runtime.


Team members:

Prarthana Majalikar
Student ID: 33598574

Bhakti Chindhe
Student ID: 78411444

Shreya Hardas
Student ID: 90594208