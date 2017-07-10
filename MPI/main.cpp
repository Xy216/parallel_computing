#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

using namespace cv;
using namespace std;


int main(int argc, char **argv) {
        Mat srcImg;
        srcImg = imread("../flowers.jpg");
        int filter[3][3] = {{-1, -1, 0}, {-1, 0, 1}, {0, 1, 1}};         //浮雕

        MPI_Status status;
        MPI_Init(NULL,NULL);
        int rank,size;
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);

        if(rank == 0) {
                Mat dstImg;
                dstImg = Mat(srcImg.rows, srcImg.cols, CV_8UC3);

                struct timespec start, end;
                double elapsed;
                clock_gettime(CLOCK_MONOTONIC, &start); //开始计时

                int sRow,eRow,rowLen;
                unsigned char *bgr;
                for(int i=1; i<size; i++) {
                        sRow = (i-1)*(srcImg.rows/(size-1));
                        eRow = i*(srcImg.rows/(size-1));
                        rowLen = srcImg.cols*3;
                        bgr = new unsigned char[(eRow-sRow)*rowLen];
                        MPI_Recv(bgr,(eRow-sRow)*rowLen,MPI_UNSIGNED_CHAR,i,i,MPI_COMM_WORLD,&status);
                        for(int j=0; j<(eRow-sRow)*rowLen; j++) {
                                dstImg.ptr<uchar>(sRow+j/(rowLen))[j%rowLen] = bgr[j];
                        }
                        delete bgr;
                }

                clock_gettime(CLOCK_MONOTONIC, &end); //结束计时
                elapsed = end.tv_sec - start.tv_sec;
                elapsed += (end.tv_nsec - start.tv_nsec) / 1.0E9;
                cout << "Time taken: " << elapsed << "seconds" << endl;

                imwrite("rilievo.jpg", dstImg);
        }
        else{
                int sRow = (rank-1)*(srcImg.rows/(size-1));
                int eRow = rank*(srcImg.rows/(size-1));
                int rowLen = srcImg.cols*3;
                int blue = 0, green = 0, red = 0;
                unsigned char *bgr;
                bgr = new unsigned char[(eRow-sRow)*rowLen];
                for (int i = sRow; i < eRow; i++) {
                        for (int j = 0; j < srcImg.cols; j++) {
                                if (i == 0 || i == srcImg.rows - 1 || j == 0 || j == srcImg.cols - 1) {
                                        bgr[(i-sRow)*rowLen+j*3] = srcImg.at<Vec3b>(i,j)[0];
                                        bgr[(i-sRow)*rowLen+j*3+1] = srcImg.at<Vec3b>(i,j)[1];
                                        bgr[(i-sRow)*rowLen+j*3+2] = srcImg.at<Vec3b>(i,j)[2];
                                }
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
                                        bgr[(i-sRow)*rowLen+j*3] = blue;
                                        bgr[(i-sRow)*rowLen+j*3+1] = green;
                                        bgr[(i-sRow)*rowLen+j*3+2] = red;
                                }
                        }
                }
                MPI_Send(bgr,(eRow-sRow)*rowLen,MPI_UNSIGNED_CHAR,0,rank,MPI_COMM_WORLD);
                delete bgr;
        }
        MPI_Finalize();
}
