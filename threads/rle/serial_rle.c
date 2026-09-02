#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 100000

char *criar_string(int n) {
    char *str = malloc((n + 1) * sizeof(char));

    if (str == NULL) {
        return NULL;
    }

    char caracteres[] = "abcdefghijklmnopqrstuvwxyz";
    int num_caracteres = strlen(caracteres);
    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        int key = rand() % num_caracteres;
        str[i] = caracteres[key];
    }

    str[n] = '\0';

    return str;
}

void compress_rle(const char* input, char* output) {
    int count, i = 0, j = 0;
    while (input[i] != '\0') {
        output[j++] = input[i];
        count = 1;
        while (input[i] == input[i + 1]) {
            count++;
            i++;
        }
        j += sprintf(&output[j], "%d", count);
        i++;
    }
    output[j] = '\0';
}

int main() {
    char *input = criar_string(N);
    char output[N*2]; // Allocate enough space for the worst-case scenario

    clock_t start = clock();
    compress_rle(input, output);
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    // printf("String comprimida: %s\n", output);
    printf("Tempo de execucao (sequencial): %f segundos\n", time_spent);

    return 0;
}