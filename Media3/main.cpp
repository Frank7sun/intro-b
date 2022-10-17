#include "MediaImage.hpp"
#include "aruco/include/opencv2/aruco.hpp"
#include "sharemem.h"
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



int main(void)
{
	SetConsoleOutputCP(CP_UTF8);/*�R���\�[����UTF-8�ɂ���*/
	setvbuf(stdout, nullptr, _IOFBF, 2); /* �o�b�t�@�����O�ɕK�v */
	mmd_camera = Getshare();/* ���L�������̎擾 */

	MediaImage mediaImage = MediaImage();
	/**************���ύX���Ȃ�*******************/

	/**************���K�X�ύX����*******************/

	cv::VideoCapture capture = cv::VideoCapture(5);/* �J������2�䂠��ꍇ�͐��l�Ŏw�� */
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

	bool flag = false;

	//�񕜃A�C�e��
	item heal1(10, 3, 1, 1, "�񕜖�(��)");

	while (1) {
		cv::Mat input;
		capture >> input;

		cv::imshow("input", input);/* �J������蓾��ꂽ�摜input����ʏo��*/
		//frame����}�[�J�[�����o���A�ʒu��corners, ID��ids�Ɋi�[����
		cv::aruco::detectMarkers(input, dictionary, corners, ids);
		// ���o�����}�[�J�[�̏ꏊ���͂�ID��t����frame�Ɋi�[����
		if (ids.size() > 0 && !flag) {
			flag = true;
			//���o�����}�[�J�[��id��10~19�̏ꍇ �񕜖�g�p
			if (ids[0] >= 10 && ids[0] <= 19) {
				//���o�����}�[�J�[�̈ʒu���͂�
				cv::aruco::drawDetectedMarkers(input, corners, ids);
				//���o�����}�[�J�[�̈ʒu���擾
				cv::Point2f center = corners[0][0];
				//�񕜃A�C�e����amount���P�ȏ�ł��邩
				if (heal1.amount >= 1) {
					//mmd�Ƀ��b�Z�[�W�𑗂�
					sprintf_s(mmd_camera->camera, "item_%d", ids[0]);
					printf("item_used: %d\nleft: %d\n", ids[0], heal1.amount);
					//item��amount�����炷
					heal1.amount--;
				}
				else {
					printf("EMPTY_ITEM %d\n", ids[0]);
				}
				/*���o�����}�[�J�[�̈ʒu����ʂɕ\��
				cv::putText(input, std::to_string(ids[0]), center, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
				���o�����}�[�J�[�̈ʒu�����L�������Ɋi�[
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

		int key = cv::waitKey(15);/* �L�[���́C���l�͓��͑҂�����(�~���b)*/

		if (key == 'q' || key == 'Q') {   /* �v���O�����I������ */
			break;
		}
	}
	Closeshare(mmd_camera);
	cv::destroyAllWindows();
	return 0;
}