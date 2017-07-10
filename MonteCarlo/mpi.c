#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100000000

double uniform_distribution(double rangeLow, double rangeHigh) {
    double r = (double) (rand()) / RAND_MAX;
    return r * (rangeHigh - rangeLow) + rangeLow;
}

int main() {
    MPI_Status status;

    // MPI 程序初始化，开始多进程
    MPI_Init(NULL, NULL);

    srand(time(NULL));

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获得进程总数 N
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 获得当前进程号 0 ~ N-1

    if (rank == 0) {
        // 主进程

        // 计时开始
        struct timespec start, finish;
        clock_gettime(CLOCK_MONOTONIC, &start);

        double counter = 0;
        for (int i = 1; i < size; i++) {
            // 从子进程取回计数值
            int samplesInsideCircle;
            MPI_Recv(&samplesInsideCircle, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
            counter += samplesInsideCircle;
        }

        // 计时结束
        clock_gettime(CLOCK_MONOTONIC, &finish);

        double elapsed = finish.tv_sec - start.tv_sec;
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        printf("PI is %f，Time taken: %f seconds\n", 4 * counter / N, elapsed);
    } else {
        const int chunk = N / (size - 1);
        int counter = 0;
        for (int i = 0; i < chunk; i++) {
            double x = uniform_distribution(-1, 1);
            double y = uniform_distribution(-1, 1);
            if (x * x + y * y < 1) {
                counter++;
            }
        }
        MPI_Send(&counter, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
