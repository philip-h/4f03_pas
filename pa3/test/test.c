#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *S;

void appendToS(char);

int main(int argc, const char *argv[])
{
    int len = 8;
    S = malloc(len * sizeof(char));
    S[0] = '\0';
    int i = strlen(S);
    appendToS('c');
    appendToS('d');
    appendToS('q');
    appendToS('f');
    appendToS('d');
    appendToS('s');
    appendToS('u');
    printf("%s\n", S);
    S[3] = '\0';
    printf("%s\n", S);

    return 0;
}

void appendToS(char c) 
{
    int l = strlen(S);
    S[l] = c;
    S[l+1] = '\0';
}
