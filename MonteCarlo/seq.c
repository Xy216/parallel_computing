#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 100000000

double uniform_distribution(double rangeLow, double rangeHigh) {
        double r = (double)(rand()) / RAND_MAX;
        return r * (rangeHigh - rangeLow) + rangeLow;
}

int main() {
        srand(time(NULL));

        // 开始计时
        struct timespec start, finish;
        clock_gettime(CLOCK_MONOTONIC, &start);

        double counter = 0;
        for (int i = 0; i < N; i++) {
                double x = uniform_distribution(-1, 1);
                double y = uniform_distribution(-1, 1);
                if (x * x + y * y < 1) {
                        counter++;
                }
        }

        //结束计时
        clock_gettime(CLOCK_MONOTONIC, &finish);
        double elapsed = finish.tv_sec - start.tv_sec;
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        printf("PI is %f，Time taken: %f seconds\n", 4 * counter / N, elapsed);
        return 0;
}
