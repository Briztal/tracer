//
// Created by root on 9/11/18.
//



#include <stdbool.h>
#include <kernel/log.h>
#include <kernel/fs/dfs.h>
#include <kernel/mod/auto_mod.h>

static void check_init() {


	kernel_log_("Listing file tree : ");

	dfs_list();

}

static bool check_exit() {

}

KERNEL_EMBED_USER_MODULE(timer_check, &check_init, &check_exit);