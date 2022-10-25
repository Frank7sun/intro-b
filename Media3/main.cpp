#include "MediaImage.hpp"
#include "aruco/include/opencv2/aruco.hpp"
#include "sharemem.h"
#include <Windows.h>
#include <regex>


using namespace std;
using namespace media;

MMD_CAMERA* mmd_camera;

//id, amount, tier, type, nameからなるitemクラス
class item {
public:
	int id;
	int amount;
	int tier;
	int type;
	string name;

	//constructor
	item(int id, int amount, int tier, int type, string name) {
		this->id = id;
		this->amount = amount;
		this->tier = tier;
		this->type = type;
		this->name = name;
	}
};



int main(void)
{
	SetConsoleOutputCP(CP_UTF8);/*コンソールをUTF-8にする*/
	setvbuf(stdout, nullptr, _IOFBF, 2); /* バッファリングに必要 */
	mmd_camera = Getshare();/* 共有メモリの取得 */

	MediaImage mediaImage = MediaImage();
	/**************↑変更しない*******************/

	/**************↓適宜変更する*******************/

	cv::VideoCapture capture = cv::VideoCapture(1);/* カメラが2台ある場合は数値で指定 */
	if (!capture.isOpened())
	{
		std::cerr << "Camera not found";
		return -1;
	}
	capture.set(cv::CAP_PROP_FPS, 30);    /* 画像のフレームレート */
	capture.set(cv::CAP_PROP_FRAME_WIDTH, 640); /* 画像の最大解像度 */
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);/* 画像の最大解像度 */

	// マーカー検出の辞書の準備
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	// 検出され他マーカーのIDを格納する配列
	std::vector<int> ids;
	// 検出されたマーカーの位置を格納する配列
	std::vector<std::vector<cv::Point2f>> corners;

	//マーカーが複数回検知されないためのフラグ
	//int flag = 0;
	//マーカー検知するべきかどうか
	int markerMode = 0;
	//printf("type mode 1 or 2\n");
	//scanf_s("%d", &markerMode);

	//アイテムの宣言 item 名前, id, amount, tier(1|2|3), type(0:数字 1:回復 2:攻撃 3:防御 4:フラグ)
	//item number0(0, 0, 1, 0, "0");

	item heal1(10, 0, 1, 1, "HealHP(small)");
	//item heal2(11, 0, 2, 1, "HealHP(medium)");
	//item heal3(12, 0, 3, 1, "HealHP(large)");

	item attack1(20, 0, 1, 2, "AttackIncrease(small)");
	//item attack2(21, 0, 2, 2, "AttackIncrease(medium)");
	//item attack3(22, 0, 3, 2, "AttackIncrease(large)");

	//item defend1(30, 3, 1, 3, "DefendIncrease(small)");
	//item defend2(31, 2, 2, 3, "DefendIncrease(medium)");
	//item defend3(32, 1, 3, 3, "DefendIncrease(large)");

	//item flag1(40, 1, 1, 4, "ItemFlag1");
	// 
	//std::regex ClaimMode("SYNTH_START \| mei \| mei_voice_normal \| ここではアイテムを見つけました.*");
	std::regex ClaimMode("LIPSYNC_EVENT_START\|mei,105,k,50,o,95,d,35,e,90,w,35,a,105,a,110,i,105,t,40,e,85,m,40,u,95,o,115,m,40.*");
	std::regex UseMode("SYNTH_START\|mei\|mei_voice_normal\|アイテムを使用します.*");

	//std::string event = mmd_camera->event;
	//event = "";

	while (1) {
		cv::Mat input;
		capture >> input;

		cv::imshow("input", input);/* カメラより得られた画像inputを画面出力*/
		//frameからマーカーを検出し、位置をcorners, IDをidsに格納する
		cv::aruco::detectMarkers(input, dictionary, corners, ids);
		
		// マーカーの入力を受け付けているか
		

		
		if (std::regex_match(mmd_camera->event, ClaimMode)) {
			markerMode = 1;
			printf("ClaimMode MODE ENABLED\n");
			printf("%s\n", mmd_camera->event);
		}
		else if (std::regex_match(mmd_camera->event, UseMode)) {
			markerMode = 2;
			printf("UseMode MODE ENABLED\n");
			printf("%s\n", mmd_camera->event);
		}

		// アイテム登録
		if (ids.size() > 0 && markerMode == 1) {
			markerMode = 0;
			//マーカーのIDによって処理を分岐
			switch (ids[0]) {
			case 10:
				heal1.amount += 1;
				printf("%sが追加され%d個になりました\n", heal1.name.c_str(), heal1.amount);
				break;

			case 20:
				attack1.amount += 1;
				printf("%sが追加され%d個になりました\n", attack1.name.c_str(), attack1.amount);
				break;
			}
		}

		//アイテム使用
		if (ids.size() > 0 && markerMode == 2) {
			markerMode = 0;

			//検出したマーカーの位置を囲む
			cv::aruco::drawDetectedMarkers(input, corners, ids);
			//検出したマーカーの位置を取得
			cv::Point2f center = corners[0][0];

			//検出したマーカーのidが10の場合 回復薬使用
			switch (ids[0]) {
			case 10:
				if (heal1.amount > 0) {
					heal1.amount -= 1;
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], heal1.amount);
					break;
				}
				else {
					sprintf_s(mmd_camera->camera, "emptyItem");
					printf("EMPTY_ITEM %d\n", ids[0]);
					break;
				}
			case 20:
				if (attack1.amount > 0) {
					attack1.amount -= 1;
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], attack1.amount);
					break;
				}
				else {
					sprintf_s(mmd_camera->camera, "emptyItem");
					printf("EMPTY_ITEM %d\n", ids[0]);
					break;
				}
			}
			/*
			if (ids[0] >= 10 && ids[0] <= 19) {
				//回復アイテムのamountが１以上であるか
				if (heal1.amount >= 1) {
					//itemのamountを減らす
					heal1.amount--;
					//mmdにメッセージを送る
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], heal1.amount);
				}
				else {
					sprintf_s(mmd_camera->camera, "emptyItem");
					printf("EMPTY_ITEM %d\n", ids[0]);
				}
				markerMode = 0;
				//検出したマーカーの位置を画面に表示
				//cv::putText(input, std::to_string(ids[0]), center, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
			}
			else if (ids[0] >= 20 && ids[0] <= 29) {
				if (attack1.amount >= 1) {
					//itemのamountを減らす
					attack1.amount--;
					//mmdにメッセージを送る
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], attack1.amount);
				}
				else {
					sprintf_s(mmd_camera->camera, "emptyItem");
					printf("EMPTY_ITEM %d\n", ids[0]);
				}
				markerMode = 0;
			}
			*/
			//cv::aruco::drawDetectedMarkers(input, corners, ids);
			printf("marker: %d\n", ids[0]);
		}
		//printf("%d\n", flag);
		printf("%s\n", mmd_camera->event);

		int key = cv::waitKey(15);/* キー入力，数値は入力待ち時間(ミリ秒)*/

		if (key == 'q' || key == 'Q') {   /* プログラム終了条件 */
			break;
		}
	}
	Closeshare(mmd_camera);
	cv::destroyAllWindows();
	return 0;
}