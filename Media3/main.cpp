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
	int flag = 0;
	//マーカー検知するべきかどうか
	bool markerActive = false;

	//アイテムの宣言 item 名前, id, amount, tier(1|2|3), type(0:数字 1:回復 2:攻撃 3:防御 4:フラグ)
	item number0(0, 0, 1, 0,"0");
	
	item heal1(10, 3, 1, 1, "回復薬(小)");
	item heal2(11, 2, 2, 1, "回復薬(中)");
	item heal3(12, 1, 3, 1, "回復薬(大)");
	
	item attack1(20, 3, 1, 2, "攻撃薬(小)");
	item attack2(21, 2, 2, 2, "攻撃薬(中)");
	item attack3(22, 1, 3, 2, "攻撃薬(大)");
	
	item defend1(30, 3, 1, 3, "防御薬(小)");
	item defend2(31, 2, 2, 3, "防御薬(中)");
	item defend3(32, 1, 3, 3, "防御薬(大)");
	
	item flag1(40, 1, 1, 4, "フラグ1");

	while (1) {
		cv::Mat input;
		capture >> input;

		cv::imshow("input", input);/* カメラより得られた画像inputを画面出力*/
		//frameからマーカーを検出し、位置をcorners, IDをidsに格納する
		cv::aruco::detectMarkers(input, dictionary, corners, ids);

		//std::regex regex("SYNTH_START\|mei\|mei_voice_normal\|こん.*");
		// マーカーの入力を受け付けているか
		/*if (std::regex_search(mmd_camera->event, regex)) {
			markerActive = true;
			printf("MMDRECEIVED\n");
			printf("%s\n", mmd_camera->event);
		} */

		markerActive = true;
		if (ids.size() > 0 && flag >= 30 && markerActive==true) {
			flag = 0;
			//検出したマーカーの位置を囲む
			cv::aruco::drawDetectedMarkers(input, corners, ids);
			//検出したマーカーの位置を取得
			cv::Point2f center = corners[0][0];
			
			//検出したマーカーのidが10~19の場合 回復薬使用
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
				markerActive = false;
				/*検出したマーカーの位置を画面に表示
				cv::putText(input, std::to_string(ids[0]), center, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
				検出したマーカーの位置を共有メモリに格納
				mmd_camera->x = center.x;
				mmd_camera->y = center.y;
				mmd_camera->id = ids[0]; */
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
				markerActive = false;
			}
			//cv::aruco::drawDetectedMarkers(input, corners, ids);
			printf("marker: %d\n", ids[0]);
			cv::waitKey(10);
		}
		else if (ids.size() == 0) {
			flag++;
			printf("%d\n", flag);
		}
		//printf("%d\n", flag);
		//printf("%s\n", mmd_camera->event);

		int key = cv::waitKey(15);/* キー入力，数値は入力待ち時間(ミリ秒)*/

		if (key == 'q' || key == 'Q') {   /* プログラム終了条件 */
			break;
		}
	}
	Closeshare(mmd_camera);
	cv::destroyAllWindows();
	return 0;
}