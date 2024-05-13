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
	//argv[1] = "data/imgList.txt";	
	FILE* finp = fopen(argv[1], "r");
	if (finp == NULL) {
		printf("Error in file opening\n");
		exit(1);
	}
	else {
		//_mkdir("out");
		op::Wrapper opWrapper{ op::ThreadManagerMode::Asynchronous };
		opWrapper.start();

		int cnt = 0, label;
		char dname1[256], dname2[256], fname[256], iname[256], oname[256];
		float TH1 = 0.75, TH2 = 0.05;

		while (!feof(finp)) {
			//printf("# of images: %d\n", cnt++);
			fscanf(finp, "%s %s %s %d\n", dname1, dname2, fname, &label);
			sprintf(iname, "%s/%s", dname1, fname);
			//printf("%s\n", iname);
			_mkdir(dname2);

			Mat img = imread(iname);
			const op::Matrix imageToProcess = OP_CV2OPCONSTMAT(img);
			auto datumProcessed = opWrapper.emplaceAndPop(imageToProcess);
			//imshow("User worker GUI", datumProcessed->at(0).cvOutputData);	
			//waitKey(-1);
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

			if (joints_set.size() == 0) {
				sprintf(oname, "%s/%s-x.jpg", dname2, fname);
				imwrite(oname, img);
				sprintf(oname, "%s/%s-x.txt", dname2, fname);
				FILE* fout = fopen(oname, "w");
				fclose(fout);

				sprintf(oname, "%s/blacklist.txt", dname2);
				FILE* fadd = fopen(oname, "a+");
				fprintf(fout, "%s\n", fname);
				fclose(fadd);
			}
			else {
				/* handling partial occ. */
				//for (int p = 0; p<joints_set.size(); p++) {
				//	printf("pid: %d\n", p);
				//	for (int k = 0; k<15; k++) printf("%f %f %f\n", joints_set[p][k][0], joints_set[p][k][1], joints_set[p][k][2]);				
				//}
				char text[32];
				float maxSum = 0;
				int selPerson = 0;
				for (int p = 0; p < joints_set.size(); p++) {
					float sum = 0;
					if (joints_set[p][0][2] < TH2) { joints_set[p][0][0] = joints_set[p][1][0]; joints_set[p][0][1] = joints_set[p][1][1]; joints_set[p][0][2] = -1; }
					if (joints_set[p][2][2] < TH2) { joints_set[p][2][0] = joints_set[p][1][0]; joints_set[p][2][1] = joints_set[p][1][1]; joints_set[p][2][2] = -1; }
					if (joints_set[p][5][2] < TH2) { joints_set[p][5][0] = joints_set[p][1][0]; joints_set[p][5][1] = joints_set[p][1][1]; joints_set[p][5][2] = -1; }
					if (joints_set[p][8][2] < TH2) { joints_set[p][8][0] = joints_set[p][1][0]; joints_set[p][8][1] = joints_set[p][1][1]; joints_set[p][8][2] = -1; }
					sum += sqrt((joints_set[p][1][0] - joints_set[p][0][0]) * (joints_set[p][1][0] - joints_set[p][0][0]) + (joints_set[p][1][1] - joints_set[p][0][1]) * (joints_set[p][1][1] - joints_set[p][0][1]));
					sum += sqrt((joints_set[p][1][0] - joints_set[p][2][0]) * (joints_set[p][1][0] - joints_set[p][2][0]) + (joints_set[p][1][1] - joints_set[p][2][1]) * (joints_set[p][1][1] - joints_set[p][2][1]));
					sum += sqrt((joints_set[p][1][0] - joints_set[p][5][0]) * (joints_set[p][1][0] - joints_set[p][5][0]) + (joints_set[p][1][1] - joints_set[p][5][1]) * (joints_set[p][1][1] - joints_set[p][5][1]));
					sum += sqrt((joints_set[p][1][0] - joints_set[p][8][0]) * (joints_set[p][1][0] - joints_set[p][8][0]) + (joints_set[p][1][1] - joints_set[p][8][1]) * (joints_set[p][1][1] - joints_set[p][8][1]));

					if (joints_set[p][3][2] < TH2) { joints_set[p][3][0] = joints_set[p][2][0]; joints_set[p][3][1] = joints_set[p][2][1]; joints_set[p][3][2] = -1; }
					if (joints_set[p][4][2] < TH2) { joints_set[p][4][0] = joints_set[p][3][0]; joints_set[p][4][1] = joints_set[p][3][1]; joints_set[p][4][2] = -1; }
					if (joints_set[p][6][2] < TH2) { joints_set[p][6][0] = joints_set[p][5][0]; joints_set[p][6][1] = joints_set[p][5][1]; joints_set[p][6][2] = -1; }
					if (joints_set[p][7][2] < TH2) { joints_set[p][7][0] = joints_set[p][6][0]; joints_set[p][7][1] = joints_set[p][6][1]; joints_set[p][7][2] = -1; }
					sum += sqrt((joints_set[p][3][0] - joints_set[p][2][0]) * (joints_set[p][3][0] - joints_set[p][2][0]) + (joints_set[p][3][1] - joints_set[p][2][1]) * (joints_set[p][3][1] - joints_set[p][2][1]));
					sum += sqrt((joints_set[p][4][0] - joints_set[p][3][0]) * (joints_set[p][4][0] - joints_set[p][3][0]) + (joints_set[p][4][1] - joints_set[p][3][1]) * (joints_set[p][4][1] - joints_set[p][3][1]));
					sum += sqrt((joints_set[p][6][0] - joints_set[p][5][0]) * (joints_set[p][6][0] - joints_set[p][5][0]) + (joints_set[p][6][1] - joints_set[p][5][1]) * (joints_set[p][6][1] - joints_set[p][5][1]));
					sum += sqrt((joints_set[p][7][0] - joints_set[p][6][0]) * (joints_set[p][7][0] - joints_set[p][6][0]) + (joints_set[p][7][1] - joints_set[p][6][1]) * (joints_set[p][7][1] - joints_set[p][6][1]));

					if (joints_set[p][9][2] < TH2) { joints_set[p][9][0] = joints_set[p][8][0]; joints_set[p][9][1] = joints_set[p][8][1]; joints_set[p][9][2] = -1; }
					if (joints_set[p][10][2] < TH2) { joints_set[p][10][0] = joints_set[p][9][0]; joints_set[p][10][1] = joints_set[p][9][1]; joints_set[p][10][2] = -1; }
					if (joints_set[p][11][2] < TH2) { joints_set[p][11][0] = joints_set[p][10][0]; joints_set[p][11][1] = joints_set[p][10][1]; joints_set[p][11][2] = -1; }
					if (joints_set[p][12][2] < TH2) { joints_set[p][12][0] = joints_set[p][8][0]; joints_set[p][12][1] = joints_set[p][8][1]; joints_set[p][12][2] = -1; }
					if (joints_set[p][13][2] < TH2) { joints_set[p][13][0] = joints_set[p][12][0]; joints_set[p][13][1] = joints_set[p][12][1]; joints_set[p][13][2] = -1; }
					if (joints_set[p][14][2] < TH2) { joints_set[p][14][0] = joints_set[p][13][0]; joints_set[p][14][1] = joints_set[p][13][1]; joints_set[p][14][2] = -1; }
					sum += sqrt((joints_set[p][9][0] - joints_set[p][8][0]) * (joints_set[p][9][0] - joints_set[p][8][0]) + (joints_set[p][9][1] - joints_set[p][8][1]) * (joints_set[p][9][1] - joints_set[p][8][1]));
					sum += sqrt((joints_set[p][10][0] - joints_set[p][9][0]) * (joints_set[p][10][0] - joints_set[p][9][0]) + (joints_set[p][10][1] - joints_set[p][9][1]) * (joints_set[p][10][1] - joints_set[p][9][1]));
					sum += sqrt((joints_set[p][11][0] - joints_set[p][10][0]) * (joints_set[p][11][0] - joints_set[p][10][0]) + (joints_set[p][11][1] - joints_set[p][10][1]) * (joints_set[p][11][1] - joints_set[p][10][1]));
					sum += sqrt((joints_set[p][12][0] - joints_set[p][8][0]) * (joints_set[p][12][0] - joints_set[p][8][0]) + (joints_set[p][12][1] - joints_set[p][8][1]) * (joints_set[p][12][1] - joints_set[p][8][1]));
					sum += sqrt((joints_set[p][13][0] - joints_set[p][12][0]) * (joints_set[p][13][0] - joints_set[p][12][0]) + (joints_set[p][13][1] - joints_set[p][12][1]) * (joints_set[p][13][1] - joints_set[p][12][1]));
					sum += sqrt((joints_set[p][14][0] - joints_set[p][13][0]) * (joints_set[p][14][0] - joints_set[p][13][0]) + (joints_set[p][14][1] - joints_set[p][13][1]) * (joints_set[p][14][1] - joints_set[p][13][1]));

					if (sum > maxSum) {
						maxSum = sum;
						selPerson = p;
					}
				}
				//imshow("img", img);
				//waitKey(-1);

				/* img2edm */
				joints selJoints;
				selJoints.keypoints.clear();
				int found = 0;
				for (int i = 0; i < 15; i++) {
					if(int(joints_set[selPerson][i][2] > 0)) {
						found = found + 1;
						keypoint joint;
						joint.x = joints_set[selPerson][i][0];
						joint.y = joints_set[selPerson][i][1];
						selJoints.keypoints.push_back(joint);
					}
				}
				selJoints.avgLength = maxSum / float(found);

				int scl = 1;
				putText(img, "00", Point(int(selJoints.keypoints[0 ].x), int(selJoints.keypoints[0 ].y)), 2, scl, Scalar::all(255));
				putText(img, "01", Point(int(selJoints.keypoints[1 ].x), int(selJoints.keypoints[1 ].y)), 2, scl, Scalar::all(255));
				putText(img, "02", Point(int(selJoints.keypoints[2 ].x), int(selJoints.keypoints[2 ].y)), 2, scl, Scalar::all(255));
				putText(img, "03", Point(int(selJoints.keypoints[3 ].x), int(selJoints.keypoints[3 ].y)), 2, scl, Scalar::all(255));
				putText(img, "04", Point(int(selJoints.keypoints[4 ].x), int(selJoints.keypoints[4 ].y)), 2, scl, Scalar::all(255));
				putText(img, "05", Point(int(selJoints.keypoints[5 ].x), int(selJoints.keypoints[5 ].y)), 2, scl, Scalar::all(255));
				putText(img, "06", Point(int(selJoints.keypoints[6 ].x), int(selJoints.keypoints[6 ].y)), 2, scl, Scalar::all(255));
				putText(img, "07", Point(int(selJoints.keypoints[7 ].x), int(selJoints.keypoints[7 ].y)), 2, scl, Scalar::all(255));
				putText(img, "08", Point(int(selJoints.keypoints[8 ].x), int(selJoints.keypoints[8 ].y)), 2, scl, Scalar::all(255));
				putText(img, "09", Point(int(selJoints.keypoints[9 ].x), int(selJoints.keypoints[9 ].y)), 2, scl, Scalar::all(255));
				putText(img, "10", Point(int(selJoints.keypoints[10].x), int(selJoints.keypoints[10].y)), 2, scl, Scalar::all(255));
				putText(img, "11", Point(int(selJoints.keypoints[11].x), int(selJoints.keypoints[11].y)), 2, scl, Scalar::all(255));
				putText(img, "12", Point(int(selJoints.keypoints[12].x), int(selJoints.keypoints[12].y)), 2, scl, Scalar::all(255));
				putText(img, "13", Point(int(selJoints.keypoints[13].x), int(selJoints.keypoints[13].y)), 2, scl, Scalar::all(255));
				putText(img, "14", Point(int(selJoints.keypoints[14].x), int(selJoints.keypoints[14].y)), 2, scl, Scalar::all(255));

				arrowedLine(img, Point(int(selJoints.keypoints[1].x ), int(selJoints.keypoints[1].y )), Point(int(selJoints.keypoints[0].x), int(selJoints.keypoints[0].y)), Scalar(255, 0, 255));
				arrowedLine(img, Point(int(selJoints.keypoints[1].x ), int(selJoints.keypoints[1].y )), Point(int(selJoints.keypoints[2].x), int(selJoints.keypoints[2].y)), Scalar(255, 0, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[1].x ), int(selJoints.keypoints[1].y )), Point(int(selJoints.keypoints[5].x), int(selJoints.keypoints[5].y)), Scalar(0, 0, 255));
				arrowedLine(img, Point(int(selJoints.keypoints[1].x ), int(selJoints.keypoints[1].y )), Point(int(selJoints.keypoints[8].x), int(selJoints.keypoints[8].y)), Scalar(0, 255, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[2].x ), int(selJoints.keypoints[2].y )), Point(int(selJoints.keypoints[3].x), int(selJoints.keypoints[3].y)), Scalar(255, 64, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[3].x ), int(selJoints.keypoints[3].y )), Point(int(selJoints.keypoints[4].x), int(selJoints.keypoints[4].y)), Scalar(255, 128, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[5].x ), int(selJoints.keypoints[5].y )), Point(int(selJoints.keypoints[6].x), int(selJoints.keypoints[6].y)), Scalar(0, 64, 255));
				arrowedLine(img, Point(int(selJoints.keypoints[6].x ), int(selJoints.keypoints[6].y )), Point(int(selJoints.keypoints[7].x), int(selJoints.keypoints[7].y)), Scalar(0, 128, 255));
				arrowedLine(img, Point(int(selJoints.keypoints[8].x ), int(selJoints.keypoints[8].y )), Point(int(selJoints.keypoints[9].x), int(selJoints.keypoints[9].y)), Scalar(64, 255, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[9].x ), int(selJoints.keypoints[9].y )), Point(int(selJoints.keypoints[10].x),int(selJoints.keypoints[10].y)), Scalar(128, 255, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[10].x), int(selJoints.keypoints[10].y)), Point(int(selJoints.keypoints[11].x),int(selJoints.keypoints[11].y)), Scalar(255, 255, 0));
				arrowedLine(img, Point(int(selJoints.keypoints[8].x ), int(selJoints.keypoints[8].y )), Point(int(selJoints.keypoints[12].x),int(selJoints.keypoints[12].y)), Scalar(0, 255, 64));
				arrowedLine(img, Point(int(selJoints.keypoints[12].x), int(selJoints.keypoints[12].y)), Point(int(selJoints.keypoints[13].x),int(selJoints.keypoints[13].y)), Scalar(0, 255, 128));
				arrowedLine(img, Point(int(selJoints.keypoints[13].x), int(selJoints.keypoints[13].y)), Point(int(selJoints.keypoints[14].x),int(selJoints.keypoints[14].y)), Scalar(0, 255, 255));

				sprintf(oname, "%s/%s.jpg", dname2, fname);
				imwrite(oname, img);

				sprintf(oname, "%s/%s.txt", dname2, fname);
				FILE* fout = fopen(oname, "w");
				fprintf(fout, "%d, ", label);
				for (int i = 0; i < 14; i++)
				for (int j = i + 1; j < 15; j++) {
					fprintf(fout, "%.4f", sqrt((selJoints.keypoints[i].x - selJoints.keypoints[j].x) * (selJoints.keypoints[i].x - selJoints.keypoints[j].x) + (selJoints.keypoints[i].y - selJoints.keypoints[j].y) * (selJoints.keypoints[i].y - selJoints.keypoints[j].y)) / selJoints.avgLength);
					if ((i == 13) & (j == 14)) fprintf(fout, "\n");
					else fprintf(fout, ", ");
				}
				fprintf(fout, "\n");
				fclose(fout);
			}
		}
	}
	fclose(finp);
	return 0;
}
