#include "MediaImage.hpp"

using namespace media;


/*--------------------------------
	OpenNI and NiTE
---------------------------------*/
/*
 * Return the depth data
*/
cv::Mat MediaImage::GetDepth()
{
	return depth.clone();
}
/*
 * Return the depth data
*/
cv::Mat MediaImage::GetDepthDist()
{
	return depthDist.clone();
}

void MediaImage::ChangeResolution(openni::VideoStream& stream)
{
	openni::VideoMode mode = stream.getVideoMode();
	mode.setResolution(xtionX, xtionX * 3 / 4);
	mode.setFps(30);
	stream.setVideoMode(mode);
}

void MediaImage::InitOpenNI(int _X)
{
	xtionX = _X;

	//NiTEを初期化する
	nite::NiTE::initialize();
	//ドライバの状態
	rc = openni::STATUS_OK;

	//デバイス情報
	openni::Device device;

	//ANY_DEVICEならPCに接続されているデバイスを読み込む
	const char* deviceURI = openni::ANY_DEVICE;

	//OpenNIの初期化を行う
	rc = openni::OpenNI::initialize();
	printf("OpenNI initialization:\n%s\n", openni::OpenNI::getExtendedError());

	//デバイスを読み込めたかどうか
	rc = device.open(deviceURI);
	if (rc != openni::DEVICE_STATE_OK)
	{
		printf("Viewer: Device open failed:\n%s\n", openni::OpenNI::getExtendedError());
		openni::OpenNI::shutdown();
		throw std::runtime_error("openni::Device::open() failed");
	}

//	userTracker.create();

	//深度情報の初期化を行う
	rc = depthStream.create(device, openni::SENSOR_DEPTH);
	if (rc == openni::STATUS_OK)
	{
		ChangeResolution(depthStream);
		rc = depthStream.start();
		if (rc != openni::STATUS_OK)
		{
			printf("Viewer: Couldn't start depth stream:\n%s\n", openni::OpenNI::getExtendedError());
			depthStream.destroy();
			throw std::runtime_error("Couldn't find depth stream failed");
		}
	}
	else
	{
		printf("Viewer: Couldn't find depth stream:\n%s\n", openni::OpenNI::getExtendedError());
		throw std::runtime_error("Couldn't find depth stream failed");
	}

	//カラー画像の初期化を行う
	rc = colorStream.create(device, openni::SENSOR_COLOR);
	if (rc == openni::STATUS_OK)
	{
		ChangeResolution(colorStream);
		rc = colorStream.start();
		if (rc != openni::STATUS_OK)
		{
			printf("Viewer: Couldn't start color stream:\n%s\n", openni::OpenNI::getExtendedError());
			colorStream.destroy();
			throw std::runtime_error("Couldn't start color stream failed");
		}
	}
	else
	{
		printf("SimpleViewer: Couldn't find color stream:\n%s\n", openni::OpenNI::getExtendedError());
		throw std::runtime_error("Couldn't find color stream failed");
	}

	//skeletonFlag = false;
	userTracker.create();


	//骨格情報を取得する変数の初期化
	nite::Point3f p(0.0, 0.0, 0.0);

	//初期化を行う
	std::vector<nite::Point3f> v(15);
	for (int i = 0; i < v.size(); ++i)
		v[i] = p;
	for (int i = 0; i < v.size(); ++i)
		userSkeletonPosition.push_back(v);
}

/* Update the OpenNI and NiTE
*-------------------------------*/
void MediaImage::UpDate()
{

	//更新されたフレームの取得
	colorStream.readFrame(&colorFrame);
	depthStream.readFrame(&depthFrame);

	//フレームデータから画像データへ変換する
	ChangeColorStream(colorFrame, image);

	//深度画像を表示できるようにする
	ChangeDepthStream(depthFrame, depth);

	userTracker.readFrame(&userFrame);

	
//	ChangeUserTrackerStream(userFrame, depth);
}


