#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define N 100000

struct ThreadArgs {
    char *input;
    int start;
    int end;
};

typedef struct ThreadArgs thread_args;

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

void *compress_rle(void *args) {
    thread_args *t_args = (thread_args *)args;
    char *input = t_args->input;
    int start = t_args->start;
    int end = t_args->end;

    // res for joining results from all the threads
    char *res = malloc((end - start) * 2 * sizeof(char));

    int count, i = start, j = 0;
    while (i < end && input[i] != '\0') {
        res[j++] = input[i];
        count = 1;
        while (input[i] == input[i + 1]) {
            count++;
            i++;
        }
        j += sprintf(&res[j], "%d", count);
        i++;
    }
    res[j] = '\0';
    // printf("\n\nThread %d: %s\n\n", start / (N / 4), res);
    pthread_exit((void *)res);
}

int main() {
    char *input = criar_string(N);
    char *output = malloc((N * 2) * sizeof(char));
    output[0] = '\0';
    struct timeval start, end;

    int NUM_THREADS = 8;

    thread_args *t_args = malloc(NUM_THREADS * sizeof(struct ThreadArgs));
    pthread_t *t = malloc(NUM_THREADS * sizeof(pthread_t));

    for (int i = 0; i < NUM_THREADS; i++) {
        t_args[i].input = input;
        t_args[i].start = (N / NUM_THREADS) * i;
        t_args[i].end = (N / NUM_THREADS) * (i + 1);
    }

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&t[i], NULL, compress_rle, &t_args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        void *t_res = NULL;
        pthread_join(t[i], &t_res);
        strcat(output, (char *)t_res);
        free(t_res);
    }
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;

    double time_spent = seconds + (microseconds / 1000000.0);
    // printf("String comprimida: %s\n", output);
    printf("Tempo de execucao (paralelo): %f segundos\n", time_spent);
    
    free(t_args);
    free(t);
    free(input);
    free(output);

    return 0;
}