#include "MediaImage.hpp"
#include "aruco/include/opencv2/aruco.hpp"
#include "sharemem.h"
#include <Windows.h>
#include <regex>


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



int main(void)
{
	SetConsoleOutputCP(CP_UTF8);/*�R���\�[����UTF-8�ɂ���*/
	setvbuf(stdout, nullptr, _IOFBF, 2); /* �o�b�t�@�����O�ɕK�v */
	mmd_camera = Getshare();/* ���L�������̎擾 */

	MediaImage mediaImage = MediaImage();
	/**************���ύX���Ȃ�*******************/

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
	// ���o���ꂽ�}�[�J�[�̈ʒu���i�[����z��
	std::vector<std::vector<cv::Point2f>> corners;

	//�}�[�J�[�������񌟒m����Ȃ����߂̃t���O
	//int flag = 0;
	//�}�[�J�[���m����ׂ����ǂ���
	int markerMode = 0;
	//printf("type mode 1 or 2\n");
	//scanf_s("%d", &markerMode);

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
	std::regex ClaimMode("LIPSYNC_EVENT_START\|mei,105,k,50,o,95,d,35,e,90,w,35,a,105,a,110,i,105,t,40,e,85,m,40,u,95,o,115,m,40.*");
	std::regex UseMode("SYNTH_START\|mei\|mei_voice_normal\|�A�C�e�����g�p���܂�.*");

	//std::string event = mmd_camera->event;
	//event = "";

	while (1) {
		cv::Mat input;
		capture >> input;

		cv::imshow("input", input);/* �J������蓾��ꂽ�摜input����ʏo��*/
		//frame����}�[�J�[�����o���A�ʒu��corners, ID��ids�Ɋi�[����
		cv::aruco::detectMarkers(input, dictionary, corners, ids);
		
		// �}�[�J�[�̓��͂��󂯕t���Ă��邩
		

		
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

		// �A�C�e���o�^
		if (ids.size() > 0 && markerMode == 1) {
			markerMode = 0;
			//�}�[�J�[��ID�ɂ���ď����𕪊�
			switch (ids[0]) {
			case 10:
				heal1.amount += 1;
				printf("%s���ǉ�����%d�ɂȂ�܂���\n", heal1.name.c_str(), heal1.amount);
				break;

			case 20:
				attack1.amount += 1;
				printf("%s���ǉ�����%d�ɂȂ�܂���\n", attack1.name.c_str(), attack1.amount);
				break;
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
				//�񕜃A�C�e����amount���P�ȏ�ł��邩
				if (heal1.amount >= 1) {
					//item��amount�����炷
					heal1.amount--;
					//mmd�Ƀ��b�Z�[�W�𑗂�
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], heal1.amount);
				}
				else {
					sprintf_s(mmd_camera->camera, "emptyItem");
					printf("EMPTY_ITEM %d\n", ids[0]);
				}
				markerMode = 0;
				//���o�����}�[�J�[�̈ʒu����ʂɕ\��
				//cv::putText(input, std::to_string(ids[0]), center, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
			}
			else if (ids[0] >= 20 && ids[0] <= 29) {
				if (attack1.amount >= 1) {
					//item��amount�����炷
					attack1.amount--;
					//mmd�Ƀ��b�Z�[�W�𑗂�
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

		int key = cv::waitKey(15);/* �L�[���́C���l�͓��͑҂�����(�~���b)*/

		if (key == 'q' || key == 'Q') {   /* �v���O�����I������ */
			break;
		}
	}
	Closeshare(mmd_camera);
	cv::destroyAllWindows();
	return 0;
}