#include "MediaImage.hpp"

using namespace media;

/*---�W�F�X�`���[�F��----------------------------------------

�ȉ��ɔF�����������W�F�X�`���[�������ɂ���ċL�q����D
���ꂼ��̍��W���w�肷��p�����[�^�͉��ɂ܂Ƃ߂Ă���̂ŎQ�l�ɂ��邱��
�܂����W�������ۂ͍��オ���_�ł���E�ɂ��ق�X�������D
���ɂ����ق�Y����������_�ɒ��ӂ��Đݒ肷�邱�ƁD

----------------------------------------------------------------------------*/
//�W�F�X�`���[��o�^����
// �uNO_GESTURE�v�Ȃ�W�F�X�`���[�F�����s
MediaGesture MediaImage::GetGestures(const int userNum)
{
	if (userNum < 0 && userNum > 15)
	{
		std::cerr << "���[�U��0�`14�l�܂łł�\n";
		return END;
	}

	//�����������
	if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y &&
		userSkeletonPosition[userNum][nite::JOINT_RIGHT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y)
	{
		std::cerr << "G: ����������Ă��܂��B\n";
		SetActionLog(BOTH_HAND, userNum);
		return BOTH_HAND;
	}

	//������Ɏ肪����
	if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y)
	{
		//���肪���Ђ������E
		if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].x > userSkeletonPosition[userNum][nite::JOINT_LEFT_ELBOW].x + 5)
		{
			std::cerr << "G: ����������Ă��܂�_1\n";
			SetActionLog(LEFT_HAND_UP_1, userNum);
			return LEFT_HAND_UP_1;
		}
	}

	//������Ɏ肪����
	if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y)
	{
		//���肪���Ђ�������
		if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].x < userSkeletonPosition[userNum][nite::JOINT_LEFT_ELBOW].x - 5)
		{
			std::cerr << "G: ����������Ă��܂�_2\n";
			SetActionLog(LEFT_HAND_UP_2, userNum);
			return LEFT_HAND_UP_2;
		}
	}

	/*-------------------------- �ۑ�6 ��������-----------------------------*/





	/*-------------------------- �ۑ�6 �����܂�-----------------------------*/

	//�W�F�X�`���[�Ȃ�
	SetActionLog(NO_GESTURE, userNum);
	return NO_GESTURE;
}
//------------------------------------------
//    ��    ��    ��    ��    ��    ��    ��
//�@�@�@�@�@�@�@�����܂łɋL�q
//------------------------------------------




/*--- �s �� �F �� ------------------------------------------

�ȉ��ɔF�����������s���������ɂ���ċL�q����D

-----------------------------------------------------------*/
MediaAction MediaImage::GetAction(const int userNum)
{
	if (userNum < 0 && userNum > 15)
	{
		std::cerr << "�����ɂ�0�`14�̒l����͂��Ă��������B\n";
		return NO_ACTION;
	}

	GetGestures(userNum);

	//�s���̒�`
	//�o�C�o�C
	std::vector<MediaGesture> byebye1 = { LEFT_HAND_UP_1, LEFT_HAND_UP_2, LEFT_HAND_UP_1, LEFT_HAND_UP_2, END };
	std::vector<MediaGesture> byebye2 = { LEFT_HAND_UP_2, LEFT_HAND_UP_1, LEFT_HAND_UP_2, LEFT_HAND_UP_1, END };

	//�o���U�C
	std::vector<MediaGesture> banzai = { BOTH_HAND, NO_GESTURE, BOTH_HAND, NO_GESTURE, END };

	/*-------------------------- �ۑ�8 ��������-----------------------------*/


	/*-------------------------- �ۑ�8 �����܂�-----------------------------*/

	if (AnalysisActionModel(byebye1, userNum) || AnalysisActionModel(byebye2, userNum))
	{
		std::cerr << "�o�C�o�C�I�I\n";

		//��x�C�s�����o�ɐ���������C���O��p��
		ResetActionLog();
		return BYE_BYE;
	}

	if (AnalysisActionModel(banzai, userNum))
	{
		std::cerr << "�o���U�C�I�I\n";

		//��x�C�s�����o�ɐ���������C���O��p��
		ResetActionLog();
		return BANZAI;
	}

	/*-------------------------- �ۑ�8 ��������-----------------------------*/


	/*-------------------------- �ۑ�8 �����܂�-----------------------------*/
	return NO_ACTION;
}
//------------------------------------------
//    ��    ��    ��    ��    ��    ��    ��
//�@�@�@�@�@�@�@�����܂łɋL�q
//------------------------------------------