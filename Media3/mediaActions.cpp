#include "MediaImage.hpp"

using namespace media;

/*---ジェスチャー認識----------------------------------------

以下に認識させたいジェスチャーを条件によって記述する．
それぞれの座標を指定するパラメータは下にまとめているので参考にすること
また座標を扱う際は左上が原点であり右によるほどXが増加．
下にいくほどYが増加する点に注意して設定すること．

----------------------------------------------------------------------------*/
//ジェスチャーを登録する
// 「NO_GESTURE」ならジェスチャー認識失敗
MediaGesture MediaImage::GetGestures(const int userNum)
{
	if (userNum < 0 && userNum > 15)
	{
		std::cerr << "ユーザは0～14人までです\n";
		return END;
	}

	//両手を挙げる
	if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y &&
		userSkeletonPosition[userNum][nite::JOINT_RIGHT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y)
	{
		std::cerr << "G: 両手を挙げています。\n";
		SetActionLog(BOTH_HAND, userNum);
		return BOTH_HAND;
	}

	//頭より上に手がある
	if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y)
	{
		//左手が左ひじよりも右
		if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].x > userSkeletonPosition[userNum][nite::JOINT_LEFT_ELBOW].x + 5)
		{
			std::cerr << "G: 左手を挙げています_1\n";
			SetActionLog(LEFT_HAND_UP_1, userNum);
			return LEFT_HAND_UP_1;
		}
	}

	//頭より上に手がある
	if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].y < userSkeletonPosition[userNum][nite::JOINT_HEAD].y)
	{
		//左手が左ひじよりも左
		if (userSkeletonPosition[userNum][nite::JOINT_LEFT_HAND].x < userSkeletonPosition[userNum][nite::JOINT_LEFT_ELBOW].x - 5)
		{
			std::cerr << "G: 左手を挙げています_2\n";
			SetActionLog(LEFT_HAND_UP_2, userNum);
			return LEFT_HAND_UP_2;
		}
	}

	/*-------------------------- 課題6 ここから-----------------------------*/





	/*-------------------------- 課題6 ここまで-----------------------------*/

	//ジェスチャーなし
	SetActionLog(NO_GESTURE, userNum);
	return NO_GESTURE;
}
//------------------------------------------
//    ↑    ↑    ↑    ↑    ↑    ↑    ↑
//　　　　　　　ここまでに記述
//------------------------------------------




/*--- 行 動 認 識 ------------------------------------------

以下に認識させたい行動を条件によって記述する．

-----------------------------------------------------------*/
MediaAction MediaImage::GetAction(const int userNum)
{
	if (userNum < 0 && userNum > 15)
	{
		std::cerr << "引数には0～14の値を入力してください。\n";
		return NO_ACTION;
	}

	GetGestures(userNum);

	//行動の定義
	//バイバイ
	std::vector<MediaGesture> byebye1 = { LEFT_HAND_UP_1, LEFT_HAND_UP_2, LEFT_HAND_UP_1, LEFT_HAND_UP_2, END };
	std::vector<MediaGesture> byebye2 = { LEFT_HAND_UP_2, LEFT_HAND_UP_1, LEFT_HAND_UP_2, LEFT_HAND_UP_1, END };

	//バンザイ
	std::vector<MediaGesture> banzai = { BOTH_HAND, NO_GESTURE, BOTH_HAND, NO_GESTURE, END };

	/*-------------------------- 課題8 ここから-----------------------------*/


	/*-------------------------- 課題8 ここまで-----------------------------*/

	if (AnalysisActionModel(byebye1, userNum) || AnalysisActionModel(byebye2, userNum))
	{
		std::cerr << "バイバイ！！\n";

		//一度，行動検出に成功したら，ログを廃棄
		ResetActionLog();
		return BYE_BYE;
	}

	if (AnalysisActionModel(banzai, userNum))
	{
		std::cerr << "バンザイ！！\n";

		//一度，行動検出に成功したら，ログを廃棄
		ResetActionLog();
		return BANZAI;
	}

	/*-------------------------- 課題8 ここから-----------------------------*/


	/*-------------------------- 課題8 ここまで-----------------------------*/
	return NO_ACTION;
}
//------------------------------------------
//    ↑    ↑    ↑    ↑    ↑    ↑    ↑
//　　　　　　　ここまでに記述
//------------------------------------------