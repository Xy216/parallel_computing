#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000

int rand_r(unsigned int *seedp);

int samplesInsideCircle(int chunk) {
    // rand 为多线程不安全
    // 应使用 rand_r，同时种子为每个线程独有
    unsigned int seed = omp_get_thread_num() * 1000;

    int counter = 0;
    for (int i = 0; i < chunk; i++) {
        double r1 = (double)(rand_r(&seed)) / RAND_MAX;
	double r2= (double)(rand_r(&seed)) / RAND_MAX;
        double x = r1 * 2 - 1;
        double y = r2 * 2 - 1;
        if (x * x + y * y < 1) {
            counter++;
        }
    }
    return counter;
}

int main() {
    int threads;
    printf("Please input threads number: ");
    scanf(" %d", &threads);

    // 开始计时
    double beginParallel = omp_get_wtime();

    double counter = 0;
    const int chunk = N / threads;
    #pragma omp parallel for reduction(+:counter)
    for (int i = 0; i < threads; i++) {
        counter += samplesInsideCircle(chunk);
    }

    //结束计时
    double endParallel = omp_get_wtime();

    printf("PI is %f，Time take: %f seconds\n", 4 * counter / N, endParallel - beginParallel);
    return 0;
}
