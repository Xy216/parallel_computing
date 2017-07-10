#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <iostream>
#include <time.h>

using namespace std;
using namespace cv;

int main(){
        Mat srcImg;
        Mat dstImg;
        srcImg = imread("../flowers.jpg");
        dstImg = newImage(srcImg.size(), srcImg.type());


        struct timespec start, end;
        double elapsed;
        clock_gettime(CLOCK_MONOTONIC, &start); //开始计时

        //GPU计算卷积
        gpu::GpuMat dst, src;
        src.upload(srcImg);

        float data[] = {-1, -1, 0, -1, 0, 1, 0, 1, 1};
        Mat kernel = Mat(3, 3, CV_32F, &data);
        gpu::filter2D(src, dst, src.depth(), kernel);

        dst.download(dstImg);

        clock_gettime(CLOCK_MONOTONIC, &end); //结束计时
        elapsed = end.tv_sec - start.tv_sec;
        elapsed += (end.tv_nsec - start.tv_nsec) / 1.0E9;
        cout << "Time taken: " << elapsed << "seconds" << endl;

        imwrite("rilievo.jpg", dstImg);
}
