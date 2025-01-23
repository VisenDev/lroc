#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h/nob.h"

#define BUILD_FOLDER "./.build/"
#define EXE_NAME BUILD_FOLDER "main"

bool build() {
    Nob_Cmd cmd = {0};
    if (!mkdir_if_not_exists(BUILD_FOLDER)) return false;

    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-fsanitize=address", "-fsanitize=undefined", "-Wpedantic", "-lncurses", "-DBACKEND_NCURSES");
    nob_cmd_append(&cmd, "-o", EXE_NAME, "main.c");
    if (!nob_cmd_run_sync(cmd)) return false;
    return true;
}


bool clean() {
    Nob_Cmd cmd = {0};
    Nob_Cmd_Redirect redirect = {0};
    nob_cmd_append(&cmd, "trash", "-f", BUILD_FOLDER);
    nob_cmd_run_sync_redirect(cmd, redirect);
    return true;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    /*get arg*/
    const char *command_name = "build";
    if (argc > 1) command_name = argv[1];
    nob_log(INFO, "selected command: %s", command_name);

    if (strcmp(command_name, "build") == 0) {
        build();
    } else if (strcmp(command_name, "clean") == 0) {
        clean();
    }

    {
        Nob_Cmd cmd = {0};
        Nob_Cmd_Redirect redirect = {0};
        nob_cmd_append(&cmd, "trash", "-f", "build.old");
        nob_cmd_run_async_redirect(cmd, redirect);
    }
    return 0;
}
