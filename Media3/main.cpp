#include "MediaImage.hpp"
#include "aruco/include/opencv2/aruco.hpp"
#include "sharemem.h"
#include <Windows.h>


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

	cv::VideoCapture capture = cv::VideoCapture(5);/* カメラが2台ある場合は数値で指定 */
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

	bool flag = false;

	//回復アイテム
	item heal1(10, 3, 1, 1, "回復薬(小)");

	while (1) {
		cv::Mat input;
		capture >> input;

		cv::imshow("input", input);/* カメラより得られた画像inputを画面出力*/
		//frameからマーカーを検出し、位置をcorners, IDをidsに格納する
		cv::aruco::detectMarkers(input, dictionary, corners, ids);
		// 検出したマーカーの場所を囲みIDを付してframeに格納する
		if (ids.size() > 0 && !flag) {
			flag = true;
			//検出したマーカーのidが10~19の場合 回復薬使用
			if (ids[0] >= 10 && ids[0] <= 19) {
				//検出したマーカーの位置を囲む
				cv::aruco::drawDetectedMarkers(input, corners, ids);
				//検出したマーカーの位置を取得
				cv::Point2f center = corners[0][0];
				//回復アイテムのamountが１以上であるか
				if (heal1.amount >= 1) {
					//mmdにメッセージを送る
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], heal1.amount);
					//itemのamountを減らす
					heal1.amount--;
				}
				else {
					printf("EMPTY_ITEM %d\n", ids[0]);
				}
				/*検出したマーカーの位置を画面に表示
				cv::putText(input, std::to_string(ids[0]), center, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
				検出したマーカーの位置を共有メモリに格納
				mmd_camera->x = center.x;
				mmd_camera->y = center.y;
				mmd_camera->id = ids[0]; */
			}
			//cv::aruco::drawDetectedMarkers(input, corners, ids);
			printf("marker: %d\n", ids[0]);
		}
		else if (ids.size() == 0) {
			flag = false;
		}

		int key = cv::waitKey(15);/* キー入力，数値は入力待ち時間(ミリ秒)*/

		if (key == 'q' || key == 'Q') {   /* プログラム終了条件 */
			break;
		}
	}
	Closeshare(mmd_camera);
	cv::destroyAllWindows();
	return 0;
}