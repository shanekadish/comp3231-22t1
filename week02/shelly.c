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

        pid_t pid = fork();
        if (pid == 0) {
            // child
            execvp(args[0], args);
            printf("should not be reachable\n");
        } else {
            // parent
            int child_termination_status;
            waitpid(pid, &child_termination_status, 0);
            printf("child terminated, back in the parent\n");
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
