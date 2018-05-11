#include<iostream>
#include<fstream>
#include<vector>
#include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>


using namespace std;

int main(){
    string filePath = "/home/chenchr/project/MH_04_difficult/im_list.txt";
    int beginIndex = 0;
    vector<string> allImage;
    string temp;
	fstream in(filePath, ios::in);
	while(in >> temp){
		allImage.push_back(temp);
		std::cout << temp << std::endl;
	}
	in.close();
    // for(int i=beginIndex; i<allImage.size(); ++i){
    //     cv::Mat frame = cv::imread(allImage[i]);
    //     cv::imshow("dsf", frame);
    //     cv::waitKey(0);
    // }
    cout << "dsfdfdfsaf" << endl;
    return 0;
}