#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_NUM_ARGS   8
#define MAX_WORD_SIZE 32 

int get_word(char *word_buf, int max_word_size);

int main(void) {
    while(true) {

        // display prompt
        printf("shelly> ");

        char current_word[MAX_WORD_SIZE];
        char *args[MAX_NUM_ARGS];
        
        // read arguments
        int nargs = 0;
        while(get_word(current_word, MAX_WORD_SIZE) != EOF) args[nargs++] = strdup(current_word);
        args[nargs++] = strdup(current_word);
        args[nargs] = 0;

        // print arguments
        printf("parent (%d): read args:", getpid());
        for (int i = 0; args[i]; i++) printf(" %s", args[i]);
        printf("\n");

        // fork
        pid_t pid = fork();
        if (pid == 0) {
            // executing in the child
            printf("child (%d): ", getpid());

            // stdout is line-buffered by default, which means it won't be flushed immediately unless there is a '\n'
            // exec overrides the current process memory image, including the I/O buffers
            // so we need to manually flush the stdout buffer before calling exec if we want our output to get written to the console
            // this SO post has some ok explanations: https://stackoverflow.com/questions/39536212/what-are-the-rules-of-automatic-stdout-buffer-flushing-in-c
            fflush(stdout);

            // exec
            if (execvp(args[0], args)) perror("shelly");

            // if exec succeeds, control will never reach here
            printf("something bad happened\n");
        } else {
            // executing in the parent
            int child_termination_status;
            
            waitpid(pid, &child_termination_status, 0);
            printf("parent (%d): finished waiting on child (%d)\n", getpid(), pid);
        }
    }
    return 0;
}

int get_word(char *word_buf, int max_word_size) {
    int current_length = 0;
    int character = getchar();
    word_buf[current_length++] = character;
    while (current_length < max_word_size && !isspace(character = getchar()))
        word_buf[current_length++] = character;
    word_buf[current_length++] = 0;
    return character == '\n' ? EOF : current_length;
}