/* Convert the color stream to the opencv type.
* input: the color stream
* output: the matrix data
* ------------------------------------------*/
void MediaImage::ChangeColorStream(const openni::VideoFrameRef &videoFrame,
	cv::Mat &image)
{
	image = cv::Mat(
		videoFrame.getHeight(),
		videoFrame.getWidth(),
		CV_8UC3,
		(unsigned char*)videoFrame.getData());

	cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
}

/* Convert the depth stream to the opencv type.
 * Convert the 0 - 10,000 meter to 8 bit.
 * input: the depth stream
 * output: the matrix data
* ------------------------------------------*/
void MediaImage::ChangeDepthStream(const openni::VideoFrameRef &videoFrame,
	cv::Mat &image)
{
	image = cv::Mat(
		videoFrame.getHeight(),
		videoFrame.getWidth(),
		CV_16UC1,
		(unsigned char*)videoFrame.getData());
		
	image.convertTo(image, CV_8U, 255.0 / 10000);

	depthDist = image.clone();

	//image = 255 - image;
}

/* Draw the skeleton information on the image.
 *-------------------------------*/
void MediaImage::CreateSkeleton()
{
	CreateSkeleton(image);
}
void MediaImage::CreateSkeleton(cv::Mat& image)
{
	const nite::Array<nite::UserData> &users = userFrame.getUsers();

	for(int i =0; i < userCheck.size(); ++i)
		userCheck[i] = false;

	for (int i = 0; i < users.getSize(); ++i)
	{
		const nite::UserData& user = users[i];

		//新しく検出したユーザなら、トラッキング開始
		if (user.isNew())
		{
			userTracker.startSkeletonTracking(user.getId());
		}

		//すでに検出したユーザの処理
		//消失していないならスケルトン表示
		if (!user.isLost())
		{
			DrawSkeleton(image, userTracker, user);
			DrawSkeletonPoint(image, userTracker, user);
		}

		GetUserSkeletionPosition(userFrame.getDepthFrame(), userTracker, user, i);
	}
}

/* Get the depth data.
 * input: the depth stream
 * output: the depth distane
 * ------------------------------------------*/
template<typename T>
void MediaImage::GetDepthData(const openni::VideoFrameRef &videoFrame,
	const T x, const T y, T &depthDist)
{
	openni::VideoMode videMode = videoFrame.getVideoMode();
	int position = (y * videMode.getResolutionX()) + x;

	unsigned short* depth = (unsigned short*)videoFrame.getData();

	if (x >= 0.0 && x < videMode.getResolutionX() && y>=0.0 && y < videMode.getResolutionY()) {
		depthDist = depth[position];
	}
	else {
		depthDist = 0;
	}
}
void MediaImage::GetDepthData(const openni::VideoFrameRef &videoFrame,
	const cv::Point2i &point, int &depthDist)
{
	GetDepthData(videoFrame, point.x, point.y, depthDist);
}

//NiTE functions 
/* Convert the depth stream to the opencv type.
 * Convert the 0 - 10,000 meter to 8 bit.
 * input: the depth stream
 * output: the matrix data
 * ------------------------------------------*/
void MediaImage::ChangeUserTrackerStream(nite::UserTrackerFrameRef userTracke,
	cv::Mat &image)
{
	openni::VideoFrameRef depthFrame = userTracke.getDepthFrame();

	if (depthFrame.isValid())
	{
		//ChangeDepthStream(depthFrame, image);

		image = cv::Mat(depthFrame.getHeight(),
			depthFrame.getWidth(),
			CV_8UC4);
	}
}

/* Get the skeleton point of user.
 * input: user information
 * output: user position
 * ------------------------------------------*/
void MediaImage::GetUserSkeletionPosition(
	const openni::VideoFrameRef &videoFrame,
	nite::UserTracker &userTracker,
	const nite::UserData user,
	nite::Point3f *userPosition)
{
	//スケルトンを取得し、追跡状態を確認
	const nite::Skeleton &skeleton = user.getSkeleton();
	if (skeleton.getState() != nite::SKELETON_TRACKED)
	{
		return;
	}

	//すべての関節を描画
	for (int j = 0; j < 15; ++j)
	{
		//信頼値（70%）以上なら処理する
		const nite::SkeletonJoint& joint = skeleton.getJoint((nite::JointType)j);
		if (joint.getPositionConfidence() < 0.7f)
		{
			continue;
		}

		//3次元を2次元に変換
		float fx = 0.0f, fy = 0.0f, fz = 0.0f;
		const nite::Point3f& position = joint.getPosition();
		userTracker.convertJointCoordinatesToDepth(
			position.x,
			position.y,
			position.z,
			&fx,
			&fy);

		userPosition[j].x = fx;
		userPosition[j].y = fy;
		GetDepthData(videoFrame, fx, fy, userPosition[j].z);
	}
}

