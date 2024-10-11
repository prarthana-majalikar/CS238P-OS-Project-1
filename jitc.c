/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"


struct jitc {
    void *handle;
};

int jitc_compile(const char *input, const char *output){
    pid_t pid = fork();

    if (pid < 0) {
        TRACE("Fork failed");
        return -1;

    } else if (pid == 0) {
        
        char *args[7];                  
        args[0] = "gcc";                
        args[1] = "-shared";           
        args[2] = "-fPIC";              
        args[3] = "-o";                 
        args[4] = (char *)output;       
        args[5] = (char *)input;        
        args[6] = NULL;  
        
        execv("/usr/bin/gcc", args); 

        EXIT("Failed to compile C file in Child process");

    } else {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            TRACE("Child process did not exit normally");
            return -1;
        } else if (WEXITSTATUS(status) != 0) {
            char error_message[100];
            snprintf(error_message, 100, "Child process failed, exit status: [%d]", WEXITSTATUS(status));
            TRACE(error_message);
            return -1;
        }
        else {
            return 0;
        }
    }
}

struct jitc *jitc_open(const char *pathname) {
    struct jitc *jitc = malloc(sizeof(struct jitc));
    if (!jitc) {
        TRACE("Memory allocation failed");
		return NULL;
    }

    jitc->handle = dlopen(pathname, RTLD_LAZY);
    if (!jitc->handle) {
        TRACE("Failed to load shared object[out.so]");
        FREE(jitc);
        return NULL;
    }

    return jitc;
}

void jitc_close(struct jitc *jitc) {
    if (jitc) {
        if (jitc->handle) {
            dlclose(jitc->handle);
        }
        FREE(jitc);
    }
}

long jitc_lookup(struct jitc *jitc, const char *symbol) {
    long (*func)(double (*)(double));  
    const char *error; 
    if (!jitc || !jitc->handle) {
        TRACE("Invalid JITC handle");
        return 0;
    }

    dlerror();

    func = (long (*)(double (*)(double)))dlsym(jitc->handle, symbol);
    error = dlerror();
    if (error) {
        fprintf(stderr, "Failed to find symbol: %s\n", error);
        return 0;
    }

    return (long)func;
}

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

/* research the above Needed API and design accordingly */
