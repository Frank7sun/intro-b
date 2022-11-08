#include "MediaImage.hpp"
#include "aruco/include/opencv2/aruco.hpp"
#include "sharemem.h"
#include <Windows.h>
#include <regex>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>


using namespace std;
using namespace media;

MMD_CAMERA* mmd_camera;

//id, amount, tier, type, name����Ȃ�item�N���X
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

int marker[200];

//�ǂݎ�����}�[�J�[����id�ɉ�����item id��Ԃ�
int getItemId(int ids) {
	if (ids >= 10 && ids <= 19) {
		marker[ids] = 1;
		return 10;
	}
	if (ids >= 20 && ids <= 29){
		return 20;
	}
}

//������id���g�p�ς݂��`�F�b�N
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
	SetConsoleOutputCP(CP_UTF8);/*�R���\�[����UTF-8�ɂ���*/
	setvbuf(stdout, nullptr, _IOFBF, 2); /* �o�b�t�@�����O�ɕK�v */
	mmd_camera = Getshare();/* ���L�������̎擾 */

	MediaImage mediaImage = MediaImage();
	/**************���ύX���Ȃ�*******************/
	
	//window capture
	HWND desktop = GetDesktopWindow();
	RECT rect;
	GetWindowRect(desktop, &rect);
	
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = rect.right;
	bmpInfo.bmiHeader.biHeight = -rect.bottom;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24; //cv::Mat�̉摜���A���t�@�`�����l���L��ɂ���ꍇ�͂�32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	
	LPDWORD lpPixel;
	HDC hDC = GetDC(desktop);
	HBITMAP hBitmap = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBitmap);

	cv::Mat monitor_img;
	monitor_img.create(rect.bottom, rect.right, CV_8UC3); //RGB�̂݁B�A���t�@�����˂��������ɂ�CV_8UN4
	
	// �摜�\���p��Window���쐬�i�E�B���h�E�̑傫����native/2�ɌŒ肷��j
	//cv::namedWindow("Screenshot", cv::WINDOW_NORMAL);
	//cv::resizeWindow("Screenshot", GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
	

	/**************���K�X�ύX����*******************/

	cv::VideoCapture capture = cv::VideoCapture(1);/* �J������2�䂠��ꍇ�͐��l�Ŏw�� */
	if (!capture.isOpened())
	{
		std::cerr << "Camera not found";
		return -1;
	}
	capture.set(cv::CAP_PROP_FPS, 30);    /* �摜�̃t���[�����[�g */
	capture.set(cv::CAP_PROP_FRAME_WIDTH, 640); /* �摜�̍ő�𑜓x */
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);/* �摜�̍ő�𑜓x */

	// �}�[�J�[���o�̎����̏���
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	// ���o���ꑼ�}�[�J�[��ID���i�[����z��
	std::vector<int> ids;
	std::vector<int> mmdIds;
	// ���o���ꂽ�}�[�J�[�̈ʒu���i�[����z��
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<std::vector<cv::Point2f>> mmdcorners;

	//�}�[�J�[���m����ׂ����ǂ���
	int markerMode = 0;

	//�A�C�e���̐錾 item ���O, id, amount, tier(1|2|3), type(0:���� 1:�� 2:�U�� 3:�h�� 4:�t���O)
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
	//std::regex ClaimMode("SYNTH_START \| mei \| mei_voice_normal \| �����ł̓A�C�e���������܂���.*");
	//std::regex ClaimMode("LIPSYNC_EVENT_START\|mei,105,k,50,o,95,d,35,e,90,w,35,a,105,a,110,i,105,t,40,e,85,m,40,u,95,o,115,m,40.*");
	//std::regex UseMode("SYNTH_START\|mei\|mei_voice_normal\|�A�C�e�����g�p���܂�.*");

	//std::string event = mmd_camera->event;
	//event = "";
	
	int musicFlag = 0;
	int prevMusicFlag = 99;
	
	//���y�Đ�
	//PlaySound("default_bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	
	while (1) {
		cv::Mat input;
		capture >> input;

		BitBlt(hMemDC, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
		GetDIBits(hMemDC, hBitmap, 0, rect.bottom, monitor_img.data, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

		//cv::imshow("Screenshot", monitor_img); // �X�N���[���V���b�g�̕\��
		
		//cv::Mat target = getMat(hWND);
		//cv::cvtColor(target, target, cv::COLOR_BGR2HSV);

		//cv::imshow("output", target);
		cv::imshow("input", input);/* �J������蓾��ꂽ�摜input����ʏo��*/
		//frame����}�[�J�[�����o���A�ʒu��corners, ID��ids�Ɋi�[����
		cv::aruco::detectMarkers(input, dictionary, corners, ids);
		cv::aruco::detectMarkers(monitor_img, dictionary, mmdcorners, mmdIds);

		// �}�[�J�[�̓��͂��󂯕t���Ă��邩
		if (mmdIds.size() > 0) {
			if (mmdIds[0] == 1) {
				markerMode = 1;
				printf("Register Mode Enabled\n");
				sprintf_s(mmd_camera->camera, "mode_changed");
			}
			else if (mmdIds[0] == 2) {
				markerMode = 2;
				printf("Use Mode Enabled\n");
				sprintf_s(mmd_camera->camera, "mode_changed");

			}
			else if (mmdIds[0] == 0) {
				markerMode = 0;
				printf("Marker Mode Disabled\n");
			}
			else if (mmdIds[0] == 10) {
				musicFlag = 0;
			}
			else if (mmdIds[0] == 11) {
				musicFlag = 1;
			}
			else if (mmdIds[0] == 12) {
				musicFlag = 2;
			}
		}

		//���y�Đ����� �ϐ����ω����Ă��Ȃ��ꍇ�͕ύX���Ȃ�
		if (musicFlag != prevMusicFlag) {
			if (musicFlag == 0) {
				PlaySound("default_bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			}
			else if (musicFlag == 1) {
				PlaySound("battle_bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			}
			else if (musicFlag == 2) {
				PlaySound("boss_bgm.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			}
		}
		prevMusicFlag = musicFlag;

		/*
		���K�\���ł�낤�Ƃ������ǂ��܂������񂩂���
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

		// �A�C�e���o�^
		if (ids.size() > 0 && markerMode == 1) {
			//�}�[�J�[��ID�ɂ���ď����𕪊�
			if (idCheck(ids[0]) == true) {
				markerMode = 0;
				switch (getItemId(ids[0])) {
				case 10:
					heal1.amount += 1;
					printf("%s���ǉ�����%d�ɂȂ�܂���\n", heal1.name.c_str(), heal1.amount);
					sprintf_s(mmd_camera->camera, "item_registerd");
					PlaySound("get.wav", NULL, SND_FILENAME);
					prevMusicFlag = 99;
					break;

				case 20:
					attack1.amount += 1;
					printf("%s���ǉ�����%d�ɂȂ�܂���\n", attack1.name.c_str(), attack1.amount);
					sprintf_s(mmd_camera->camera, "item_registerd");
					PlaySound("get.wav", NULL, SND_FILENAME);
					prevMusicFlag = 99;
					break;
				}
			}
			else {
				printf("Used Marker\n");
			}
		}

		//�A�C�e���g�p
		if (ids.size() > 0 && markerMode == 2) {
			markerMode = 0;

			//���o�����}�[�J�[�̈ʒu���͂�
			cv::aruco::drawDetectedMarkers(input, corners, ids);
			//���o�����}�[�J�[�̈ʒu���擾
			cv::Point2f center = corners[0][0];

			//���o�����}�[�J�[��id��10�̏ꍇ �񕜖�g�p
			switch (ids[0]) {
			case 10:
				if (heal1.amount > 0) {
					heal1.amount -= 1;
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], heal1.amount);
					PlaySound("item_use.wav", NULL, SND_FILENAME);
					prevMusicFlag = 99;
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
					PlaySound("item_use.wav", NULL, SND_FILENAME);
					prevMusicFlag = 99;
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


		int key = cv::waitKey(15);/* �L�[���́C���l�͓��͑҂�����(�~���b)*/

		if (key == 'q' || key == 'Q') {   /* �v���O�����I������ */
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