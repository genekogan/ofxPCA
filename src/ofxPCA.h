#pragma once

#include "../libs/Eigen/include/Eigen/Core"
#include "../libs/Eigen/include/Eigen/Eigen"
#include "../libs/Eigen/include/Eigen/Dense"
#include "../libs/Eigen/include/Eigen/SVD"
#include "ofMain.h"





class ofxPCA {
private:
    vector<float>  _x;   // Initial matrix as vector filled by rows.
    Eigen::MatrixXf     _xXf; // Initial matrix as Eigen MatrixXf structure
    unsigned int  _nrows,     // Number of rows in matrix x.
    _ncols,     // Number of cols in matrix x.
    _ncomponents; // Number of eigenvectors (principal components) to keep
    bool  _is_center,         // Whether the variables should be shifted to be zero centered
    _is_scale,          // Whether the variables should be scaled to have unit variance
    _is_corr;           // PCA with correlation matrix, not covariance
    std::string
    _method;            // svd, cor, cov
    vector<unsigned int>
    _eliminated_columns;  // Numbers of eliminated columns
    vector<float>  _sd,  // Standard deviation of each component
    _prop_of_var,   // Proportion of variance
    _cum_prop,      // Cumulative proportion
    _scores;        // Rotated values
    unsigned int  _kaiser,    // Number of PC according Kaiser criterion
    _thresh95;  // Number of PC according 95% variance threshold
    
    
    Eigen::MatrixXf eigen_eigenvectors_truncated;
    
public:
    int calculate(vector<vector<float> > &data, const int ncomponents,
                  const bool is_corr = true, const bool is_center = true, const bool is_scale = true);
    vector<float> transform(vector<float> &x);

    unsigned int nrows(void);
    unsigned int ncols(void);
    unsigned int ncomponents(void);
    bool is_center(void);
    bool is_scale(void);
    string method(void);
    vector<unsigned int> eliminated_columns(void);
    vector<float> sd(void);
    vector<float> prop_of_var(void);
    vector<float> cum_prop(void);
    unsigned int kaiser(void);
    unsigned int thresh95(void);
    vector<float> scores(void);

    ofxPCA(void);
    ~ofxPCA(void);
};
