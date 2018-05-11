#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "klt.h"

using namespace std;

class featureCpp{
public:
    featureCpp(float x, float y, size_t frame_num){
        _id = global_cnt++;
        if(_id == 5)
            cout << "x: " << x << " y: " << y << endl;
        _begin_frame = frame_num;
        _location.push_back(make_pair(x, y));
    }
    static size_t global_cnt;
    size_t _id;
    size_t _begin_frame;
    vector<pair<float, float> > _location;
};

class mapOfFeature{
public:
    mapOfFeature(){
        _allFeature.clear();
        _mapFeature.clear();
    }
    void add_feature(featureCpp feature, int index_in_fl){
        _allFeature.push_back(feature);
        _mapFeature[index_in_fl] = _allFeature.size()-1;
    }
    void update_feature(float x, float y, int index_in_fl){
        _allFeature[_mapFeature[index_in_fl]]._location.push_back(make_pair(x, y));
    }
    vector<featureCpp> _allFeature;
    map<int, int> _mapFeature;
};

size_t featureCpp::global_cnt = 0;

void draw_feature(cv::Mat &img, KLT_FeatureList featurelist, mapOfFeature &mof){
    int x, y, length;
    for (int i = 0 ; i < featurelist->nFeatures ; i++)
    if (featurelist->feature[i]->val >= 0)  {
      x = (int) (featurelist->feature[i]->x + 0.5);
      y = (int) (featurelist->feature[i]->y + 0.5);
      length = mof._allFeature[mof._mapFeature[i]]._location.size();
      double r = 255 - length/25.0*255.0;
      double g = length/25.0*255.0;
      r = r > 0 ? r : 0;
      g = g < 255 ? g : 255;
      circle(img, cv::Point(x,y), 4, cv::Scalar(0,g,r), 2);
    }
}

void update_mapOfFeature(mapOfFeature& mof, KLT_FeatureList featurelist, size_t frame_index){
    for(int i=0; i<featurelist->nFeatures; ++i){
        if(featurelist->feature[i]->val > 0){ //new feature
            featureCpp temp(featurelist->feature[i]->x, featurelist->feature[i]->y, frame_index);
            mof.add_feature(temp, i);
        }else if(featurelist->feature[i]->val == 0){ //tracked feature
            mof.update_feature(featurelist->feature[i]->x, featurelist->feature[i]->y, i);
        }else{
            continue;
        }
    }
}

void running_tracking_stocastic(mapOfFeature &mof){
    cout << "num of feature process: " << mof._allFeature.size() << endl;
    double sumAll = 0, sum1 = 0, sum5 = 0, sum10 = 0, sum15 = 0, sum20 = 0, sumOver = 0;
    for(int i=0; i<mof._allFeature.size(); ++i){
        if(mof._allFeature[i]._location.size() <= 1)
            ++sum1;
        else if(mof._allFeature[i]._location.size() <= 5)
            ++sum5;
        else if(mof._allFeature[i]._location.size() <= 10)
            ++sum10;
        else if(mof._allFeature[i]._location.size() <= 15)
            ++sum15;
        else if(mof._allFeature[i]._location.size() <= 20)
            ++sum20;
        else
            ++sumOver;
        sumAll += mof._allFeature[i]._location.size();
    }
    cout << "average tracker length: " << sumAll/mof._allFeature.size() << endl;
    cout << "sum1: \t" << sum1 << "\t" << sum1/mof._allFeature.size() << endl
         << "sum5: \t" << sum5 << "\t" << sum5/mof._allFeature.size() << endl
         << "sum10: \t" << sum10 << "\t" << sum10/mof._allFeature.size() << endl
         << "sum15: \t" << sum15 << "\t" << sum15/mof._allFeature.size() << endl
         << "sum20: \t" << sum20 << "\t" << sum20/mof._allFeature.size() << endl
         << "sumOv: \t" << sumOver << "\t" << sumOver/mof._allFeature.size() << endl;
}

