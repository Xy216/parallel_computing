#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

using namespace cv;
using namespace std;

Mat srcImg;
Mat dstImg;
int filter[3][3] = {{-1, -1, 0}, {-1, 0, 1}, {0, 1, 1}}; //浮雕
struct paremeter {
        int sRow;
        int eRow;
};

void* cal_thread(void *arg) {
        int sRow = ((struct paremeter *)arg)->sRow;
        int eRow = ((struct paremeter *)arg)->eRow;
        int blue = 0, green = 0, red = 0;
        for (int i = sRow; i < eRow; i++) {
                for (int j = 0; j < srcImg.cols; j++) {
                        if (i == 0 || i == srcImg.rows - 1 || j == 0 || j == srcImg.cols - 1)
                                dstImg.at<Vec3b>(i, j) = srcImg.at<Vec3b>(i, j);
                        else {
                                blue = green = red = 0;
                                for (int p = -1; p <= 1; p++) {
                                        for (int q = -1; q <= 1; q++) {
                                                Vec3b color = srcImg.at<Vec3b>(i + p, j + q);
                                                blue += (color[0]) * filter[p + 1][q + 1];
                                                green += (color[1]) * filter[p + 1][q + 1];
                                                red += (color[2]) * filter[p + 1][q + 1];
                                        }
                                }
                                //浮雕颜色偏移
                                blue += 128;
                                green += 128;
                                red += 128;
                                blue = blue < 0 ? 0 : blue;
                                blue = blue > 255 ? 255 : blue;
                                green = green < 0 ? 0 : green;
                                green = green > 255 ? 255 : green;
                                red = red < 0 ? 0 : red;
                                red = red > 255 ? 255 : red;
                                dstImg.at<Vec3b>(i, j)[0] = blue;
                                dstImg.at<Vec3b>(i, j)[1] = green;
                                dstImg.at<Vec3b>(i, j)[2] = red;
                        }
                }
        }
};

int main(int argc, char **argv) {
        srcImg = imread("../flowers.jpg");
        dstImg = Mat(srcImg.rows, srcImg.cols, CV_8UC3);
        const int threads = 4;
        pthread_t pid[threads];
        int sRow, eRow;
        struct paremeter pare[threads];

        struct timespec start, end;
        double elapsed;
        clock_gettime(CLOCK_MONOTONIC, &start);  //开始计时

        for(int i=0; i<threads; i++) {
                sRow = i*(srcImg.rows/threads);
                eRow = (i+1)*(srcImg.rows/threads);
                pare[i].sRow = sRow;
                pare[i].eRow = eRow;
                pthread_create(&pid[i],NULL,cal_thread,&pare[i]);
        }
        for(int i=0; i<threads; i++) {
                pthread_join(pid[i],NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &end); //结束计时
        elapsed = end.tv_sec - start.tv_sec;
        elapsed += (end.tv_nsec - start.tv_nsec) / 1.0E9;
        cout << "Time taken: " << elapsed << "seconds" << endl;

        imwrite("rilievo.jpg", dstImg);
}