void MediaImage::GetUserSkeletionPosition(
	const openni::VideoFrameRef &videoFrame,
	nite::UserTracker &userTracker,
	const nite::UserData user,
	int userID)
{
	//スケルトンを取得し、追跡状態を確認
	const nite::Skeleton &skeleton = user.getSkeleton();
	if (skeleton.getState() != nite::SKELETON_TRACKED)
	{
		return;
	}

	//すべての関節を描画
	for (int j = 0; j < 15; ++j)
	{
		//信頼値（70%）以上なら処理する
		const nite::SkeletonJoint& joint = skeleton.getJoint((nite::JointType)j);
		if (joint.getPositionConfidence() < 0.7f)
		{
			continue;
		}

		//3次元を2次元に変換
		float fx = 0.0f, fy = 0.0f, fz = 0.0f;
		const nite::Point3f& position = joint.getPosition();
		userTracker.convertJointCoordinatesToDepth(
			position.x,
			position.y,
			position.z,
			&fx,
			&fy);

		//userPositions[userID][j].x = fx;
		//userPositions[userID][j].y = fy;
		//GetDepthData(videoFrame, fx, fy, userPositions[userID][j].z);

		//New
		userSkeletonPosition[userID][j].x = fx;
		userSkeletonPosition[userID][j].y = fy;
		GetDepthData(videoFrame, fx, fy, userSkeletonPosition[userID][j].z);
		userCheck[userID] = true;
	}	
}


/* -------------------------------------------
 * Draw the skeleton in the image.
 * input: the color stream.
 * output: image data.
 * ------------------------------------------*/
void MediaImage::DrawSkeletonPoint(
	cv::Mat &image,
	nite::UserTracker &userTracker,
	const nite::UserData user)
{
	//スケルトンを取得し、追跡状態を確認
	const nite::Skeleton &skeleton = user.getSkeleton();
	if (skeleton.getState() != nite::SKELETON_TRACKED)
	{
		return;
	}

	std::vector < cv::Scalar > color;
	color.push_back(cv::Scalar(0, 0, 255));
	color.push_back(cv::Scalar(0, 0, 128));
	color.push_back(cv::Scalar(0, 0, 64));
	color.push_back(cv::Scalar(0, 255, 0));
	color.push_back(cv::Scalar(0, 128, 0));
	color.push_back(cv::Scalar(0, 64, 0));
	color.push_back(cv::Scalar(255, 0, 0));
	color.push_back(cv::Scalar(128, 0, 0));
	color.push_back(cv::Scalar(64, 0, 0));
	color.push_back(cv::Scalar(255, 0, 255));
	color.push_back(cv::Scalar(128, 0, 128));
	color.push_back(cv::Scalar(64, 0, 64));
	color.push_back(cv::Scalar(0, 255, 255));
	color.push_back(cv::Scalar(0, 128, 128));
	color.push_back(cv::Scalar(0, 64, 64));



	//すべての関節を描画
	for (int j = 0; j < 15; ++j)
	{
		//信頼値（70%）以上なら処理する
		const nite::SkeletonJoint& joint = skeleton.getJoint((nite::JointType)j);
		if (joint.getPositionConfidence() < 0.7f)
		{
			continue;
		}

		//3次元を2次元に変換
		float fx = 0.0f, fy = 0.0f;
		const nite::Point3f& position = joint.getPosition();
		userTracker.convertJointCoordinatesToDepth(
			position.x,
			position.y,
			position.z,
			&fx,
			&fy);

		cv::circle(image, cv::Point(fx, fy), 5, color[j], -1);
	}
}