extern int KLT_verbose;
int main(){
    KLT_verbose = 0;
    string filePath = "/home/chenchr/project/MH_04_difficult/im_list.txt";
    int beginIndex = 500;
    vector<string> allImage;
    string temp;
	fstream in(filePath, ios::in);
	while(in >> temp){
		allImage.push_back(temp);
	}
	in.close();
    // for(int i=beginIndex; i<allImage.size(); ++i){
    //     cv::Mat frame = cv::imread(allImage[i], 0);
    //     cout << "channel: " << frame.channels() << endl;
    //     cout << "col: " << frame.cols << endl;
    //     cout << "row: " << frame.rows << endl;
    //     cv::imshow("dsf", frame);
    //     cv::waitKey(0);
    //     cout << "sdfdffds" << endl;
    // }
    // cout << "dsfdfdfsaf" << endl;

    KLT_TrackingContext tc;
    KLT_FeatureList fl;
    KLT_FeatureTable ft;
    int nFeatures = 150, nFrames = allImage.size();
    int i;

    tc = KLTCreateTrackingContext();
    fl = KLTCreateFeatureList(nFeatures);
    ft = KLTCreateFeatureTable(nFrames, nFeatures);
    tc->sequentialMode = TRUE;
    tc->writeInternalImages = FALSE;
    tc->affineConsistencyCheck = -1;  /* set this to 2 to turn on affine consistency check */
    tc->smoothBeforeSelecting = TRUE;
// KLT_BOOL sequentialMode;
// KLT_BOOL smoothBeforeSelecting;
// KLT_BOOL writeInternalImages;
// KLT_BOOL lighting_insensitive;
// int min_eigenvalue;
// float min_determinant;
// float min_displacement;
// int max_iterations;
// float max_residue;
// float grad_sigma;
// float smooth_sigma_fact;
// float pyramid_sigma_fact;
// float step_factor;
// int nSkippedPixels;
// int borderx;
// int bordery;
// int nPyramidLevels;
// int subsampling;
// int affine_window_width, affine_window_height;
// int affineConsistencyCheck;
// int affine_max_iterations;
// float affine_max_residue;
// float affine_min_displacement;
// float affine_max_displacement_differ;
// void *pyramid_last;
// void *pyramid_last_gradx;
// void *pyramid_last_grady;
// } *KLT_TrackingContext;
tc->nPyramidLevels = 4;
tc->subsampling = 2;
tc->window_height = 7;
tc->window_width = 7;
tc->lighting_insensitive = 1;
    cout << "system settings: " << endl 
         << "mindist: " << tc->mindist << endl
         << "window_width: " << tc->window_width << endl
         << "window_height: " << tc->window_height << endl
         << "sequentialMode: " << tc->sequentialMode << endl
         << "smoothBeforeSelecting: " << tc->smoothBeforeSelecting << endl
         << "lighting_insensitive: " << tc->lighting_insensitive << endl
         << "min_eigenvalue: " << tc->min_eigenvalue << endl
         << "min_displacement: " << tc->min_displacement << endl
         << "max_iterations: " << tc->max_iterations << endl
         << "max_residue: " << tc->max_residue << endl
         << "grad_sigma: " << tc->grad_sigma << endl
         << "smooth_sigma_fact: " << tc->smooth_sigma_fact << endl 
         << "pyramid_sigma_fact: " << tc->pyramid_sigma_fact << endl
         << "nPyramidLevels: " << tc->nPyramidLevels << endl
         << "subsampling: " << tc->subsampling << endl
         << "affine_window_width: " << tc->affine_window_width << endl
         << "affine_window_height: " << tc->affine_window_height << endl;

    mapOfFeature featureLogging;

    cv::Mat img1 = cv::imread(allImage[beginIndex], 0);

    int ncols = img1.cols, nrows = img1.rows;
    KLTSelectGoodFeatures(tc, (unsigned char*)(img1.data), ncols, nrows, fl);
    // KLTStoreFeatureList(fl, ft, 0);

    update_mapOfFeature(featureLogging, fl, beginIndex);

    cv::Mat img2rgb;
    cv::cvtColor(img1, img2rgb, CV_GRAY2BGR);
    draw_feature(img2rgb, fl, featureLogging);
    cv::imshow("img2", img2rgb);
    cv::waitKey(500);
    cout << "min eigen val: " << tc->min_eigenvalue << " mindist: " << tc->mindist << endl;
    for (i = beginIndex+1 ; i < 1570 ; i++)  {
        cv::Mat img2 = cv::imread(allImage[i], 0);
        
        KLTTrackFeatures(tc, (unsigned char*)(img1.data), (unsigned char*)(img2.data), ncols, nrows, fl);
        KLTReplaceLostFeatures(tc, (unsigned char*)(img2.data), ncols, nrows, fl);
        // KLTStoreFeatureList(fl, ft, i);
        update_mapOfFeature(featureLogging, fl, i);
        cv::Mat img2rgb;
        cv::cvtColor(img2, img2rgb, CV_GRAY2BGR);
        draw_feature(img2rgb, fl, featureLogging);
        cv::imshow("img2", img2rgb);
        cv::waitKey(1);
    }
    size_t length = featureLogging._allFeature[5]._location.size();
    cout << "sdfdsf: " << featureLogging._allFeature[5]._location[0].second << endl;
    cout << "location of feature id 5: " << endl;
    for(int i=0; i<length; ++i)
        cout << featureLogging._allFeature[5]._location[i].first << " " << featureLogging._allFeature[5]._location[i].second << endl;
    // KLTWriteFeatureTable(ft, "features.txt", "%5.1f");
    // KLTWriteFeatureTable(ft, "features.ft", NULL);
    running_tracking_stocastic(featureLogging);
    KLTFreeFeatureTable(ft);
    KLTFreeFeatureList(fl);
    KLTFreeTrackingContext(tc);
    return 0;
}