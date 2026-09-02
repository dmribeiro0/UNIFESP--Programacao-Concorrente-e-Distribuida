// Concurrent code for calculating polar distance between every point
// Each thread computes its own private results array; main() merges them
// after pthread_join, so there is zero shared mutable state between threads.

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define R 100  // Radius size limit

double calculate_polar_distance(double r1, double a1, double r2, double a2) {
    double a1_rad = a1 * M_PI / 180.0;
    double a2_rad = a2 * M_PI / 180.0;
    double x1 = r1 * cos(a1_rad), y1 = r1 * sin(a1_rad);
    double x2 = r2 * cos(a2_rad), y2 = r2 * sin(a2_rad);
    double dx = x2 - x1, dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

// What each thread receives
typedef struct {
    double *points;   // shared, read-only
    int P;             // total number of points
    int p_start;       // first point index this thread owns (inclusive)
    int p_end;         // last point index this thread owns (exclusive)
} thread_arg_t;

// What each thread hands back via pthread_exit / return
typedef struct {
    double *D;
    long count;
} thread_result_t;

void *ThreadProcess(void *arg) {
    thread_arg_t *ta = (thread_arg_t *)arg;

    // How many distances this thread will produce
    long count = 0;
    for (int p = ta->p_start; p < ta->p_end; p++) {
        count += ta->P - 1 - p;
    }

    thread_result_t *res = malloc(sizeof(thread_result_t));
    if (!res) {
        fprintf(stderr, "malloc failed in thread\n");
        pthread_exit(NULL);
    }

    res->count = count;
    res->D = (count > 0) ? malloc(count * sizeof(double)) : NULL;
    if (count > 0 && !res->D) {
        fprintf(stderr, "malloc failed in thread\n");
        free(res);
        pthread_exit(NULL);
    }

    long idx = 0;
    for (int p = ta->p_start; p < ta->p_end; p++) {
        int i = 2 * p;
        double r1 = ta->points[i], a1 = ta->points[i + 1];
        for (int q = p + 1; q < ta->P; q++) {
            int j = 2 * q;
            res->D[idx++] = calculate_polar_distance(r1, a1, ta->points[j], ta->points[j + 1]);
        }
    }

    return res;  // equivalent to pthread_exit(res);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }
    int T = atoi(argv[1]);
    if (T <= 0) {
        fprintf(stderr, "num_threads must be positive\n");
        return 1;
    }

    int N;  // total number of doubles (2 per point)
    if (scanf("%d", &N) != 1 || N <= 0 || N % 2 != 0) {
        fprintf(stderr, "N must be a positive even integer\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    double *points = malloc(N * sizeof(double));
    if (!points) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    for (int i = 0; i < N; i += 2) {
        points[i]     = (double)rand() / RAND_MAX * R;   // radius
        points[i + 1] = (double)rand() / RAND_MAX * 360; // angle
    }

    // printf("Points (radius, angle):\n");
    // for (int i = 0; i < N; i += 2) {
    //     printf("Point %d: (%.2f, %.2f)\n", i / 2, points[i], points[i + 1]);
    // }

    int P = N / 2;                    // number of points
    long d = (long)P * (P - 1) / 2;   // total number of distances

    if (T > P) {
        T = (P > 0) ? P : 1;          // don't spin up more threads than points
    }

    pthread_t *threads = malloc(T * sizeof(pthread_t));
    thread_arg_t *targs = malloc(T * sizeof(thread_arg_t));
    if (!threads || !targs) {
        fprintf(stderr, "malloc failed\n");
        free(points); free(threads); free(targs);
        return 1;
    }

    // Equal point-count split (simple; not equal-work -- see note below)
    for (int t = 0; t < T; t++) {
        int p_start = t * P / T;
        int p_end   = (t + 1) * P / T;
        targs[t] = (thread_arg_t){ points, P, p_start, p_end };
        pthread_create(&threads[t], NULL, ThreadProcess, &targs[t]);
    }

    // Merge results as each thread finishes
    double *D = malloc(d * sizeof(double));
    if (!D) {
        fprintf(stderr, "malloc failed\n");
        free(points); free(threads); free(targs);
        return 1;
    }

    long write_pos = 0;
    for (int t = 0; t < T; t++) {
        void *ret;
        pthread_join(threads[t], &ret);
        thread_result_t *res = (thread_result_t *)ret;
        if (res) {
            if (res->count > 0) {
                memcpy(D + write_pos, res->D, res->count * sizeof(double));
                write_pos += res->count;
            }
            free(res->D);
            free(res);
        }
    }

    // printf("Distances:\n");
    // for (long i = 0; i < d; i++) {
    //     printf("Distance %ld: %.2f\n", i, D[i]);
    // }

    free(points);
    free(D);
    free(threads);
    free(targs);
    return 0;
}