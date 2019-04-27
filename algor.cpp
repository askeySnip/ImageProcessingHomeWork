#include "algor.h"

#include <QDebug>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>

using namespace cv;

// assume the src & dst have same value inital
void arithAverFilter(Mat& src, Mat& dst, int win_size) {
  int start = win_size / 2;
  int t = win_size * win_size;
  if (src.channels() == 3) {    // color image
    int sum[3] = {};
    for (int i = start; i < src.rows - start; i++) {
      for (int j = start; j < src.cols - start; j++) {
        for (int m = i - start; m <= i + start; m++) {
          for (int n = j - start; n <= j + start; n++) {
            for(int k = 0; k < 3; k++) {
              sum[k] += src.at<Vec3b>(m, n)[k];
            }
          }
        }
        for (int k = 0; k < 3; k++) {
          dst.at<Vec3b>(i, j)[k] = sum[k] / t;
          sum[k] = 0;
        }
      }
    }
  }

  if (src.channels() == 1) {
    int sum = 0;
    for (int i = start; i < src.rows - start; i++) {
      for (int j= start; j < src.cols - start; j++) {
        for (int m = i - start; m <= i + start; m++) {
          for (int n = j - start; n <= j + start; n++) {
            sum += src.at<uchar>(m, n);
          }
        }
        dst.at<uchar>(i, j) = sum / t;
        sum = 0;
      }
    }
  }
  // imshow("arithAverFilter", dst);
}

void geoAverFilter(Mat& src, Mat& dst, int win_size) {
  double p = 1.0 / (win_size * win_size);
  int start = win_size / 2;
  if (src.channels() == 1) {
    dst.convertTo(dst, CV_32FC1);
    double geo = 1.0;
    for (int i = start; i < src.rows - start; i++) {
      for (int j = start; j < src.cols - start; j++) {
        for (int ii = i - start; ii <= i + start; ii++) {
          for (int jj = j - start; jj <= j + start; jj++) {
            if(src.at<uchar>(ii, jj) != 0) geo *= src.at<uchar>(ii, jj);
          }
        }
        dst.at<float>(i, j) = pow(geo, p);
        geo = 1.0;
      }
    }
    dst.convertTo(dst, CV_8UC1);
  }
  if (src.channels() == 3) {
    dst.convertTo(dst, CV_32FC3);
    double geo[3] = {1.0, 1.0, 1.0};
    for (int i = start; i < src.rows - start; i++) {
      for (int j = start; j < src.cols - start; j++) {
        for (int ii = i - start; ii <= i + start; ii++) {
          for (int jj = j - start; jj <= j + start; jj++) {
            for (int k = 0; k < 3; k++) {
              if(src.at<Vec3b>(ii, jj)[k] != 0) geo[k] *= src.at<Vec3f>(ii, jj)[k];
            }
          }
        }
        for (int k = 0; k < 3; k++) {
          dst.at<Vec3f>(i, j)[k] = pow(geo[k], p);
          geo[k] = 1.0;
        }
      }
    }
    dst.convertTo(dst, CV_8UC3);
  }
}

uchar adaptProcess(Mat& dst, int r, int c, int ks, int max_size) {
  std::vector<uchar> pixels;
  for (int a = -ks / 2; a <= ks / 2; a++) {
    for (int b = -ks / 2; b <= ks / 2; b++) {
      pixels.push_back(dst.at<uchar>(r + a, c + b));
    }
  }

  std::sort(pixels.begin(), pixels.end());
  uchar Zmin = pixels[0];
  uchar Zmax = pixels[ks * ks - 1];
  uchar Zmed = pixels[ks * ks / 2];
  uchar Zxy = dst.at<uchar>(r, c);
  if (Zmed > Zmin && Zmed < Zmax) {
    if (Zxy > Zmin && Zxy < Zmax) return Zxy;
    else return Zmed;
  } else {
    ks += 2;
    if (ks <= max_size) {
      return adaptProcess(dst, r, c, ks, max_size);
    } else {
      return Zmed;
    }
  }
  return 0;
}

// min_size is set to 3 default.
void selfAdaptFilter(Mat& src, Mat& dst, int kernal_size) {
  CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8U);
  if (dst.empty()) {
    dst.create(src.rows, src.cols, CV_8UC1);
  }

  int start = kernal_size / 2;
  for (int i = start; i < src.rows - start; i++) {
    for (int j = start; j < src.cols - start; j++) {
      dst.at<uchar>(i, j) = adaptProcess(dst, i, j, 3, kernal_size);
    }
  }
}

double generateGaussNoise(double mu, double sigma) {
  const double epsilon = std::numeric_limits<double>::min();
  static double z0, z1;
  static bool flag = false;
  flag = !flag;

  if(!flag) return z1 * sigma + mu;

  double u1, u2;

  // srand(int(time(0)));
  do {
    u1 = rand() * (1.0 / RAND_MAX);
    u2 = rand() * (1.0 / RAND_MAX);
  } while (u1 <= epsilon);

  z0 = sqrt(-2.0 * log(u1)) * cos(2 * CV_PI * u2);
  z1 = sqrt(-2.0 * log(u1)) * sin(2 * CV_PI * u2);
  return z1 * sigma + mu;
}

void addGaussNoise(Mat& dst) {
  int channels = dst.channels();
  int nRows = dst.rows;
  int nCols = dst.cols * channels;

  if (dst.isContinuous()) {
    nCols *= nRows;
    nRows = 1;
  }

  for (int i = 0; i < nRows; i++) {
    for (int j = 0; j < nCols; j++) {
      int val = dst.ptr<uchar>(i)[j] + generateGaussNoise(2, 0.8) * 32;
      if (val < 0) val = 0;
      else if (val > 255) val = 255;
      dst.ptr<uchar>(i)[j] = (uchar)val;
    }
  }
}

void addSaltNoise(Mat& dst, int n) {
  for (int k = 0; k < n; k++) {
    int i = rand() % dst.rows;
    int j = rand() % dst.cols;

    if (dst.channels() == 1) {
      dst.at<uchar>(i, j) = 255;
    } else {
      dst.at<Vec3b>(i, j)[0] = 255;
      dst.at<Vec3b>(i, j)[1] = 255;
      dst.at<Vec3b>(i, j)[2] = 255;
    }
  }

  for (int k = 0; k < n; k++) {
    int i = rand() % dst.rows;
    int j = rand() % dst.cols;

    if (dst.channels() == 1) {
      dst.at<uchar>(i, j) = 0;
    } else {
      dst.at<Vec3b>(i, j)[0] = 0;
      dst.at<Vec3b>(i, j)[1] = 0;
      dst.at<Vec3b>(i, j)[2] = 0;
    }
  }
}

Mat QImageToMat(QImage image)
{
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}
