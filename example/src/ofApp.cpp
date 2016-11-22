#include "ofApp.h"



//! Reading test numbers from text file to float stl vector
/*!
 \param  fname Path to txt file with numbers, separated by white space
 \result stl vector of float numbers or empty stl vector if there was an error
 */

vector<float> ReadNumbersToVector(const string &fname) {
    vector<float> vec;
    ifstream in(fname);
    if (!in) {
        cerr << "Can't open file!" << endl;
        return vec;
    }
    double curr_number;
    while (!in.eof()) {
        in >> curr_number;
        vec.push_back(curr_number);
    }
    in.close();
    return vec;
}

/*
int DoPCA(vector<float> &vec,
          const unsigned int &n,
          const unsigned int &m,
          const string &fname) {
    // Principal component analisys
    Pca *pca = new Pca();
    int init_result = pca->Calculate(vec, n, m);//, true, true, false);
    if (0 != init_result) {
        cerr << "There is an error during PCA calculation!" << endl;
        return -1;
    }
    vector<float> sd = pca->sd(),
    prop_of_var = pca->prop_of_var(),
    cum_prop = pca->cum_prop(),
    scores = pca->scores();
    vector<unsigned int> el_cols = pca->eliminated_columns();
    float         kaiser = pca->kaiser(),
    thresh95 = pca->thresh95();
    unsigned int
    ncols = pca->ncols(),
    nrows = pca->nrows();
    string
    method = pca->method();
    delete pca;
    // Save the result to text file
    ofstream outfile(fname);
    if (!outfile) {
        cerr << "Can't create output file!" << endl;
        return -1;
    }
    outfile << "Initial matrix: " << endl;
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < m; ++j) {
            outfile << setw(7) << vec[j + m*i];
        }
        outfile << endl;
    }
    if (0 != el_cols.size()) {
        outfile << "\nNumbers of eliminated columns (0 based):\n";
        copy(el_cols.begin(), el_cols.end(), std::ostream_iterator<unsigned int>(outfile, " "));
        outfile << "\n\nMatrix after the eliminating: " << endl;
        for (unsigned int i = 0; i < n; ++i) {
            for (unsigned int j = 0; j < m; ++j) {
                if ( find(el_cols.begin(), el_cols.end(), j) == el_cols.end() ) {
                    outfile << setw(7) << vec[j + m*i];
                }
            }
            outfile << endl;
        }
    }
    outfile << "\n\n" << method << " method was used\n";
    outfile << "\n\nStandard deviation:\n";
    copy(sd.begin(), sd.end(), std::ostream_iterator<float>(outfile, " "));
    outfile << "\n\nProportion of variance:\n";
    copy(prop_of_var.begin(), prop_of_var.end(), std::ostream_iterator<float>(outfile, " "));
    outfile << "\n\nCumulative proportion:\n";
    copy(cum_prop.begin(), cum_prop.end(), std::ostream_iterator<float>(outfile, " "));
    outfile << "\n\nKaiser criterion: " << kaiser;
    outfile << "\n\n95% threshold criterion: " << thresh95 << endl;
    
    outfile << "\n\nRotated data: " << endl;
    unsigned int row_lim = nrows,
    col_lim = ncols;
    if (scores.size() != nrows * ncols) {
        row_lim = (nrows < ncols)? nrows : ncols,
        col_lim = (ncols < nrows)? ncols : nrows;
    }
    for (unsigned int i = 0; i < row_lim; ++i) {
        for (unsigned int j = 0; j < col_lim; ++j) {
            outfile << setw(13) << scores[j + col_lim*i];
        }
        outfile << endl;
    }
    outfile.close();
    
    return 0;
}
*/


