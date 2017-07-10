#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

using namespace cv;
using namespace std;

class image {
public:
Mat srcImg;
Mat dstImg;
//卷积核
// int filter[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}}; //锐化
// int filter[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}}; //边缘检测
int filter[3][3] = {{-1, -1, 0}, {-1, 0, 1}, {0, 1, 1}};   //浮雕1
// int filter[3][3] = {{2, 0, 0}, {0, -1, 0}, {0, 0, -1}}; //浮雕2

image(string str) {
        srcImg = imread(str);
        dstImg = Mat(srcImg.rows, srcImg.cols, CV_8UC3);
}
void cal_faltung(void) {
        struct timespec start, end;
        double elapsed;
        clock_gettime(CLOCK_MONOTONIC, &start);

        #pragma omp parallel for
        for (int i = 0; i < srcImg.rows; i++) {
                cal(i);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed = end.tv_sec - start.tv_sec;
        elapsed += (end.tv_nsec - start.tv_nsec) / 1.0E9;
        cout << "Time taken: " << elapsed << "seconds" << endl;
}
void cal(int i){
        int blue =0, green =0, red = 0;
        for (int j = 0; j < srcImg.cols; j++) {
                if (i == 0 || i == srcImg.rows - 1 || j == 0 || j == srcImg.cols - 1)
                        dstImg.at<Vec3b>(i, j) = srcImg.at<Vec3b>(i, j);
                else {
                        blue =0, green =0, red = 0;
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
void show(void) {
        imshow("img", dstImg);
        waitKey(0);
}
void save(void) {
        imwrite("rilievo.jpg", dstImg);
}
};

int main(int argc, char **argv) {
        string src("../flowers.jpg");
        image img(src);
        img.cal_faltung();
        img.save();
}
