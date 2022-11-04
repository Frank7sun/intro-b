#include "MediaImage.hpp"
#include "aruco/include/opencv2/aruco.hpp"
#include "sharemem.h"
#include <Windows.h>
#include <regex>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


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

/*
cv::Mat getMat(HWND hWND) {
	HDC deviceContext = GetDC(hWND);
	HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);
	
	RECT windowRect;
	GetClientRect(hWND, &windowRect);

	int height = windowRect.bottom;
	int width = windowRect.right;

	HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);
	
	SelectObject(memoryDeviceContext, bitmap);
	
	BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY);
	
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1;
	bi.biYPelsPerMeter = 2;
	bi.biClrUsed = 3;
	bi.biClrImportant = 4;

	cv::Mat mat = cv::Mat(height, width, CV_8UC4);
	return mat;
	GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteObject(bitmap);
	DeleteDC(memoryDeviceContext);
	ReleaseDC(hWND, deviceContext);
}
*/

int marker[200];

int getItemId(int ids) {
	if (ids >= 10 && ids <= 19) {
		marker[ids] = 1;
		return 10;
	}
	if (ids >= 20 && ids <= 29){
		return 20;
	}
}

bool idCheck(int ids){
	if (marker[ids] == 1) {
		return false;
	}
	else {
		return true;
	} 
}

int main(void)
{
	SetConsoleOutputCP(CP_UTF8);/*コンソールをUTF-8にする*/
	setvbuf(stdout, nullptr, _IOFBF, 2); /* バッファリングに必要 */
	mmd_camera = Getshare();/* 共有メモリの取得 */

	MediaImage mediaImage = MediaImage();
	/**************↑変更しない*******************/
	
	//window capture
	HWND desktop = GetDesktopWindow();
	RECT rect;
	GetWindowRect(desktop, &rect);
	
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = rect.right;
	bmpInfo.bmiHeader.biHeight = -rect.bottom;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24; //cv::Matの画像をアルファチャンネル有りにする場合はは32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	
	LPDWORD lpPixel;
	HDC hDC = GetDC(desktop);
	HBITMAP hBitmap = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBitmap);

	cv::Mat monitor_img;
	monitor_img.create(rect.bottom, rect.right, CV_8UC3); //RGBのみ。アルファちゃんねるを加えるにはCV_8UN4
	
	// 画像表示用のWindowを作成（ウィンドウの大きさをnative/2に固定する）
	cv::namedWindow("Screenshot", cv::WINDOW_NORMAL);
	cv::resizeWindow("Screenshot", GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
	
	/*
	LPCTSTR windowTitle = "MMDAgent - Toolkit for building voice interaction systems";
	HWND hWND = FindWindow(NULL, windowTitle);
	while (!hWND) {
		std::system("cls");
		std::cout << "Start the MMD..." << std::endl;
		HWND hwnd = FindWindow(NULL, windowTitle);
		Sleep(100);
	}
	*/

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
	std::vector<int> mmdIds;
	// 検出されたマーカーの位置を格納する配列
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> mmdcorners;

	//マーカー検知するべきかどうか
	int markerMode = 0;

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
	//std::regex ClaimMode("LIPSYNC_EVENT_START\|mei,105,k,50,o,95,d,35,e,90,w,35,a,105,a,110,i,105,t,40,e,85,m,40,u,95,o,115,m,40.*");
	//std::regex UseMode("SYNTH_START\|mei\|mei_voice_normal\|アイテムを使用します.*");

	//std::string event = mmd_camera->event;
	//event = "";
	
	//cv::namedWindow("output", cv::WINDOW_NORMAL);

	while (1) {
		cv::Mat input;
		capture >> input;

		BitBlt(hMemDC, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
		GetDIBits(hMemDC, hBitmap, 0, rect.bottom, monitor_img.data, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

		cv::imshow("Screenshot", monitor_img); // スクリーンショットの表示
		
		//cv::Mat target = getMat(hWND);
		//cv::cvtColor(target, target, cv::COLOR_BGR2HSV);

		//cv::imshow("output", target);
		cv::imshow("input", input);/* カメラより得られた画像inputを画面出力*/
		//frameからマーカーを検出し、位置をcorners, IDをidsに格納する
		cv::aruco::detectMarkers(input, dictionary, corners, ids);
		cv::aruco::detectMarkers(monitor_img, dictionary, mmdcorners, mmdIds);
		
		// マーカーの入力を受け付けているか
		if (mmdIds.size() > 0) {
			if (mmdIds[0] == 1) {
				markerMode = 1;
				printf("Register Mode Enabled\n");
			}
			else if (mmdIds[0] == 2) {
				markerMode = 2;
				printf("Use Mode Enabled\n");
			}
			else if (mmdIds[0] == 0) {
				markerMode = 0;
				printf("Marker Mode Disabled\n");
			}
		}

		

		/*
		正規表現でやろうとしたけどうまくいかんかった
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
		*/

		// アイテム登録
		if (ids.size() > 0 && markerMode == 1) {
			//マーカーのIDによって処理を分岐
			if (idCheck(ids[0]) == true) {
				markerMode = 0;
				switch (getItemId(ids[0])) {
				case 10:
					heal1.amount += 1;
					printf("%sが追加され%d個になりました\n", heal1.name.c_str(), heal1.amount);
					sprintf_s(mmd_camera->camera, "item_registerd");
					break;

				case 20:
					attack1.amount += 1;
					printf("%sが追加され%d個になりました\n", attack1.name.c_str(), attack1.amount);
					sprintf_s(mmd_camera->camera, "item_registerd");
					break;
				}
			}
			else {
				printf("Used Marker\n");
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
			cv::aruco::drawDetectedMarkers(input, corners, ids);
			printf("marker: %d\n", ids[0]);
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
	ReleaseDC(desktop, hDC);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	return 0;
}