//--------------------------------------------------------------
void ofApp::setup(){
    
    
    
    vector<float> vec = ReadNumbersToVector(ofToDataPath("02-in.txt"));
    cout << ofToString(vec) << endl;
    cout << vec.size() << endl;
    
    ofxPCA *pca = new ofxPCA();
    
    
    vector<vector<float> > data;
    data.resize(12);
    for (int i=0; i<12; i++) {
        data[i].resize(5);
        for (int j=0; j<5; j++) {
            data[i][j] = vec[j + 5*i];
        }
    }
    
    
    
    int init_result = pca->calculate(data, 3);//, true, true, false);
    
    
    vector<float> x = {1.0, 0.0, 1.0, 0.0, 0.0};
    vector<float> xt = pca->transform(x);
    cout << ofToString(xt) << endl;
    
    
/*
    vector<ofVec3f> verts;
    for (int i=0; i<100; i++) {
        verts.push_back(ofVec3f(ofRandom(1),ofRandom(1),ofRandom(1)));
    }
    
    ofxPCAResult result = pca.analyze(verts);
    // The ofxPCAResult object gives you the eigenvectors and
    // eigenvalues sorted in descending order of the eigenvalues,
    // i.e. by the most principal component to the least:
    cout << "top eigenvalue: " << result.eigenvalues[0] << endl;
    ofVec3f ev1 = result.eigenvectors[0];
    cout << "top eigenvector: " <<  ev1.x << "m" << ev1.y << "," << ev1.z << endl;
 
 */
    
    //pca.test();
    
    
    /*
    long long curr_time;
    vector<float> vec;
    
    // 1 number
    cout << "::: 01-in.txt: 1 number :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("01-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 1, 1, "01-out-01.txt");
    cout << "\t 1x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 0, 1, "01-out-02.txt");
    cout << "\t 0x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 0, 0, "01-out-03.txt");
    cout << "\t 0x0, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 70, 9, "01-out-04.txt");
    cout << "\t 70x9, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 60 numbers
    cout << endl << "::: 02-in.txt: 60 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("02-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 12, 5, "02-out-01.txt");
    cout << "\t 12x5, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 5, 12, "02-out-02.txt");
    cout << "\t 5x12, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 10, 6, "02-out-03.txt");
    cout << "\t 10x6, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 60, 1, "02-out-04.txt");
    cout << "\t 60x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 24 numbers
    cout << endl <<  "::: 03-in.txt: 24 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("03-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 12, 2, "03-out-01.txt");
    cout << "\t 12x2, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 2, 12, "03-out-02.txt");
    cout << "\t 2x12, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 8, 3, "03-out-03.txt");
    cout << "\t 8x3, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 3, 8, "03-out-04.txt");
    cout << "\t 60x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 100 numbers
    cout << endl << "::: 04-in.txt: 100 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("04-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 10, 10, "04-out-01.txt");
    cout << "\t 10x10, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 50, 2, "04-out-02.txt");
    cout << "\t 50x2, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 4, 25, "04-out-03.txt");
    cout << "\t 4x25, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 25, 4, "04-out-04.txt");
    cout << "\t 25x4, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 100 numbers
    cout << endl << "::: 05-in.txt: 100 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("05-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 10, 10, "05-out-01.txt");
    cout << "\t 10x10, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 4, 25, "05-out-02.txt");
    cout << "\t 4x25, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 100, 1, "05-out-03.txt");
    cout << "\t 100x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 25, 4, "05-out-04.txt");
    cout << "\t 25x4, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 1000 numbers, uniform distribution
    cout << endl << "::: 06-in.txt: 10^3 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("06-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 100, 10, "06-out-01.txt");
    cout << "\t 100x10, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 10, 100, "06-out-02.txt");
    cout << "\t 10x100, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 1000, 1, "06-out-03.txt");
    cout << "\t 1000x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 250, 4, "06-out-04.txt");
    cout << "\t 250x4, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 10000 numbers
    cout << endl << "::: 07-in.txt: 10^4 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("07-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 1000, 10, "07-out-01.txt");
    cout << "\t 1000x10, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 10, 1000, "07-out-02.txt");
    cout << "\t 10x1000, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 250, 40, "07-out-03.txt");
    cout << "\t 250x40, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 125, 80, "07-out-04.txt");
    cout << "\t 125x80, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 100000 numbers
    cout << endl << "::: 08-in.txt: 10^5 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("08-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 1000, 100, "08-out-01.txt");
    cout << "\t 1000x100, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 10000, 10, "08-out-02.txt");
    cout << "\t 10000x10, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 800, 125, "08-out-03.txt");
    cout << "\t 800x125, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 100, 1000, "08-out-04.txt");
    cout << "\t 100x1000, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 26 numbers
    cout << endl << "::: 09-in.txt: 26 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("09-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 13, 2, "09-out-01.txt");
    cout << "\t 13x2, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 1, 26, "09-out-02.txt");
    cout << "\t 1x26, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 26, 1, "09-out-03.txt");
    cout << "\t 26x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 2, 13, "09-out-04.txt");
    cout << "\t 2x13, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    // 8 numbers
    cout << endl << "::: 10-in.txt: 8 numbers :::" << endl;
    vec = ReadNumbersToVector(ofToDataPath("10-in.txt"));
    curr_time = GetTimeMs64();
    DoPCA(vec, 2, 4, "10-out-01.txt");
    cout << "\t 2x4, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 8, 1, "10-out-02.txt");
    cout << "\t 8x1, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 4, 2, "10-out-03.txt");
    cout << "\t 4x2, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    curr_time = GetTimeMs64();
    DoPCA(vec, 1, 8, "10-out-04.txt");
    cout << "\t 1x8, time (ms): " << double( GetTimeMs64()- curr_time ) << endl;
    
    return 0;
    
 */
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