/* Draw the skeletion line.
 * ------------------------------------------*/
void MediaImage::DrawLine(const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2,
	const nite::UserTracker &userTracker, cv::Mat &image)
{
	if (joint1.getPositionConfidence() < 0.7f ||
		joint2.getPositionConfidence() < 0.7f)
	{
		return;
	}

	//3次元を2次元に変換
	cv::Point2f f1, f2;
	const nite::Point3f& position1 = joint1.getPosition();
	userTracker.convertJointCoordinatesToDepth(
		position1.x, position1.y, position1.z,
		&f1.x, &f1.y);

	const nite::Point3f& position2 = joint2.getPosition();
	userTracker.convertJointCoordinatesToDepth(
		position2.x, position2.y, position2.z,
		&f2.x, &f2.y);

	cv::line(image, f1, f2, cv::Scalar(0, 255, 0), 2, cv::LINE_AA, 0);
}
/* Draw the skeletion.
 * ------------------------------------------*/
void MediaImage::DrawSkeleton(
	cv::Mat &image,
	nite::UserTracker &userTracker,
	const nite::UserData user)
{
	//スケルトンを取得し、追跡状態を確認
	const nite::Skeleton &skeleton = user.getSkeleton();
	if (skeleton.getState() != nite::SKELETON_TRACKED)
	{
		return;
	}

	DrawLine(skeleton.getJoint(nite::JOINT_HEAD), skeleton.getJoint(nite::JOINT_NECK), userTracker, image);

	DrawLine(skeleton.getJoint(nite::JOINT_NECK), skeleton.getJoint(nite::JOINT_LEFT_SHOULDER), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_NECK), skeleton.getJoint(nite::JOINT_RIGHT_SHOULDER), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_RIGHT_SHOULDER), skeleton.getJoint(nite::JOINT_TORSO), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_LEFT_SHOULDER), skeleton.getJoint(nite::JOINT_TORSO), userTracker, image);

	DrawLine(skeleton.getJoint(nite::JOINT_LEFT_SHOULDER), skeleton.getJoint(nite::JOINT_LEFT_ELBOW), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_LEFT_ELBOW), skeleton.getJoint(nite::JOINT_LEFT_HAND), userTracker, image);

	DrawLine(skeleton.getJoint(nite::JOINT_RIGHT_SHOULDER), skeleton.getJoint(nite::JOINT_RIGHT_ELBOW), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_RIGHT_ELBOW), skeleton.getJoint(nite::JOINT_RIGHT_HAND), userTracker, image);

	DrawLine(skeleton.getJoint(nite::JOINT_TORSO), skeleton.getJoint(nite::JOINT_LEFT_HIP), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_TORSO), skeleton.getJoint(nite::JOINT_RIGHT_HIP), userTracker, image);

	DrawLine(skeleton.getJoint(nite::JOINT_LEFT_HIP), skeleton.getJoint(nite::JOINT_LEFT_KNEE), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_LEFT_KNEE), skeleton.getJoint(nite::JOINT_LEFT_FOOT), userTracker, image);

	DrawLine(skeleton.getJoint(nite::JOINT_RIGHT_HIP), skeleton.getJoint(nite::JOINT_RIGHT_KNEE), userTracker, image);
	DrawLine(skeleton.getJoint(nite::JOINT_RIGHT_KNEE), skeleton.getJoint(nite::JOINT_RIGHT_FOOT), userTracker, image);
}

/* Get the user points.
 * output: the user points
 * ------------------------------------------*/
//nite::Point3f **MediaImage::GetUserPoint()
//{
//	return userPositions;
//}


/* Get the user points.
 * output: the user points
 * ------------------------------------------*/
std::vector<std::vector<nite::Point3f> > MediaImage::GetUserPoint()
{
	return userSkeletonPosition;
}

std::vector<bool> MediaImage::CheckUser()
{
	return userCheck;
}


/**
* @brief
*   Logデータの初期化
*/
void MediaImage::ResetActionLog()
{
	for (int i = 0; i < 15; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			gestureLog[i][j] = END;
		}
	}
}

