// Sequential code for calculating polar distance between every point
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//  Radius size limit
#define R 100

double calculate_polar_distance(double r1, double a1, double r2, double a2) {
    // Convert angles from degrees to radians
    double a1_rad = a1 * M_PI / 180.0;
    double a2_rad = a2 * M_PI / 180.0;

    // Calculate Cartesian coordinates
    double x1 = r1 * cos(a1_rad);
    double y1 = r1 * sin(a1_rad);
    double x2 = r2 * cos(a2_rad);
    double y2 = r2 * sin(a2_rad);

    // Calculate Euclidean distance
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int main(void) {
    int N;  // Size of the array
    scanf("%d", &N);

    // Array of points
    double *points = (double *)malloc(N * sizeof(double));

    // Fill up the array with random points
    for (int i = 0; i < N; i+=2) {
        points[i] = (double)rand() / RAND_MAX * R; // Random radius between 0 and R
        points[i+1] = (double)rand() / RAND_MAX * 360; // Random angle between 0 and 360 degrees
    }

    // Print the points (for debugging purposes)
    // printf("Points (radius, angle):\n");
    // for (int i = 0; i < N; i+=2) {
    //     printf("Point %d: (%.2f, %.2f)\n", i/2, points[i], points[i+1]);
    // }

    // Calculate the polar distance between every pair of points
    double d = ((N-1)/2 * ((N-1)/2 + 1)) / 2;   // Number of distances calculated
    double *D = (double *)malloc(d * sizeof(double));

    int index = 0;
    for (int i = 0; i < N; i+=2) {
        double r1 = points[i], a1 = points[i+1];
        for (int j = i+2; j < N; j+=2) {
            D[index] = calculate_polar_distance(r1, a1, points[j], points[j+1]);
            index++;
        }
    }

    // Print the distances (for debugging purposes)
    // printf("Distances:\n");
    // for (int i = 0; i < d; i++) {
    //     printf("Distance %d: %.2f\n", i, D[i]);
    // }
}