#include "common/common.h"
#include "vm/vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void repl() {
    char line_buf[1024];

    while (true) {
        printf("clox >> ");

        if (!fgets(line_buf, sizeof(line_buf), stdin)) {
            printf("\n");
            break;
        }

        if (line_buf[0] == 'c' && line_buf[1] == 'l') { exit(0); }

        interpret(line_buf);
    }
}

static char *read_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file '%s'.\n", path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);

    char *buffer = malloc(len + 1);
    if (buffer == NULL) { fprintf(stderr, "Not enough memory to read file '%s'.\n", path); }

    rewind(file);

    size_t len_read = fread(buffer, sizeof(char), len, file);
    if (len_read < len) { fprintf(stderr, "Unable to read file '%s'.", path); }
    fclose(file);

    buffer[len_read] = '\0';
    return buffer;
}

static void run_file(const char *path) {
    char *buffer = read_file(path);
    interpret_result res = interpret(buffer);
    free(buffer);

    if (res == INTERPRET_COMPILE_ERROR)
        exit(65);
    else if (res == INTERPRET_RUNTIME_ERROR)
        exit(70);
}

int main(int argc, const char **argv) {
    init_vm();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Path not specified! Usage: clox [path]");
    }

    free_vm();

    return 0;
}
