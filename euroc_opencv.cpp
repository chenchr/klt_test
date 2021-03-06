#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include "klt.h"

using namespace std;

class featureCpp{
public:
    featureCpp(float x, float y, size_t frame_num){
        _id = global_cnt++;
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


void featurelistToVec(KLT_FeatureList featurelist, vector<cv::Point2f> &vec){
    vec.clear();
    for(int i=0; i<featurelist->nFeatures; ++i){
        if(featurelist->feature[i]->val >= 0)
            vec.push_back(cv::Point2f(featurelist->feature[i]->x, featurelist->feature[i]->y));
    }
}

void VecToFeaturelist(vector<cv::Point2f> &vec, vector<uchar> &status, KLT_FeatureList featurelist, vector<float> &err, float maxResidual){
    for(int i=0; i<status.size(); ++i){
        if(status[i] == 0){ // lost
            featurelist->feature[i]->val = -1;
        }else if(err[i] <= maxResidual){
            featurelist->feature[i]->val = KLT_TRACKED;
            featurelist->feature[i]->x = vec[i].x;
            featurelist->feature[i]->y = vec[i].y;
        }
    }
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
    tc->affineConsistencyCheck = 0;  /* set this to 2 to turn on affine consistency check */
    tc->smoothBeforeSelecting = FALSE;

    float maxResidual = 10.0;
    int templateSize = 19;
    int pyramidLevel = 3;

    cout << "euroc opencv: " << endl;
    cout << "maxResidual: " << maxResidual << endl;
    cout << "templateSize: " << templateSize << endl;
    cout << "pyramidLevel: " << pyramidLevel << endl;

    mapOfFeature featureLogging;

    cv::Mat img1 = cv::imread(allImage[beginIndex], 0);

    int ncols = img1.cols, nrows = img1.rows;
    KLTSelectGoodFeatures(tc, (unsigned char*)(img1.data), ncols, nrows, fl);
    // KLTStoreFeatureList(fl, ft, 0);
    vector<cv::Point2f> pre_point;
    featurelistToVec(fl, pre_point);
    update_mapOfFeature(featureLogging, fl, beginIndex);

    cv::Mat img2rgb;
    cv::cvtColor(img1, img2rgb, CV_GRAY2BGR);
    draw_feature(img2rgb, fl, featureLogging);
    cv::imshow("img2", img2rgb);
    cv::waitKey(500);
    for (i = beginIndex+1 ; i < 1570 ; i++)  {
        cv::Mat img2 = cv::imread(allImage[i], 0);
        vector<cv::Point2f> next_point;
        vector<uchar> status;
        vector<float> err;
        cv::calcOpticalFlowPyrLK(img1, img2, pre_point, next_point, status, err, cv::Size(templateSize,templateSize), pyramidLevel);
        // KLTTrackFeatures(tc, (unsigned char*)(img1.data), (unsigned char*)(img2.data), ncols, nrows, fl);
        VecToFeaturelist(next_point, status, fl, err, maxResidual);
        KLTReplaceLostFeatures(tc, (unsigned char*)(img2.data), ncols, nrows, fl);
        featurelistToVec(fl, pre_point);
        // KLTStoreFeatureList(fl, ft, i);
        update_mapOfFeature(featureLogging, fl, i);
        cv::Mat img2rgb;
        cv::cvtColor(img2, img2rgb, CV_GRAY2BGR);
        draw_feature(img2rgb, fl, featureLogging);
        cv::imshow("img2", img2rgb);
        cv::waitKey(1);
        img1 = img2;
    }
    // size_t length = featureLogging._allFeature[5]._location.size();
    // cout << "sdfdsf: " << featureLogging._allFeature[5]._location[0].second << endl;
    // cout << "location of feature id 5: " << endl;
    // for(int i=0; i<length; ++i)
    //     cout << featureLogging._allFeature[5]._location[i].first << " " << featureLogging._allFeature[5]._location[i].second << endl;
    // KLTWriteFeatureTable(ft, "features.txt", "%5.1f");
    // KLTWriteFeatureTable(ft, "features.ft", NULL);
    running_tracking_stocastic(featureLogging);
    KLTFreeFeatureTable(ft);
    KLTFreeFeatureList(fl);
    KLTFreeTrackingContext(tc);
    return 0;
}