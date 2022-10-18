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
	int flag = 0;
	//�}�[�J�[���m����ׂ����ǂ���
	bool markerActive = false;

	//�A�C�e���̐錾 item ���O, id, amount, tier(1|2|3), type(0:���� 1:�� 2:�U�� 3:�h�� 4:�t���O)
	item number0(0, 0, 1, 0,"0");
	
	item heal1(10, 3, 1, 1, "�񕜖�(��)");
	item heal2(11, 2, 2, 1, "�񕜖�(��)");
	item heal3(12, 1, 3, 1, "�񕜖�(��)");
	
	item attack1(20, 3, 1, 2, "�U����(��)");
	item attack2(21, 2, 2, 2, "�U����(��)");
	item attack3(22, 1, 3, 2, "�U����(��)");
	
	item defend1(30, 3, 1, 3, "�h���(��)");
	item defend2(31, 2, 2, 3, "�h���(��)");
	item defend3(32, 1, 3, 3, "�h���(��)");
	
	item flag1(40, 1, 1, 4, "�t���O1");

	while (1) {
		cv::Mat input;
		capture >> input;

		cv::imshow("input", input);/* �J������蓾��ꂽ�摜input����ʏo��*/
		//frame����}�[�J�[�����o���A�ʒu��corners, ID��ids�Ɋi�[����
		cv::aruco::detectMarkers(input, dictionary, corners, ids);

		//std::regex regex("SYNTH_START\|mei\|mei_voice_normal\|����.*");
		// �}�[�J�[�̓��͂��󂯕t���Ă��邩
		/*if (std::regex_search(mmd_camera->event, regex)) {
			markerActive = true;
			printf("MMDRECEIVED\n");
			printf("%s\n", mmd_camera->event);
		} */

		markerActive = true;
		if (ids.size() > 0 && flag >= 30 && markerActive==true) {
			flag = 0;
			//���o�����}�[�J�[�̈ʒu���͂�
			cv::aruco::drawDetectedMarkers(input, corners, ids);
			//���o�����}�[�J�[�̈ʒu���擾
			cv::Point2f center = corners[0][0];
			
			//���o�����}�[�J�[��id��10~19�̏ꍇ �񕜖�g�p
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
				markerActive = false;
				/*���o�����}�[�J�[�̈ʒu����ʂɕ\��
				cv::putText(input, std::to_string(ids[0]), center, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
				���o�����}�[�J�[�̈ʒu�����L�������Ɋi�[
				mmd_camera->x = center.x;
				mmd_camera->y = center.y;
				mmd_camera->id = ids[0]; */
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

		int key = cv::waitKey(15);/* �L�[���́C���l�͓��͑҂�����(�~���b)*/

		if (key == 'q' || key == 'Q') {   /* �v���O�����I������ */
			break;
		}
	}
	Closeshare(mmd_camera);
	cv::destroyAllWindows();
	return 0;
}