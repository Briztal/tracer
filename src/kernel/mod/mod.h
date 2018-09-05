//
// Created by root on 9/4/18.
//

#ifndef TRACER_MOD_H
#define TRACER_MOD_H

//Add a module to the kernel;
void mod_add(const char *name, void (*init)(), void (*exit));

//Remove a module from the kernel;
void mod_remove(const char *name);

//Load all automatically loadable modules;
void mod_autoload();


#endif //TRACER_MOD_H
