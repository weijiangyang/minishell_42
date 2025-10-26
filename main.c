#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void) {
    read_history("history.txt"); // charge un historique précédent

    char *input;
    while ((input = readline("cmd> "))) {
        if (*input) add_history(input);

        if (strcmp(input, "exit") == 0) break;

        printf("Commande : %s\n", input);
        free(input);
    }

    write_history("history.txt"); // sauvegarde à la fin
    return 0;
}
