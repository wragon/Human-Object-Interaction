#define OPENPOSE_FLAGS_DISABLE_POSE
#include <openpose/flags.hpp>
#include <openpose/headers.hpp>

#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

#include "direct.h"

typedef vector<vector<float>> vecJoints;
typedef struct {
	float x, y;
	float avgLength;
} keypoint;
typedef struct {
	vector<keypoint> keypoints;
	float avgLength;
} joints;

int main(int argc, char* argv[])
{
	FILE* finp = fopen(argv[1], "r");
	FILE* fout = fopen("out/result.txt", "w");
	if (finp == NULL) {
		printf("Error in file opening\n");
		exit(1);
	}
	else {
		op::Wrapper opWrapper{ op::ThreadManagerMode::Asynchronous };
		opWrapper.start();

		int cnt = 0, fid, gt, prediction;
		char cname[256], iname[256], dname[256], oname[256];
		float TH1 = 0.75, TH2 = 0.05, TH3 = 300;
		fscanf(finp, "%s\n", cname);
		FILE* cinp = fopen(cname, "r");

		while (!feof(finp)) {
			fscanf(finp, "%s %s\n", iname, dname);
			fscanf(cinp, "%d %d %d\n", &fid, &gt, &prediction);

			Mat img = imread(iname);
			printf("%s %s\n", iname, dname);

			const op::Matrix imageToProcess = OP_CV2OPCONSTMAT(img);
			auto datumProcessed = opWrapper.emplaceAndPop(imageToProcess);			
			vector<vecJoints> joints_set;
			joints_set.clear();
			if ((datumProcessed != nullptr) && (!datumProcessed->empty())) {
				const auto& poseKeypoints = datumProcessed->at(0)->poseKeypoints;

				for (auto person = 0; person < poseKeypoints.getSize(0); person++) {
					vecJoints joints;
					joints.clear();
					joints.resize(15);
					for (int k = 0; k < 15; k++) joints[k].resize(3);

					for (auto bodyPart = 0; bodyPart < 15; bodyPart++) {
						std::string valueToPrint;
						for (auto xyscore = 0; xyscore < poseKeypoints.getSize(2); xyscore++) {
							valueToPrint = std::to_string(poseKeypoints[{person, bodyPart, xyscore}]);
							joints[bodyPart][xyscore] = stof(valueToPrint.c_str());
						}
					}
					if (joints[1][2] > TH1) joints_set.push_back(joints);
				}
			}

			joints selJoints;
			if (joints_set.size() == 0) {
				printf("[error] no keypoint detected\n");
				exit(1);
			}
			else {
				char text[32];
				float maxSum = 0;
				int selPerson = 0;
				for (int p = 0; p < joints_set.size(); p++) {
					float sum = 0;
					if (joints_set[p][0][2] < TH2) { joints_set[p][0][0] = joints_set[p][1][0]; joints_set[p][0][1] = joints_set[p][1][1]; joints_set[p][0][2] = -1; }
					if (joints_set[p][2][2] < TH2) { joints_set[p][2][0] = joints_set[p][1][0]; joints_set[p][2][1] = joints_set[p][1][1]; joints_set[p][2][2] = -1; }
					if (joints_set[p][5][2] < TH2) { joints_set[p][5][0] = joints_set[p][1][0]; joints_set[p][5][1] = joints_set[p][1][1]; joints_set[p][5][2] = -1; }
					if (joints_set[p][8][2] < TH2) { joints_set[p][8][0] = joints_set[p][1][0]; joints_set[p][8][1] = joints_set[p][1][1]; joints_set[p][8][2] = -1; }
					if (joints_set[p][3][2] < TH2) { joints_set[p][3][0] = joints_set[p][2][0]; joints_set[p][3][1] = joints_set[p][2][1]; joints_set[p][3][2] = -1; }
					if (joints_set[p][4][2] < TH2) { joints_set[p][4][0] = joints_set[p][3][0]; joints_set[p][4][1] = joints_set[p][3][1]; joints_set[p][4][2] = -1; }
					if (joints_set[p][6][2] < TH2) { joints_set[p][6][0] = joints_set[p][5][0]; joints_set[p][6][1] = joints_set[p][5][1]; joints_set[p][6][2] = -1; }
					if (joints_set[p][7][2] < TH2) { joints_set[p][7][0] = joints_set[p][6][0]; joints_set[p][7][1] = joints_set[p][6][1]; joints_set[p][7][2] = -1; }
					if (joints_set[p][9][2] < TH2) { joints_set[p][9][0] = joints_set[p][8][0]; joints_set[p][9][1] = joints_set[p][8][1]; joints_set[p][9][2] = -1; }
					if (joints_set[p][10][2] < TH2) { joints_set[p][10][0] = joints_set[p][9][0]; joints_set[p][10][1] = joints_set[p][9][1]; joints_set[p][10][2] = -1; }
					if (joints_set[p][11][2] < TH2) { joints_set[p][11][0] = joints_set[p][10][0]; joints_set[p][11][1] = joints_set[p][10][1]; joints_set[p][11][2] = -1; }
					if (joints_set[p][12][2] < TH2) { joints_set[p][12][0] = joints_set[p][8][0]; joints_set[p][12][1] = joints_set[p][8][1]; joints_set[p][12][2] = -1; }
					if (joints_set[p][13][2] < TH2) { joints_set[p][13][0] = joints_set[p][12][0]; joints_set[p][13][1] = joints_set[p][12][1]; joints_set[p][13][2] = -1; }
					if (joints_set[p][14][2] < TH2) { joints_set[p][14][0] = joints_set[p][13][0]; joints_set[p][14][1] = joints_set[p][13][1]; joints_set[p][14][2] = -1; }
				}				
				selJoints.keypoints.clear();
				int found = 0;
				for (int i = 0; i < 15; i++) {
					if (int(joints_set[selPerson][i][2] > 0)) {
						found = found + 1;
						keypoint joint;
						joint.x = joints_set[selPerson][i][0];
						joint.y = joints_set[selPerson][i][1];
						selJoints.keypoints.push_back(joint);
					}
				}

				int sclText=2, sclLine=10;
				arrowedLine(img, Point(int(selJoints.keypoints[1].x), int(selJoints.keypoints[1].y)), Point(int(selJoints.keypoints[0].x), int(selJoints.keypoints[0].y)), Scalar(255, 0, 255), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[1].x), int(selJoints.keypoints[1].y)), Point(int(selJoints.keypoints[2].x), int(selJoints.keypoints[2].y)), Scalar(255, 0, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[1].x), int(selJoints.keypoints[1].y)), Point(int(selJoints.keypoints[5].x), int(selJoints.keypoints[5].y)), Scalar(0, 0, 255), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[1].x), int(selJoints.keypoints[1].y)), Point(int(selJoints.keypoints[8].x), int(selJoints.keypoints[8].y)), Scalar(0, 255, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[2].x), int(selJoints.keypoints[2].y)), Point(int(selJoints.keypoints[3].x), int(selJoints.keypoints[3].y)), Scalar(255, 64, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[3].x), int(selJoints.keypoints[3].y)), Point(int(selJoints.keypoints[4].x), int(selJoints.keypoints[4].y)), Scalar(255, 128, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[5].x), int(selJoints.keypoints[5].y)), Point(int(selJoints.keypoints[6].x), int(selJoints.keypoints[6].y)), Scalar(0, 64, 255), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[6].x), int(selJoints.keypoints[6].y)), Point(int(selJoints.keypoints[7].x), int(selJoints.keypoints[7].y)), Scalar(0, 128, 255), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[8].x), int(selJoints.keypoints[8].y)), Point(int(selJoints.keypoints[9].x), int(selJoints.keypoints[9].y)), Scalar(64, 255, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[9].x), int(selJoints.keypoints[9].y)), Point(int(selJoints.keypoints[10].x), int(selJoints.keypoints[10].y)), Scalar(128, 255, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[10].x), int(selJoints.keypoints[10].y)), Point(int(selJoints.keypoints[11].x), int(selJoints.keypoints[11].y)), Scalar(255, 255, 0), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[8].x), int(selJoints.keypoints[8].y)), Point(int(selJoints.keypoints[12].x), int(selJoints.keypoints[12].y)), Scalar(0, 255, 64), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[12].x), int(selJoints.keypoints[12].y)), Point(int(selJoints.keypoints[13].x), int(selJoints.keypoints[13].y)), Scalar(0, 255, 128), sclLine);
				arrowedLine(img, Point(int(selJoints.keypoints[13].x), int(selJoints.keypoints[13].y)), Point(int(selJoints.keypoints[14].x), int(selJoints.keypoints[14].y)), Scalar(0, 255, 255), sclLine);
				putText(img, "0", Point(int(selJoints.keypoints[0].x), int(selJoints.keypoints[0].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "1", Point(int(selJoints.keypoints[1].x), int(selJoints.keypoints[1].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "2", Point(int(selJoints.keypoints[2].x), int(selJoints.keypoints[2].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "3", Point(int(selJoints.keypoints[3].x), int(selJoints.keypoints[3].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "4", Point(int(selJoints.keypoints[4].x), int(selJoints.keypoints[4].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "5", Point(int(selJoints.keypoints[5].x), int(selJoints.keypoints[5].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "6", Point(int(selJoints.keypoints[6].x), int(selJoints.keypoints[6].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "7", Point(int(selJoints.keypoints[7].x), int(selJoints.keypoints[7].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "8", Point(int(selJoints.keypoints[8].x), int(selJoints.keypoints[8].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "9", Point(int(selJoints.keypoints[9].x), int(selJoints.keypoints[9].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "10", Point(int(selJoints.keypoints[10].x), int(selJoints.keypoints[10].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "11", Point(int(selJoints.keypoints[11].x), int(selJoints.keypoints[11].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "12", Point(int(selJoints.keypoints[12].x), int(selJoints.keypoints[12].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "13", Point(int(selJoints.keypoints[13].x), int(selJoints.keypoints[13].y)), 2, sclText, Scalar::all(255), 2);
				putText(img, "14", Point(int(selJoints.keypoints[14].x), int(selJoints.keypoints[14].y)), 2, sclText, Scalar::all(255), 2);
			}

			int cls;
			float x, y, w, h, conf, maxconf=0;
			float imgW = img.cols, imgH = img.rows;

			// human-object interaction
			FILE* dinp = fopen(dname, "r");
			while (!feof(dinp)) {
				fscanf(dinp, "%d %f %f %f %f %f\n", &cls, &x, &y, &w, &h, &conf);
				int x_=int(x*imgW), y_=int(y*imgH);
				if(cls==0) {
					//putText(img, "person", Point(int(x_), int(y_)), 2, 3, Scalar::all(255), 3);
					//line(img, Point(int(x_-10), int(y_)), Point(int(x_+10), int(y_)), Scalar(255, 255, 255), 5);
					//line(img, Point(int(x_), int(y_-10)), Point(int(x_), int(y_+10)), Scalar(255, 255, 255), 5);
				} else if (cls == 41) {					
					putText(img, "cup", Point(int(x_), int(y_)), 3, 3, Scalar(0, 255, 0), 3);
					line(img, Point(int(x_ - 10), int(y_)), Point(int(x_ + 10), int(y_)), Scalar(0, 255, 0), 5);
					line(img, Point(int(x_), int(y_ - 10)), Point(int(x_), int(y_ + 10)), Scalar(0, 255, 0), 5);
					if ( (prediction==6) || (prediction==18) ) {
						float handx = float(selJoints.keypoints[7].x);
						float handy = float(selJoints.keypoints[7].y);
						float edist = sqrt((handx - float(x_)) * (handx - float(x_)) + (handy - float(y_)) * (handy - float(y_)));
						if (edist < TH3) {	// detect HOI						
							if(conf>maxconf) { // non-maximum suppression
								maxconf = conf;
								circle(img, Point((handx + x_) / 2, (handy + y_) / 2), float(TH3) / 2, Scalar(0, 255, 255), 4);
								prediction = 15;								
							}
						}
					} else if ((prediction == 3) || (prediction == 11)) {
						float handx = float(selJoints.keypoints[7].x);
						float handy = float(selJoints.keypoints[7].y);
						float edist = sqrt((handx-float(x_))*(handx-float(x_)) + (handy-float(y_))*(handy-float(y_)));
						if (edist < TH3) {							
							if (conf>maxconf) {
								maxconf = conf;
								circle(img, Point((handx + x_) / 2, (handy + y_) / 2), float(TH3) / 2, Scalar(0, 255, 255), 4);
								if (prediction == 3) prediction = 16;
								if (prediction == 11) prediction = 17;
							}
						}
					} else if ((prediction == 4) || (prediction == 12)) {
						float handx = float(selJoints.keypoints[4].x);
						float handy = float(selJoints.keypoints[4].y);
						float cupx = float(x_);
						float cupy = float(y_);
						float edist = sqrt((handx - float(x_)) * (handx - float(x_)) + (handy - float(y_)) * (handy - float(y_)));
						if (edist < TH3) {
							if (conf>maxconf) {
								maxconf = conf;
								circle(img, Point((handx + x_) / 2, (handy + y_) / 2), float(TH3) / 2, Scalar(0, 255, 255), 4);
								if (prediction == 4) prediction = 16;
								if (prediction == 12) prediction = 17;
							}
						}
					}

				} else if (cls == 67) {
					putText(img, "phone", Point(int(x_), int(y_)), 3, 3, Scalar(255, 0, 0), 3);
					line(img, Point(int(x_ - 10), int(y_)), Point(int(x_ + 10), int(y_)), Scalar(255, 0, 0), 5);
					line(img, Point(int(x_), int(y_ - 10)), Point(int(x_), int(y_ + 10)), Scalar(255, 0, 0), 5);
					if ( (prediction == 6)||(prediction == 15) ) {
						float handx = float(selJoints.keypoints[7].x);
						float handy = float(selJoints.keypoints[7].y);
						float edist = sqrt((handx - float(x_)) * (handx - float(x_)) + (handy - float(y_)) * (handy - float(y_)));
						if (edist < TH3) {
							if (conf > maxconf) {
								maxconf = conf;
								circle(img, Point((handx + x_) / 2, (handy + y_) / 2), float(TH3) / 2, Scalar(0, 255, 255), 4);
								prediction = 18;
							}
						}
					} else if ((prediction == 3) || (prediction == 11)) {
						float handx = float(selJoints.keypoints[7].x);
						float handy = float(selJoints.keypoints[7].y);
						float edist = sqrt((handx - float(x_)) * (handx - float(x_)) + (handy - float(y_)) * (handy - float(y_)));
						if (edist < TH3) {
							if (conf > maxconf) {
								maxconf = conf;
								circle(img, Point((handx + x_) / 2, (handy + y_) / 2), float(TH3) / 2, Scalar(0, 255, 255), 4);
								//if (prediction == 3) prediction = 20; new class?
								if (prediction == 11) prediction = 19;
							}
						}
					} else if ((prediction == 4) || (prediction == 12)) {
						float handx = float(selJoints.keypoints[4].x);
						float handy = float(selJoints.keypoints[4].y);
						float cupx = float(x_);
						float cupy = float(y_);
						float edist = sqrt((handx - float(x_)) * (handx - float(x_)) + (handy - float(y_)) * (handy - float(y_)));
						if (edist < TH3) {
							if (conf > maxconf) {
								maxconf = conf;
								circle(img, Point((handx + x_) / 2, (handy + y_) / 2), float(TH3) / 2, Scalar(0, 255, 255), 4);
								//if (prediction == 4) prediction = 20;
								if (prediction == 12) prediction = 19;
							}
						}
					}
				}
			}
			fclose(dinp);
			
			//if(prediction==gt) {
			int fontcolor = 25;
			int thickness = 5;
				switch (prediction) {
				case 0:	 putText(img, "0: Standing", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 1:	 putText(img, "1: Bowing", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 2:	 putText(img, "2: Sitting", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 3:	 putText(img, "3: Raising left hand", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 4:	 putText(img, "4: Raising right hand", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 5:	 putText(img, "5: Raising two hands", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 6:	 putText(img, "6: Holding two hands", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 7:	 putText(img, "7: Pointing left", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 8:	 putText(img, "8: Pointing right", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 9:	 putText(img, "9: Pointing left with two hands", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 10: putText(img, "10: Pointing right with two hands", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 11: putText(img, "11: Touching face with left hand", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 12: putText(img, "12: Touching face with right hand", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 13: putText(img, "13: Touching head with two hand", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 14: putText(img, "14: Doing yoga", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 15: putText(img, "15: Holding a cup", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 16: putText(img, "16: Raising a cup", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 17: putText(img, "17: Drinking", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 18: putText(img, "18: Holding a phone", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				case 19: putText(img, "19: Phoning", Point(int(100), int(100)), 2, 3, Scalar::all(fontcolor), thickness); break;
				default:
					break;
				}
			//} else {
			//}
			fprintf(fout, "%d\n", prediction);
			sprintf(oname, "out/%04d_%02d.jpg", cnt, prediction);
			imwrite(oname, img);
			cnt++;
		}
		fclose(cinp);
	}
	fclose(finp);
	fclose(fout);

	return 0;
}