/**
* @brief
*   行動認識　第1引数：定義した行動モデル，　第2引数:行動ログ
*/
bool MediaImage::AnalysisActionModel(const std::vector<MediaGesture> recogAction, const int userNum)
{
	int i, j;
	//定義した
	for (i = 0; i < ACTION_LENGTH; ++i)
	{
		if (recogAction[i] == END)
			break;
	}
	int recogLength = i;

	for (i = 0; i < ACTION_LENGTH; ++i)
	{
		if (gestureLog[userNum][i] == END)
			break;
	}
	int logLength = i;

	//ログがたまっていなければ無視する
	if (recogLength > logLength)
		return false;

	for (i = 0; i <= logLength - recogLength; ++i)
	{
		for (j = 0; j <= recogLength; ++j)
		{
			//最後まで行けばTRUEを返却
			if (recogAction[j] == END)
			{
				return true;
			}

			if (recogAction[j] != gestureLog[userNum][j + i])
			{
				break;
			}
		}
	}

	return false;
}

/**
* @brief
*   OpenNIの初期化関数
*/
bool MediaImage::SetActionLog(const MediaGesture gesture, const int userNum)
{
	int i = 0;
	//格納箇所の指定
	for (i = 0; i < ACTION_LENGTH; ++i)
	{
		if (gestureLog[userNum][i] == END)
		{
			break;
		}
	}

	//前回と同じなら無視する
	if (i >= 1)
	{
		if (gestureLog[userNum][i - 1] == gesture)
		{
			return false;
		}
	}

	//格納箇所がない場合は古いデータから捨てる
	if (i == ACTION_LENGTH)
	{
		for (int j = 0; j < ACTION_LENGTH - 1; j++)
		{
			gestureLog[userNum][j] = gestureLog[userNum][j + 1];
		}
		i -= 1;
	}

	gestureLog[userNum][i] = gesture;

	return true;
}

bool MediaImage::PleaseGesture(const MediaGesture gesuture, const int timeout, const int userNum)
{
	const std::string s = "CheckGesture";
	std::cerr << "動作：[" << GestureName[gesuture] << "]が検出されるまで待機\n";

	if (timeout >= 0)
	{
		std::cerr << "設定時間:" << timeout << "[s]\n";
	}

	DWORD start = timeGetTime();
	DWORD end = timeGetTime();
	
	int old = -1;
	while (gesuture != GetGestures(userNum) && (timeout == -1 ? 1 : timeout - ((int)(end - start) / 1000) > 0))
	{
		UpDate();

		CreateSkeleton();
		ShowImage(s);
		cv::waitKey(1);
		end = timeGetTime();

		if (timeout != -1 && old != (timeout - ((int)(end - start) / 1000)))
		{
			std::cerr << "   " << timeout - ((int)(end - start) / 1000) << "\n";
			old = (timeout - ((int)(end - start) / 1000));
		}
	}

	ReleaseWindow(s);
	
	if (timeout != -1 && timeout - ((int)(end - start) / 1000) > 0)
	{
		return false;
	}

	return true;
}

bool MediaImage::PleaseAction(const MediaAction action, const int timeout, const int userNum)
{
	const std::string s = "CheckAction";
	std::cerr << "行動：[" << ActionName[action] << "]が検出されるまで待機\n";

	if (timeout >= 0)
	{
		std::cerr << "設定時間:" << timeout << "[s]\n";
	}

	DWORD start = timeGetTime();
	DWORD end = timeGetTime();

	int old = -1;
	while (action != GetAction(userNum) && (timeout == -1 ? 1 : timeout - ((int)(end - start) / 1000) > 0))
	{
		UpDate();

		CreateSkeleton();
		ShowImage(s);

		GetGestures();

		cv::waitKey(1);
		end = timeGetTime();

		if (timeout != -1 && old != (timeout - ((int)(end - start) / 1000)))
		{
			std::cerr << "   " << timeout - ((int)(end - start) / 1000) << "\n";
			old = (timeout - ((int)(end - start) / 1000));
		}
	}

	ReleaseWindow(s);

	if (timeout != -1 && timeout - ((int)(end - start) / 1000) > 0)
	{
		return false;
	}

	return true;
}