# Stritium : 

A free lightweight portable kernel for embedded systems, based on nostd;


## Project objectives

TODO subsections


Be standalone, and require as less dependencies as possible. 

The code is written exclusively in gcc flavour C89 (see section WHY C89 ?) except for inline assembly in 
specific parts of the code (see khal and arch_mods); 

dependencies are : 
- gnu environment for build process (mainly mkdir / rm);
- gnu-make for the build process;
- gcc suite (cc + ld + ar + objcopy + size) if possible and proprietary compiler if not, for the compilation 
    process;


Provide a kernel that : 

- can be ported easily on a large variety of platforms;
- can be easily configured;
- can load static and dynamic modules and applications, depending on the platform hardware capabilities;
- can offers an applicative environment that takes the best advantage of the platform hardware capabilities (MMU, MPU);
- supports multi-tasking and (in the future) multi-threading;
- offers shared resources (devices, code, memory etc...) management;



## Project organisation

Stritium is divided in the following parts :

- the kernel hardware abstraction layer (khal) : the kernel's hardware dependent code for 
    the target architecture. 
    This part is as small as possible, to allow the best portability;
    The kernel can't work without this code. It concerns mainly exceptions management 
    and cpu-oriented features (see include/khal for detailed declarations);
    
- the arch modules set (arch_mods) : 
    
- the kernel generic code (kernel) : contains the kernel's hardware independent code;
    this part contains the most of the kernel code, to allow the best portability;
    
    


Stritium is a kernel that aims to be deployed on various architectures (for instance, NXP's Kinetis K series), 
and to 



### Clone the project

To clone the project, since we are using submodules you will have to intializa your
local configuration files with `git submodule init` and `git submodule update` to fetch
all the data from the diffrent submodules.

```bash
git clone https://github.com/Essenceia/Zeldy/stable
git submodule init
git submodule update
```

### Update the project

If a new push occures on the tracked submodules branches and you are not up to date
anymore you can update it using :
```bash
git submodule update
```

### *Devlopper* Add new submodules

Adding a submodule is equivalent in a way as adding a reference to another git within a
project. You can do this with :
```bash
git submodule add <git_submodule_address>/<branch>
```

## License

This code is released under the GNU GPL v3 public license.
:smile:
