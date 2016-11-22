#include "ofxPCA.h"


using namespace Eigen;



int ofxPCA::calculate(vector<vector<float> > &data,
                      const int ncomponents,
                      const bool is_corr,
                      const bool is_center,
                      const bool is_scale) {
    _nrows = data.size();
    _ncols = data[0].size();
    _ncomponents = ncomponents;
    _is_corr = is_corr;
    _is_center = is_center;
    _is_scale = is_scale;
    
    _xXf.resize(_nrows, _ncols);
    for (unsigned int i = 0; i < _nrows; ++i) {
        for (unsigned int j = 0; j < _ncols; ++j) {
            _xXf(i, j) = data[i][j];
        }
    }
    // Mean and standard deviation for each column
    VectorXf mean_vector(_ncols);
    mean_vector = _xXf.colwise().mean();
    VectorXf sd_vector(_ncols);
    unsigned int zero_sd_num = 0;
    float denom = static_cast<float>((_nrows > 1)? _nrows - 1: 1);
    for (unsigned int i = 0; i < _ncols; ++i) {
        VectorXf curr_col  = VectorXf::Constant(_nrows, mean_vector(i)); // mean(x) for column x
        curr_col = _xXf.col(i) - curr_col; // x - mean(x)
        curr_col = curr_col.array().square(); // (x-mean(x))^2
        sd_vector(i) = sqrt((curr_col.sum())/denom);
        if (0 == sd_vector(i)) {
            zero_sd_num++;
        }
    }
    // If colums with sd == 0 are too many,
    // don't continue calculations
    cout << "MADE IT HERE" << endl;
    cout << " = " << _ncols << " " << zero_sd_num << endl;
    if (1 > _ncols-zero_sd_num) {
        cout << "return 1"<<endl;
        return -1;
    }
    // Delete columns where sd == 0
    MatrixXf tmp(_nrows, _ncols-zero_sd_num);
    VectorXf tmp_mean_vector(_ncols-zero_sd_num);
    unsigned int curr_col_num = 0;
    for (unsigned int i = 0; i < _ncols; ++i) {
        if (0 != sd_vector(i)) {
            tmp.col(curr_col_num) = _xXf.col(i);
            tmp_mean_vector(curr_col_num) = mean_vector(i);
            curr_col_num++;
        } else {
            _eliminated_columns.push_back(i);
        }
    }
    _ncols -= zero_sd_num;
    _xXf = tmp;
    mean_vector = tmp_mean_vector;
    tmp.resize(0, 0); tmp_mean_vector.resize(0);
    // Shift to zero
    if (true == _is_center) {
        for (unsigned int i = 0; i < _ncols; ++i) {
            _xXf.col(i) -= VectorXf::Constant(_nrows, mean_vector(i));
        }
    }
    // Scale to unit variance
    if ( (false == _is_corr) || (true == _is_scale)) {
        for (unsigned int i = 0; i < _ncols; ++i) {
            _xXf.col(i) /= sqrt(_xXf.col(i).array().square().sum()/denom);
        }
    }
    // When _nrows < _ncols then svd will be used.
    // If corr is true and _nrows > _ncols then will be used correlation matrix
    // (TODO): What about covariance?
    if ( (_nrows < _ncols) || (false == _is_corr)) { // Singular Value Decomposition is on
        _method = "svd";
        JacobiSVD<MatrixXf> svd(_xXf, ComputeThinV);
        VectorXf eigen_singular_values = svd.singularValues();
        VectorXf tmp_vec = eigen_singular_values.array().square();
        float tmp_sum = tmp_vec.sum();
        tmp_vec /= tmp_sum;
        // PC's standard deviation and
        // PC's proportion of variance
        _kaiser = 0;
        unsigned int lim = (_nrows < _ncols)? _nrows : _ncols;
        for (unsigned int i = 0; i < lim; ++i) {
            _sd.push_back(eigen_singular_values(i)/sqrt(denom));
            if (_sd[i] >= 1) {
                _kaiser = i + 1;
            }
            _prop_of_var.push_back(tmp_vec(i));
        }
        tmp_vec.resize(0);
        // PC's cumulative proportion
        _thresh95 = 1;
        _cum_prop.push_back(_prop_of_var[0]);
        for (unsigned int i = 1; i < _prop_of_var.size(); ++i) {
            _cum_prop.push_back(_cum_prop[i-1]+_prop_of_var[i]);
            if (_cum_prop[i] < 0.95) {
                _thresh95 = i+1;
            }
        }
        // Scores
        MatrixXf eigen_scores = _xXf * svd.matrixV();
        _scores.reserve(lim*lim);
        for (unsigned int i = 0; i < lim; ++i) {
            for (unsigned int j = 0; j < lim; ++j) {
                _scores.push_back(eigen_scores(i, j));
            }
        }
        eigen_scores.resize(0, 0);
    } else { // COR OR COV MATRICES ARE HERE
        _method = "cor";
        // Calculate covariance matrix
        MatrixXf eigen_cov; // = MatrixXf::Zero(_ncols, _ncols);
        VectorXf sds;
        // (TODO) Should be weighted cov matrix, even if is_center == false
        eigen_cov = (1.0 /(_nrows/*-1*/)) * _xXf.transpose() * _xXf;
        sds = eigen_cov.diagonal().array().sqrt();
        MatrixXf outer_sds = sds * sds.transpose();
        eigen_cov = eigen_cov.array() / outer_sds.array();
        outer_sds.resize(0, 0);
        // ?If data matrix is scaled, covariance matrix is equal to correlation matrix
        EigenSolver<MatrixXf> edc(eigen_cov);
        VectorXf eigen_eigenvalues = edc.eigenvalues().real();
        MatrixXf eigen_eigenvectors = edc.eigenvectors().real();
        // The eigenvalues and eigenvectors are not sorted in any particular order.
        // So, we should sort them
        typedef pair<float, int> eigen_pair;
        vector<eigen_pair> ep;
        for (unsigned int i = 0 ; i < _ncols; ++i) {
            ep.push_back(make_pair(eigen_eigenvalues(i), i));
        }
        sort(ep.begin(), ep.end()); // Ascending order by default
        // Sort them all in descending order
        MatrixXf eigen_eigenvectors_sorted = MatrixXf::Zero(eigen_eigenvectors.rows(), eigen_eigenvectors.cols());
        VectorXf eigen_eigenvalues_sorted = VectorXf::Zero(_ncols);
        int colnum = 0;
        int i = ep.size()-1;
        for (; i > -1; i--) {
            eigen_eigenvalues_sorted(colnum) = ep[i].first;
            eigen_eigenvectors_sorted.col(colnum++) += eigen_eigenvectors.col(ep[i].second);
        }
        // We don't need not sorted arrays anymore
        eigen_eigenvalues.resize(0);
        eigen_eigenvectors.resize(0, 0);
        _sd.clear(); _prop_of_var.clear(); _kaiser = 0;
        float tmp_sum = eigen_eigenvalues_sorted.sum();
        for (unsigned int i = 0; i < _ncols; ++i) {
            _sd.push_back(sqrt(eigen_eigenvalues_sorted(i)));
            if (_sd[i] >= 1) {
                _kaiser = i + 1;
            }
            _prop_of_var.push_back(eigen_eigenvalues_sorted(i)/tmp_sum);
        }
        // PC's cumulative proportion
        _cum_prop.clear(); _thresh95 = 1;
        _cum_prop.push_back(_prop_of_var[0]);
        for (unsigned int i = 1; i < _prop_of_var.size(); ++i) {
            _cum_prop.push_back(_cum_prop[i-1]+_prop_of_var[i]);
            if (_cum_prop[i] < 0.95) {
                _thresh95 = i+1;
            }
        }
        // Scores for PCA with correlation matrix
        // Scale before calculating new values
        for (unsigned int i = 0; i < _ncols; ++i) {
            _xXf.col(i) /= sds(i);
        }
        sds.resize(0);
        MatrixXf eigen_scores = _xXf * eigen_eigenvectors_sorted;
        _scores.clear();
        _scores.reserve(_ncols*_nrows);
        for (unsigned int i = 0; i < _nrows; ++i) {
            for (unsigned int j = 0; j < _ncols; ++j) {
                _scores.push_back(eigen_scores(i, j));
            }
        }
        eigen_scores.resize(0, 0);
        // make copy of truncated eigenvectors with ncomponents
        int nr = int(eigen_eigenvectors_sorted.rows());
        int nc = _ncomponents;
        eigen_eigenvectors_truncated = MatrixXf::Zero(nr, nc);
        // copy
        for (int r=0; r<nr; r++) {
            for (int c=0; c<nc; c++) {
                eigen_eigenvectors_truncated(r,c) = eigen_eigenvectors_sorted(r,c);
            }
        }
        cout << "calculate trunc " <<eigen_eigenvectors_truncated.rows() << " by " << eigen_eigenvectors_truncated.cols() << endl;
    }
    return 0;
}

vector<float> ofxPCA::transform(vector<float> &x) {
    MatrixXf raw_point = MatrixXf::Zero(1, x.size());
    for (int i=0; i<x.size(); i++) {
        raw_point(0, i) = x[i];
    }
    cout << "grab trunc " <<eigen_eigenvectors_truncated.rows() << " by " << eigen_eigenvectors_truncated.cols() << endl;
    cout << "raw point is " << raw_point.rows() << " by " << raw_point.cols() << endl;
    MatrixXf transformed_point_ = raw_point * eigen_eigenvectors_truncated;
    vector<float> transformed_point;
    transformed_point.resize(_ncomponents);
    for (int i=0; i<_ncomponents; i++) {
        transformed_point[i] = transformed_point_(0, i);
    }
    return transformed_point;
}

vector<float> ofxPCA::sd(void) { return _sd; };
vector<float> ofxPCA::prop_of_var(void) {return _prop_of_var; };
vector<float> ofxPCA::cum_prop(void) { return _cum_prop; };
vector<float> ofxPCA::scores(void) { return _scores; };
vector<unsigned int> ofxPCA::eliminated_columns(void) { return _eliminated_columns; }
string ofxPCA::method(void) { return _method; }
unsigned int ofxPCA::kaiser(void) { return _kaiser; };
unsigned int ofxPCA::thresh95(void) { return _thresh95; };
unsigned int ofxPCA::ncols(void) { return _ncols; }
unsigned int ofxPCA::nrows(void) { return _nrows; }
unsigned int ofxPCA::ncomponents(void) { return _ncomponents; }
bool ofxPCA::is_scale(void) {  return _is_scale; }
bool ofxPCA::is_center(void) { return _is_center; }
ofxPCA::ofxPCA(void) {
    _nrows      = 0;
    _ncols      = 0;
    // Variables will be scaled by default
    _is_center  = true;
    _is_scale   = true;  
    // By default will be used singular value decomposition
    _method   = "svd";
    _is_corr  = false;
    
    _kaiser   = 0;
    _thresh95 = 1;
}
ofxPCA::~ofxPCA(void) {
    _xXf.resize(0, 0);
    _x.clear();
}
