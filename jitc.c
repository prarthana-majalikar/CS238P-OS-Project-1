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
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    } else if (pid == 0) {

        char *args[7];                  
        args[0] = "gcc";                
        args[1] = "-shared";           
        args[2] = "-fPIC";              
        args[3] = "-o";                 
        args[4] = (char *)output;       
        args[5] = (char *)input;        
        args[6] = NULL;  
        
        /*char *args[] = {"gcc", "-shared", "-fPIC", "-o", (char *) output, (char *) input, NULL}; */
        execv("/usr/bin/gcc", args); 

        TRACE("Failed to compile C file in Child process");
        exit(1);
    } else {
        
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child process executed successfully, exit status: %d\n", WEXITSTATUS(status));
        } else {
            fprintf(stderr, "Child process failed.\n");
        }
    }
    return 0;
}

struct jitc *jitc_open(const char *pathname) {
    struct jitc *jitc = malloc(sizeof(struct jitc));
    if (!jitc) {
        TRACE("Memory allocation failed");
		return NULL;
    }

    jitc->handle = dlopen(pathname, RTLD_LAZY);
    if (!jitc->handle) {
        TRACE("Failed to load shared object");
        FREE(jitc);
        exit(1);
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
    long (*func)(void);  
    const char *error; 
    if (!jitc || !jitc->handle) {
        TRACE("Invalid JITC handle");
        return 0;
    }

    dlerror();

    func = (long (*)(void))dlsym(jitc->handle, symbol);
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
