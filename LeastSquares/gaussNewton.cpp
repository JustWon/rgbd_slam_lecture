#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main(int argc, char **argv) {

  // initialization
  double ar = 1.0, br = 2.0, cr = 1.0; 
  double ae = 2.0, be = -1.0, ce = 5.0;
  int N = 100;                         
  double w_sigma = 1.0;                
  double inv_sigma = 1.0 / w_sigma;
  cv::RNG rng;                         

  // data
  vector<double> x_data, y_data;     
  for (int i = 0; i < N; i++) {
    double x = i / 100.0;
    x_data.push_back(x);
    y_data.push_back(exp(ar * x * x + br * x + cr) + rng.gaussian(w_sigma * w_sigma));
  }

  int iterations = 100;  
  double cost = 0, lastCost = 0; 

  // least square
  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
  for (int iter = 0; iter < iterations; iter++) {

    Matrix3d H = Matrix3d::Zero(); 
    Vector3d b = Vector3d::Zero(); 
    cost = 0;

    for (int i = 0; i < N; i++) {
      double xi = x_data[i], yi = y_data[i]; 
      double error = yi - exp(ae * xi * xi + be * xi + ce);
      Vector3d J; 
      J[0] = -xi * xi * exp(ae * xi * xi + be * xi + ce);
      J[1] = -xi * exp(ae * xi * xi + be * xi + ce); 
      J[2] = -exp(ae * xi * xi + be * xi + ce); 

      H += inv_sigma * inv_sigma * J * J.transpose();
      b += -inv_sigma * inv_sigma * error * J;

      cost += error * error;
    }

    Vector3d dx = H.ldlt().solve(b);
    if (isnan(dx[0])) {
      cout << "result is nan!" << endl;
      break;
    }

    if (iter > 0 && cost >= lastCost) {
      cout << "cost: " << cost << ">= last cost: " << lastCost << ", break." << endl;
      break;
    }

    ae += dx[0];
    be += dx[1];
    ce += dx[2];

    lastCost = cost;

    cout << "total cost: " << cost << ", \t\tupdate: " << dx.transpose() <<
         "\t\testimated params: " << ae << "," << be << "," << ce << endl;
  }

  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  cout << "solve time cost = " << time_used.count() << " seconds. " << endl;

  cout << "estimated abc = " << ae << ", " << be << ", " << ce << endl;
  return 0;
}
