#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int writeUP(const char * username, const char * password) {

    char fileInput[100];

    FILE *fp;
    fp=fopen("users.txt", "a");
    printf("file opened\n");
    strcpy(fileInput, username);
    printf("pre colon\n");
    strcat(fileInput, ":");
    printf("pre pw\n");
    strcat(fileInput, password);
    printf("pre nl\n");
    strcat(fileInput, "\n");
    fwrite(fileInput, 1, strlen(fileInput), fp);
    fclose(fp);

    return 0;
}

const char * getPassword(const char * username) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("users.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s\n", line);
        char *ch;
        char name[100];
        ch = strtok(line, ":");
        strcpy(name, ch);
        ch = strtok(NULL, ":");
        printf("password\n");
        printf("%s\n", ch); 
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}



int main() {

    writeUP("burke", "buga");
    writeUP("test", "pass");

    printf("pass:%s\n", getPassword("burke"));

}
