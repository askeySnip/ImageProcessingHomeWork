#ifndef ALGOR_H
#define ALGOR_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QImage>


void arithAverFilter(cv::Mat& src, cv::Mat& dst, int win_size);
void geoAverFilter(cv::Mat& src, cv::Mat& dst, int win_size);
void selfAdaptFilter(cv::Mat& src, cv::Mat& dst, int kernal_size);
void addGaussNoise(cv::Mat& dst);
void addSaltNoise(cv::Mat& dst, int n);
cv::Mat QImageToMat(QImage image);
QImage MatToQImage(const cv::Mat& mat);

#endif // ALGOR_H
