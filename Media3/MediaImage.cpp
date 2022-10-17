#include "MediaImage.hpp"

using namespace media;

void MediaImage::ColorInit(const int num)
{
	if (num < 0)
	{
		colors.push_back(cv::Vec3b(255, 0, 0));
		colors.push_back(cv::Vec3b(255, 125, 0));
		colors.push_back(cv::Vec3b(255, 255, 0));
		colors.push_back(cv::Vec3b(125, 125, 0));
		colors.push_back(cv::Vec3b(125, 255, 0));
		colors.push_back(cv::Vec3b(0, 255, 0));
		colors.push_back(cv::Vec3b(0, 255, 125));
		colors.push_back(cv::Vec3b(0, 255, 255));
		colors.push_back(cv::Vec3b(0, 125, 255));
		colors.push_back(cv::Vec3b(0, 0, 255));
		colors.push_back(cv::Vec3b(0, 125, 125));
		colors.push_back(cv::Vec3b(125, 0, 125));
		colors.push_back(cv::Vec3b(255, 0, 125));
		colors.push_back(cv::Vec3b(125, 0, 255));
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			colors.push_back(cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255)));
		}
	}
}

/*
 * Get the color list
 * @output: the number of list
*/
std::vector<cv::Vec3b> MediaImage::GetColor()
{
	return colors;
}

MediaImage::MediaImage()
{
	Init_Vector(colors);
	ColorInit();

	//Hog initalize
	N_BIN = 9;
	HoG_THETA = (180 / N_BIN);

	CELL_SIZE = 20;
	BLOCK_SIZE = 3;
	R = CELL_SIZE * BLOCK_SIZE * 0.5;
}
MediaImage::MediaImage(const MediaImage& m)
{
	std::cout << "copy constructor called.\n";
}
MediaImage::~MediaImage()
{

}

//画像のチェック
bool MediaImage::CheckImage(cv::Mat &image)
{
	if (image.empty())
		return false;
	return true;
}


/**
* @brief
*   内部変数への読み込み
*/
void MediaImage::ReadImage(const std::string path)
{
	image = cv::imread(path, -1);

	if (!CheckImage(image))
	{
		ErrorLog("Not image file found");
		throw std::runtime_error("Not read the image");
	}
}

/**
* @brief
*   指定変数への読み込み
*/
void MediaImage::ReadImage(const std::string path, cv::Mat& image)
{
	image = cv::imread(path, -1);
	if (!CheckImage(image))
	{
		ErrorLog("Not image file found");
		throw std::runtime_error("Not read the image");
	}
}

/**
* @brief
*   画像を表示する
*/
void MediaImage::ShowImage(const std::string caption)
{
	if (!CheckImage(image))
	{
		ErrorLog("");
		throw std::runtime_error("Not found the image data");
	}

	cv::namedWindow(caption, cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
	cv::imshow(caption, image);
	cv::waitKey(1);
}

/**
* @brief
*   画像を表示する
*/
void MediaImage::ShowImage(const std::string caption, cv::Mat &image)
{
	if (!CheckImage(image))
	{
		ErrorLog("");
		throw std::runtime_error("not image data");
	}

	cv::namedWindow(caption, cv::WINDOW_AUTOSIZE | cv::WINDOW_FREERATIO);
	cv::imshow(caption, image);
	cv::waitKey(1);
}

/**
* @brief
*   ベクトル配列の画像を表示する
*/
void MediaImage::ShowImage(const std::string caption, std::vector<cv::Mat> &image)
{
	//view center_surround_image
	for (int i = 0; i < image.size(); i++)
	{
		std::stringstream ss;
		ss << i;

		ShowImage(caption + "_" + ss.str(), image[i]);
		cv::waitKey(1);
	}
}

/**
* @brief
*   メモリ解放
*/
void MediaImage::ReleaseImage()
{
	if (!CheckImage(image))
	{
		image.release();
	}
}
void MediaImage::ReleaseImage(cv::Mat &image)
{
	if (!CheckImage(image))
	{
		image.release();
	}
}
void MediaImage::ReleaseImage(std::vector<cv::Mat> &image)
{
	for (int i = 0; i < image.size(); i++)
	{
		ReleaseImage(image[i]);
	}
}
template<typename T>
void MediaImage::ReleaseImage(std::vector<cv::Mat_<T>> &image)
{
	for (int i = 0; i < image.size(); i++)
	{
		ReleaseImage(image[i]);
	}
}

/*
 * Release the image data
 * @input: window caption
*/
void MediaImage::ReleaseWindow()
{
	cv::destroyAllWindows();
}
void MediaImage::ReleaseWindow(const std::string caption)
{
	cv::destroyWindow(caption);
}

/*
 * Release the image data
 * @input: image data type
 * @input: the name of written image
 * @input: the format of written image (default JPEG)
*/
void MediaImage::SaveImage(const std::string caption, const std::string format)
{
	SaveImage(image, caption, format);
}
void MediaImage::SaveImage(cv::Mat &image, const std::string caption, const std::string format)
{
	if (!CheckImage(image))
	{
		ErrorLog("");
		throw std::runtime_error("not image data");
	}
	cv::imwrite(caption + format, image);
}
void MediaImage::SaveImage(std::vector<cv::Mat> &image, const std::string caption, const std::string format)
{
	//view center_surround_image
	for (int i = 0; i < image.size(); i++)
	{
		std::stringstream ss;
		ss << i;

		SaveImage(image[i], caption + "_" + ss.str(), format);
	}
}

/*
 * Move the window
 * @input: the window caption
 * @input: the x point (default 0)
 * @input: the y point (default 0)
*/
void MediaImage::MoveWindow(const std::string caption, const int x, const int y)
{
	cv::moveWindow(caption, x, y);
}

/**
* @brief
*   画像の取得
*/
cv::Mat MediaImage::GetImage()
{
	return image.clone();
}

/**
* @brief
*   通常のWEBカメラを使用
*/
void MediaImage::InitVideo(const int camera_num,
	const int width, const int height)
{
	std::cerr << "Web Camera Initalize...";
	capture = cv::VideoCapture(camera_num);

	//カメラが開けなかった場合
	if (!capture.isOpened())
	{
		ErrorLog("");
		throw std::runtime_error("Not found camera");
	}

	capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);

//	cemara_flag = false;
	capture >> image;
	std::cerr << "OK\n";
}

//映像データを読み込む
void MediaImage::InitVideo(const std::string videoName)
{
	std::cerr << "Web Camera Initalize...";
	capture = cv::VideoCapture(videoName);

	//カメラが開けなかった場合
	if (!capture.isOpened())
	{
		ErrorLog("");
		throw std::runtime_error("Not found video file");
	}

	//	cemara_flag = false;
	capture >> image;
	std::cerr << "OK\n";
}

//ビデオカメラのフレームを更新する
void MediaImage::UpdateVideo()
{
	capture >> image;
}


/**
* @brief
*   色抽出関数(元データを書き換え)
*/
void MediaImage::ExtractColor(
	const int ch1_lower, const int ch1_upper,
	const int ch2_lower, const int ch2_upper,
	const int ch3_lower, const int ch3_upper,
	cv::Mat& result)
{
	ExtractColor(ch1_lower, ch1_upper,
		ch2_lower, ch2_upper,
		ch3_lower, ch3_upper,
		image, result);
}
void MediaImage::ExtractColor(
	const int ch1_lower, const int ch1_upper,
	const int ch2_lower, const int ch2_upper,
	const int ch3_lower, const int ch3_upper,
	cv::Mat &inImage, cv::Mat &result)
{
	result = inImage.clone();

	cv::Mat hsv;
	cv::cvtColor(inImage, hsv, cv::COLOR_BGR2HSV);

	for (int i = 0; i < hsv.rows; i++)
	{
		for (int j = 0; j < hsv.cols; j++)
		{
			float h = (float)hsv.data[i * hsv.step + j * hsv.elemSize() + 0];
			float s = (float)hsv.data[i * hsv.step + j * hsv.elemSize() + 1];
			float v = (float)hsv.data[i * hsv.step + j * hsv.elemSize() + 2];

			//Hue
			if (ch1_lower <= ch1_upper)
			{
				if (!(ch1_lower <= h && h <= ch1_upper)) {
					result.data[i * result.step + j * result.elemSize() + 0] *= 0;
					result.data[i * result.step + j * result.elemSize() + 1] *= 0;
					result.data[i * result.step + j * result.elemSize() + 2] *= 0;
				}

			}
			else {
				if (ch1_upper <= h && h <= ch1_lower)
				{
					result.data[i * result.step + j * result.elemSize() + 0] *= 0;
					result.data[i * result.step + j * result.elemSize() + 1] *= 0;
					result.data[i * result.step + j * result.elemSize() + 2] *= 0;
				}
			}

			//Saluation
			if (ch2_lower <= ch2_upper)
			{
				if (!(ch2_lower <= s && s <= ch2_upper)) {
					result.data[i * result.step + j * result.elemSize() + 0] *= 0;
					result.data[i * result.step + j * result.elemSize() + 1] *= 0;
					result.data[i * result.step + j * result.elemSize() + 2] *= 0;
				}

			}
			else {
				if (ch2_upper <= s && s <= ch2_lower)
				{
					result.data[i * result.step + j * result.elemSize() + 0] *= 0;
					result.data[i * result.step + j * result.elemSize() + 1] *= 0;
					result.data[i * result.step + j * result.elemSize() + 2] *= 0;
				}
			}

			//Value
			if (ch3_lower <= ch3_upper)
			{
				if (!(ch3_lower <= v && v <= ch3_upper)) {
					result.data[i * result.step + j * result.elemSize() + 0] *= 0;
					result.data[i * result.step + j * result.elemSize() + 1] *= 0;
					result.data[i * result.step + j * result.elemSize() + 2] *= 0;
				}

			}
			else {
				if (ch3_upper <= v && v <= ch3_lower)
				{
					result.data[i * result.step + j * result.elemSize() + 0] *= 0;
					result.data[i * result.step + j * result.elemSize() + 1] *= 0;
					result.data[i * result.step + j * result.elemSize() + 2] *= 0;
				}
			}
		}
	}
}

/*
 * Labeling processing
 * @input: hue low, hue high [0-180]
 * @input: saturation low, saturation high [0-255]
 * @input: value low, value high [0-255]
 * @input: the image data
 * @output: the result image
*/
void MediaImage::LabelingProcessing(cv::Mat& image)
{
	LabelingProcessing(image, LabelImg, stats, centroids, nLab);
}
void MediaImage::LabelingProcessing(cv::Mat& image, cv::Mat &LabelImg,
	cv::Mat &stats, cv::Mat &centroids, int &nLab)
{
	cv::Mat gray;
	if (image.type() != CV_8UC1)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	cv::Mat binary;
	//大津法
	//cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	cv::threshold(gray, binary, 1, 255, cv::THRESH_BINARY);

	nLab = cv::connectedComponentsWithStats(binary, LabelImg, stats, centroids);
	
	Init_Vector(squares);
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);
		squares.push_back(param[cv::ConnectedComponentsTypes::CC_STAT_AREA]);
	}
}

template<typename T>
void MediaCom::Init_Vector(std::vector<T> &vec)
{
	vec.clear();
	vec.begin();
	vec.shrink_to_fit();
	std::vector<T>().swap(vec);
}

/*
 * Labeling processing
 * @input: the image data
 * @output: the labeled image
 * @output: the status data
 * @output: the centroided image
 * @output: the number of label area
 * @input: mode default = -1
 *    - -1: Draw all
 *    -  0: Draw the rectrangle only
 *    -  1: Draw the gravity only
 *    -  2: Draw the squea only
 *    -  3: Draw the rectangle and the gravity
 *    -  4: Draw the rectangle and the squea
 *    -  5: Draw the gravity and the squea
*/
void MediaImage::DrawLabeling(cv::Mat &result, int mode)
{
	DrawLabeling(image, LabelImg, stats, centroids, nLab, result, mode);
}
void MediaImage::DrawLabeling(cv::Mat& image, cv::Mat &LabelImg,
	cv::Mat &stats, cv::Mat &centroids, int &nLab, cv::Mat &result, int mode)
{
	result = image.clone();
	if (mode < 0 || mode > 5)
		mode = -1;

	if (mode == -1 || mode == 0 || mode == 3 || mode == 4)
	{
		for (int i = 1; i < nLab; ++i) {
			int *param = stats.ptr<int>(i);

			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

			cv::rectangle(result, cv::Rect(x, y, width, height), colors[i - 1], 2);
		}
	}
		
	//重心の出力
	if (mode == -1 || mode == 1 || mode == 3 || mode == 5)
	{
		for (int i = 1; i < nLab; ++i) {
			double *param = centroids.ptr<double>(i);
			int x = static_cast<int>(param[0]);
			int y = static_cast<int>(param[1]);

			cv::circle(result, cv::Point(x, y), 3, colors[i - 1], -1);
		}
	}

	//面積値の出力
	if (mode == -1 || mode == 2 || mode == 4 || mode == 5)
	{
		for (int i = 1; i < nLab; ++i) {
			int *param = stats.ptr<int>(i);
			//ROIの左上に番号を書き込む
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			std::stringstream num, area;
			num << i;
			area << param[cv::ConnectedComponentsTypes::CC_STAT_AREA];
			cv::putText(result, num.str() + ": " + area.str(), cv::Point(x + 5, y + 20), cv::FONT_HERSHEY_COMPLEX, 0.7, colors[i - 1], 2);
		}
	}
}

/*
 * You can get the squares of labeling area.
 * @output: the squares of labeling area.
*/
std::vector<unsigned int> MediaImage::GetSquare()
{
	return squares;
}

/*
 * Morphology processing
 * @input: the image data
 * @output: the image data
*/
void MediaImage::Morphology(cv::Mat &image, cv::Mat &result, std::string mode, int num)
{
	if (mode == "op")
		cv::morphologyEx(image, result, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), num);
	else if (mode == "cl")
		cv::morphologyEx(image, result, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), num);
	else
		ErrorLog("modeには【op】【cl】のどちらかを選択してください。");
}


/*
 * Edge processing
 * @input: the image data
 * @output: the result image data
*/
void MediaImage::EdgeDetection(cv::Mat &image, cv::Mat &result, std::string mode)
{
	cv::Mat gray;
	if (image.type() == CV_8UC3)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	if (mode == "canny")
	{
		cv::Canny(gray, result, 125, 255);
	}
	else if (mode == "laplacian")
	{
		cv::Laplacian(gray, result, 3);
		cv::convertScaleAbs(result, result, 1, 0);
		cv::threshold(result, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	}
	else if (mode == "sobel")
	{
		//x, y方向の微分
		cv::Mat img_s_x, img_s_y;
		cv::Sobel(gray, img_s_x, CV_8UC1, 1, 0, 3);
		cv::Sobel(gray, img_s_y, CV_8UC1, 0, 1, 3);
		//x, y方向に微分をした画像を合成
		result = abs(img_s_x) + abs(img_s_y);
		cv::convertScaleAbs(result, result, 1, 0);
		//2値化
		cv::threshold(result, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
	}
	else
	{
		ErrorLog("modeには【canny】【laplacian】【sobel】のどちらかを選択してください。");
	}
}

/*
 * Histogram of priented gradient (HoG) feature functions
 * @input: the resolution
 * @input: the cell size
 * @input: the block size
*/
void MediaImage::SetHoGParameters(const int _bin, const int _cell, const int _block)
{
	//Hog initalize
	N_BIN = _bin;
	HoG_THETA = (180 / _bin);

	CELL_SIZE = _cell;
	BLOCK_SIZE = _block;
	R = CELL_SIZE * BLOCK_SIZE * 0.5;
}

/*
 * Histogram of priented gradient (HoG) feature functions
 * @input: the image data
 * @output: the result image data
*/
void MediaImage::CalculateIntegralHOG(const cv::Mat& image)
{
	cv::Mat gray;
	if (image.type() != CV_8UC1)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	//The image differentiate for x-axis and y-axis.
	cv::Mat x_sobel, y_sobel;
	cv::Sobel(gray, x_sobel, CV_32F, 1, 0);
	cv::Sobel(gray, y_sobel, CV_32F, 0, 1);

	//Generate the image of each angles
	std::vector<cv::Mat> bins(N_BIN);
	for (int i = 0; i < N_BIN; i++)
		bins[i] = cv::Mat::zeros(gray.size(), CV_32F);

	// X, Y微分画像を勾配方向と強度に変換
	cv::Mat Imag, Iang;
	cv::cartToPolar(x_sobel, y_sobel, Imag, Iang, true);
	// 勾配方向を[0, 180)にする
	cv::add(Iang, cv::Scalar(180), Iang, Iang < 0);
	cv::add(Iang, cv::Scalar(-180), Iang, Iang >= 179);
	// 勾配方向を[0, 1, ..., 8]にする準備（まだfloat）
	Iang /= HoG_THETA;

	// 勾配方向を強度で重みをつけて、角度別に投票する
	for (int y = 0; y < gray.rows; ++y)
	{
		for (int x = 0; x < gray.cols; ++x)
		{
			int ind = Iang.at<float>(y, x);
			bins[ind].at<float>(y, x) += Imag.at<float>(y, x);
		}
	}

	// 角度別に積分画像生成
	Init_Vector(integrals);
	integrals = std::vector<cv::Mat>(N_BIN);
	for (int i = 0; i < N_BIN; ++i) 
	{
		// 積分画像をつくる、OpenCVの関数がある
		cv::integral(bins[i], integrals[i]);
	}
}

/*
 * Calclate the integral
 * @input: the cell image
 * @input: the cell image
 * @input: the integrals value
 * @output: the cell image
*/
void MediaImage::CalculateHOGInCell(cv::Mat &hogCell, const cv::Rect roi, const std::vector<cv::Mat>& integrals) 
{
	if (integrals.size() != N_BIN)
	{
		ErrorLog("The vector size of integrals is different.");
		return;
	}
	int x0 = roi.x, y0 = roi.y;
	int x1 = x0 + roi.width, y1 = y0 + roi.height;
	for (int i = 0; i < N_BIN; i++) {
		cv::Mat integral = integrals[i];
		float a = integral.at<double>(y0, x0);
		float b = integral.at<double>(y1, x1);
		float c = integral.at<double>(y0, x1);
		float d = integral.at<double>(y1, x0);
		hogCell.at<float>(0, i) = (a + b) - (c + d);
	}
}

/* Calclate the HoG features.
 * @input: the image points
 * @input: the integrals value
 * @output: the hog features.
*/
bool MediaImage::GetHOG(const cv::Point pt, cv::Mat &histgram)
{
	if (integrals.size() > 0)
	{
		//ErrorLog("Not generate the integrals.");
		false;
	}

	histgram = cv::Mat();
	if (pt.x - R < 0 ||
		pt.y - R < 0 ||
		pt.x + R >= integrals[0].cols ||
		pt.y + R >= integrals[0].rows
		) {
		return false;
	}

	//ヒストグラムの初期化
	histgram = cv::Mat::zeros
	(cv::Size(N_BIN * BLOCK_SIZE * BLOCK_SIZE, 1), CV_32F);

	cv::Point tl(0, pt.y - R);
	int cellStep = 0;
	for (int i = 0; i < BLOCK_SIZE; ++i)
	{
		tl.x = pt.x - R;
		for (int j = 0; j < BLOCK_SIZE; ++j)
		{
			cv::Mat temp = histgram.colRange(cellStep, cellStep + N_BIN);
			CalculateHOGInCell(temp,
				cv::Rect(tl, tl + cv::Point(CELL_SIZE, CELL_SIZE)),
				integrals);

			tl.x += CELL_SIZE;
			cellStep += N_BIN;
		}
		tl.y += CELL_SIZE;
	}

	//L2ノルムで正規化
	cv::normalize(histgram, histgram, 1, 0, cv::NORM_L2);

	return true;
}

/* Calclate the HoG features.
 * @input: the image points
 * @output: the hog features.
 */
void MediaImage::CalcHOGHistgram(const cv::Mat &image, cv::Mat &histogram)
{
	//積分値を求める
	CalculateIntegralHOG(image);

	cv::Mat meanHOGInBlock(cv::Size(N_BIN, 1), CV_32F);
	histogram = cv::Mat::zeros(cv::Size(N_BIN, 1), CV_32F);

	Init_Vector(HOGInBlocks);
	Init_Vector(centers);

	for (int y = CELL_SIZE / 2; y < image.rows; y += CELL_SIZE)
	{
		for (int x = CELL_SIZE / 2; x < image.cols; x += CELL_SIZE)
		{
			cv::Mat hogHist;


			//画像からはみ出ていたら無視する
			if (!GetHOG(cv::Point(x, y), hogHist))
			{
				continue;
			}

			//勾配ヒストグラムを生成する
			meanHOGInBlock = cv::Scalar(0);

			double max = 0.0;
			int index = 0;

			for (int i = 0; i < N_BIN; ++i)
			{
				for (int j = 0; j < BLOCK_SIZE * BLOCK_SIZE; ++j)
				{
					meanHOGInBlock.at<float>(0, i)
						+= hogHist.at<float>(0, i + j * N_BIN);
				}

				//最大値を考慮する
				if (meanHOGInBlock.at<float>(0, i) > max)
				{
					max = meanHOGInBlock.at<float>(0, i);
					index = i;
				}
			}
			histogram.at<float>(0, index)++;

			//表示用の値を格納する
			cv::Mat norm;
			cv::normalize(meanHOGInBlock, norm, 1, 0, cv::NORM_L2);
			HOGInBlocks.push_back(norm);
			centers.push_back(cv::Point(x, y));
		}
	}
	//Initalize 0 to 1
	cv::normalize(histogram, histogram, 1, 0, cv::NORM_L2);
}

/* Show the HoG features on result image.
		 * @input: the image points
		 * @input: the integrals value
		 * @output: the hog features.
		 */
void MediaImage::GetHoGOnImage(const cv::Mat &image, cv::Mat &result)
{
	result = image.clone();

	if (result.type() != CV_8UC1)
		cvtColor(result, result, cv::COLOR_BGR2GRAY);

	result *= 0.5;

	//Draw the strongth and directions
	for (int i = 0; i < std::min(HOGInBlocks.size(), centers.size()); ++i)
	{
		for (int j = 0; j < N_BIN; ++j)
		{
			//double theta = (j * HoG_THETA + 90.0) * CV_PI / 180.0;
			double theta = (j * HoG_THETA) * CV_PI / 180.0;

			cv::Point rd(CELL_SIZE * 0.5 * cos(theta), CELL_SIZE * 0.5 * sin(theta));
			cv::Point rp = centers[i] - rd;
			cv::Point lp = centers[i] - -rd;
			cv::line(result, rp, lp, cv::Scalar(255 * HOGInBlocks[i].at<float>(0, j), 255, 255));
		}
	}
}

/*Calclate the optical flow.
 * @input: the prev image
 * @input: the next image
 * @output: the flow histogram.
 */
void MediaImage::CalclateOpticalFlow(const cv::Mat &prev, const cv::Mat &next, cv::Mat &flow, std::vector<cv::Point2f> &prev_pts, const int vector_num)
{
	cv::Mat gray_prev, gray_next;
	if (prev.type() != CV_8UC1)
		cv::cvtColor(prev, gray_prev, cv::COLOR_BGR2GRAY);
	else
		gray_prev = prev.clone();

	if (next.type() != CV_8UC1)
		cv::cvtColor(next, gray_next, cv::COLOR_BGR2GRAY);
	else
		gray_next = next.clone();

	//std::vector<cv::Point2f> prev_pts;

	cv::Size flowSize(vector_num, vector_num); //ベクトルの数

	cv::Point2f center = cv::Point(gray_prev.cols / 2., gray_prev.rows / 2.);
	for (int i = 0; i < flowSize.width; ++i) {
		for (int j = 0; j < flowSize.height; ++j) {
			cv::Point2f p(i*float(gray_prev.cols) / (flowSize.width - 1),
				j*float(gray_prev.rows) / (flowSize.height - 1));
			prev_pts.push_back((p - center)*0.95f + center);
		}
	}
	
	//cv::Mat flow;

	//計算する
	cv::calcOpticalFlowFarneback(gray_prev, gray_next, flow, 0.8, 10, 15, 3, 5, 1.1, 0);

	// オプティカルフローの表示
//	std::vector<cv::Point2f>::const_iterator p = prev_pts.begin();
//	for (; p != prev_pts.end(); ++p) {
//		const cv::Point2f& fxy = flow.at<cv::Point2f>(p->y, p->x);
//		cv::line(res, *p, *p + fxy * 8, cv::Scalar(0), 1);
//	}
}

/* Calclate the optical flow.
 * @input: the point
 * @input: the flow
 * @output: the result.
 */
void MediaImage::ShowOpticalFlow(const std::vector<cv::Point2f> &points, const cv::Mat &flow, cv::Mat &result)
{
	// オプティカルフローの表示
	std::vector<cv::Point2f>::const_iterator p = points.begin();
	for (; p != points.end(); ++p) {
		const cv::Point2f& fxy = flow.at<cv::Point2f>(p->y, p->x);
		cv::line(result, *p, *p + fxy * 8, cv::Scalar(0), 1);
	}
}

/* ------------------------------- *
         Facial processing
   -------------------------------
 */

 /* Get the features of facial.
  * @input: the image
  * @input: the cascade file path
  * @output: the facial point.
  */
bool MediaImage::GetFaceFeatures(const cv::Mat &image, std::vector<cv::Rect_<int>>& features, const std::string cascade, const double scaleFactor)
{
	cv::CascadeClassifier cas;
	cas.load(cv::samples::findFile(cascade, !cascade.empty()));

	if (!cas.empty())
	{
		cas.detectMultiScale(image, features, scaleFactor, 3, 0 | cv::CASCADE_SCALE_IMAGE);//, cv::Size(30, 30));
		
	}
	else
	{
		return false;
	}

	Init_Vector(faces_count);
	faces_count.push_back(features.size());
	return true;
}

/* Get the features of facial.
 * @input: the image
 * @input: the cascade file path
 * @input: the facial regions.
 * @input: the scale factorm, default=1.1.
 * @output: the facial parts regions.
 */
bool MediaImage::GetFaceFeatures(const cv::Mat &image, const std::vector<cv::Rect_<int>>& face_regions, std::vector<cv::Rect_<int>>& features, const std::string cascade, const double scaleFactor)
{
	Init_Vector(parts_count);
	
	std::vector<cv::Rect_<int>> f;
	for(int i = 0; i < face_regions.size(); ++i)
	{ 
		cv::Mat roi = image(face_regions[i]);

		if (!GetFaceFeatures(roi, features, cascade, scaleFactor))
			return false;

		for (int j = 0; j < features.size(); ++j)
		{
			features[j].x += face_regions[i].x;
			features[j].y += face_regions[i].y;
			f.push_back(features[j]);
		}
		parts_count.push_back(std::max(features.size(),(size_t)0));
	}
	features = f;
	return true;
}


/* Draw the features of facial on the image.
 * @input: the image
 * @input: the face featuers
 * @output: the detected image.
 */
void MediaImage::DrawFeatures(cv::Mat &image, std::vector<cv::Rect_<int>>& features)
{
	for (int i = 0; i < features.size(); ++i)
	{
		cv::Rect face = features[i];
		cv::rectangle(image, face, colors[i], 1, 8);
	}
}
/* Draw the features of facial on the image.
 * @input: the image
 * @input: the face featuers
 * @input: the smile score
 * @output: the detected image.
 */
void MediaImage::DrawFeatures(cv::Mat &image, std::vector<cv::Rect_<int>>& features, std::vector<float>& score)
{
	if (features.size() != score.size())
		throw "検出された数が異なります。\n";

	for (int i = 0; i < features.size(); ++i)
	{
		cv::Rect face = features[i];
		cv::rectangle(image, face, cv::Scalar(255 - (255 * score[i]), 0, 255 * score[i]), 3, 8);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(3) << score[i];
		cv::putText(image, ss.str(), cv::Point(face.x, face.y+20), cv::FONT_HERSHEY_SIMPLEX,
			0.8, cv::Scalar(255 - (255 * score[i]), 0, 255 * score[i]), 2, cv::LINE_AA);
	}
}

/* Get the number of detected regions.
		 * @output: The number of detected regions.
		 */
std::vector<int> MediaImage::GetDetectedCount()
{
	return parts_count;
}

/* Evaluate a smile level.
 * @input: the image
 * @input: the number of detected regions
 * @output: the smile score.
 */
void MediaImage::EvaluateSmile(std::vector<int>& count, std::vector<float>& score)
{
	double max = 5;
	for (int i = 0; i < count.size(); ++i)
	{
		if (max < count[i])
			max = count[i];
	}

	for (int i = 0; i < count.size(); ++i)
	{
		score.push_back(count[i] / max);
	}
}


/* Get the features of facial.
 * @input: the image
 * @input: the HOG descripter
 * @input: the stride pixel
 * @input: the padding pixel
 * @input: the scale factor, default=1.1
 * @output: the human regions.
 */
bool MediaImage::GetHumanFeatures(const cv::Mat &image, cv::HOGDescriptor &hog, 
	std::vector<cv::Rect_<int>>& features, const int stride, const int padding, 
	const double scaleFactor)
{
	//グレー画像か確認
	if(image.type() != CV_8UC1)
	{
		return false;
	}

	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
	hog.detectMultiScale(image, features, 0, 
		cv::Size(stride, stride), cv::Size(padding, padding), scaleFactor, 2);

	Init_Vector(human_count);
	human_count.push_back(features.size());
	return true;
}




/*--------------------------------
			Machine Learning
---------------------------------*/
/* Generate the color histogram.
 * @input: the image.
 * @output: the color histogram.
 */
bool MediaImage::GenerateColorHistogram(cv::Mat& img, std::vector<double> &hist, int bin, const int channel)
{
	if (channel < 0 || channel > 3) return false;

	//初期化
	hist = std::vector<double>(bin*channel, 0);

	//hsvに変換する
	cv::Mat hsv;
	cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

	for (int r = 0; r < channel; ++r)
	{
		for (int y = 0; y < hsv.size().height; ++y)
		{
			for (int x = 0; x < hsv.size().width; ++x)
			{
				int c = (int)hsv.data[y * hsv.step + x * hsv.elemSize() + r];
				hist[(c *bin/ 180) + bin * r]++;
			}
		}
	}

	for (int i = 0; i < hist.size(); ++i)
	{
		hist[i] = hist[i] / (hsv.cols * hsv.rows);
	}

	return true;
}

bool MediaImage::GenerateColorHistogram(cv::Mat& img, cv::Mat &hist, int bin)
{

	//初期化
	hist = cv::Mat::zeros(1, bin, CV_32FC1);

	//hsvに変換する
	cv::Mat hsv;
	cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

	for (int y = 0; y < hsv.rows; ++y)		
	{
		for (int x = 0; x < hsv.cols; ++x)		
		{
			if (hsv.data[y * hsv.step + x * hsv.elemSize() + 1] > 20) {

				int c = (int)hsv.data[y * hsv.step + x * hsv.elemSize()];
				hist.at<float>(0, c * (bin-4) / 180 + 4)++;
			}
			else {
				int v = (int)hsv.data[y * hsv.step + x * hsv.elemSize() + 2];
				if (v < 64) {
					hist.at<float>(0, 0)++;
				}
				else if (v < 128) {
					hist.at<float>(0, 1)++;
				}
				else if (v < 192) {
					hist.at<float>(0, 2)++;
				}
				else {
					hist.at<float>(0, 3)++;
				}
			}
		}
	}

	hist /= hsv.cols * hsv.rows;
	
	return true;
}

/*----------------
 * 各要素で同じ数値をカウントする
 * [in]  : original
 * [in]  : predicted
 * [out] : 一致数
 *------------------*/
int MediaImage::CalcCountElements(cv::Mat &original, cv::Mat &predicted)
{
	int count = 0;
	if (CheckImage(original) && CheckImage(predicted))
	{
		//同じサイズでなければエラー
		if (original.size() != predicted.size())
		{
			std::cerr << "同じサイズではありません。\n";
			return count;
		}

		//ラベルデータがfloat型ならint型に変化する
		cv::Mat S1, S2;
		if (original.type() != CV_32S)
			original.convertTo(S1, CV_32S);
		else
			S1 = original.clone();

		if (predicted.type() != CV_32S)
			predicted.convertTo(S2, CV_32S);
		else
			S2 = predicted.clone();

		//比較し，同じであれば加算
		for (int i = 0; i < original.rows; ++i)
		{
			for (int j = 0; j < original.cols; ++j)
			{
				int a = S1.at<int>(i, j);
				int b = S2.at<int>(i, j);
				if (a == b) count++;
			}
		}
	}
	return count;
}


/*-----------------
* 識別精度を計算する
* [in] trainラベル
* [in] 結果ラベル
*----------------*/
float MediaImage::CalculateAccuracyPercent(cv::Mat &original, cv::Mat &predicted)
{
	return 100 * CalcCountElements(original, predicted) / (float)predicted.total();
}

/*-----------------
* 識別精度を計算する
* Coonfusion Matrixなので対角線でカウント
* [in] Confusion Matrix
*----------------*/
float MediaImage::CalculateAccuracyPercent(cv::Mat &confusionMatrix)
{
	int total = 0;
	int count = 0;
	for (int i = 0; i < confusionMatrix.rows; ++i)
	{
		for (int j = 0; j < confusionMatrix.cols; ++j)
		{
			if (i == j)
				count += confusionMatrix.at<int>(i, j);
			total += confusionMatrix.at<int>(i, j);
		}
	}

	return 100 * count / (float)total;
}

//-- k-NN functions
/* k nearest neighobr alghrithm
 * @input: k value
 * @input: algorithm name
 */
void MediaImage::kNN(const int _k, const std::string _algorithm)
{
	SetK(_k);
	SetAlgorithm(_algorithm);
}

/* The setting k using k nearest neighobr alghrithm
 * @input: k value
 */
void MediaImage::SetK(const int _k)
{
	k = _k;
}

/* The setting algorithm name using k nearest neighobr alghrithm
 * @input: algorithm name
 */
void MediaImage::SetAlgorithm(const std::string _algorithm)
{
	algorithm = _algorithm;
}

/*-------------------------------
 * Learning the k-NN
 * @input: the training data
 * @input: the label data
 * @output: the result of k-NN
 *-------------------------------*/
bool MediaImage::Train(const cv::Mat train, const cv::Mat labels,
	cv::Ptr<cv::ml::KNearest> &kNearest)
{
	//svmを初期化する
	kNearest = cv::ml::KNearest::create();

	if (algorithm == "BRUTE")
	{
		kNearest->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
	}
	else if (algorithm == "KDTREE")
	{
		kNearest->setAlgorithmType(cv::ml::KNearest::KDTREE);
	}
	else
	{
		std::cerr << "アルゴリズムタイプ\n";
		std::cerr << "   BRUTE  KDTREE\n";
		return false;
	}
	kNearest->setDefaultK(k);

	kNearest->train(train, cv::ml::ROW_SAMPLE, labels);
	return true;
}



//-- SVM functions
		/* support vector machine alghrithm
		*   @input: type : C_SVC
		*   @input: kernel : LINEAR
		*   @input: C : 1.0
		*   @input: degree : 1.2
		*   @input: gamma : 0.0001
		*   @input: coef0 : 1.0
		*-------------------*/
void MediaImage::SupperVctorMachine(
	const std::string _type, const std::string _kernel,
	const double _C, const double _degree,
	const double _gamma, const double _coef0)
{
	SetType(_type);
	SetKernel(_kernel);
	SetC(_C);
	SetDegree(_degree);
	SetGamma(_gamma);
	SetCoef(_coef0);
}

/* Set the kernel type.
 * @input: kernel type
 *-------------------*/
void MediaImage::SetKernel(const std::string _karnel)
{
	kernel = _karnel;
}

/* Set the model type
 * @input: model type
 *------------------*/
void MediaImage::SetType(const std::string _type)
{
	type = _type;
}

/* Set the C value
 * @input: C value
 *------------------*/
void MediaImage::SetC(const double _C)
{
	C = _C;
}

/* Set the gamma value
 * @input: gamma value
 *------------------*/
void MediaImage::SetGamma(const double _gamma)
{
	gamma = _gamma;
}

/* Set the degree
 * @input: degree
 *------------------*/
void MediaImage::SetDegree(const double _degree)
{
	degree = _degree;
}

/*Set the Ceof0 value
 * @input: coef0
 *------------------*/
void MediaImage::SetCoef(const double _coef0)
{
	coef0 = _coef0;
}

/*-------------------------------
* Training the SVM
* @input: training data
* @input: label data
* @output: result of svm
*-------------------------------*/
void MediaImage::Train(const cv::Mat train, const cv::Mat labels,
	cv::Ptr<cv::ml::SVM> &svm)
{
	//svmを初期化する
	svm = cv::ml::SVM::create();

	if (type == "C_SVC")
	{
		svm->setType(cv::ml::SVM::C_SVC);
	}
	else if (type == "NU_SVC")
	{
		svm->setType(cv::ml::SVM::NU_SVC);
	}
	else if (type == "ONE_CLASS")
	{
		svm->setType(cv::ml::SVM::ONE_CLASS);
	}
	else if (type == "EPS_SVR")
	{
		svm->setType(cv::ml::SVM::EPS_SVR);
	}
	else if (type == "NU_SVR")
	{
		svm->setType(cv::ml::SVM::NU_SVR);
	}
	else {
		std::cerr << "SVMのタイプを設定してください。\n";
		std::cerr << "   C_SVC    NU_SVC    ONE_CLASS\n";
		std::cerr << "   EPS_SVR  NU_SVR\n";
		return;
	}

	//カーネルのセット
	if (kernel == "LINEAR")
	{
		svm->setKernel(cv::ml::SVM::LINEAR); //LINEAR, POLY, RBF
	}
	else if (kernel == "RBF")
	{
		svm->setKernel(cv::ml::SVM::RBF); //LINEAR, POLY, RBF
	}
	else if (kernel == "POLY")
	{
		svm->setKernel(cv::ml::SVM::POLY); //LINEAR, POLY, RBF
	}
	else if (kernel == "SIGMOID")
	{
		svm->setKernel(cv::ml::SVM::SIGMOID); //LINEAR, POLY, RBF
	}
	else if (kernel == "CHI2")
	{
		svm->setKernel(cv::ml::SVM::CHI2); //LINEAR, POLY, RBF
	}
	else if (kernel == "INTER")
	{
		svm->setKernel(cv::ml::SVM::INTER); //LINEAR, POLY, RBF
	}
	else {
		std::cerr << "カーネルをセットしてください\n";
		std::cerr << "   LINEAR    RBF    POLY\n";
		std::cerr << "   SIGMOID   CHI2   INTER\n";
		return;
	}
	svm->setC(C);      //for C_SVC
	svm->setDegree(degree); //for POLY
	svm->setGamma(gamma);  //for POLY, RBF
	svm->setCoef0(coef0);  //for POLY
	svm->setTermCriteria(cv::TermCriteria(
		cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 1000, 1e-6));

	svm->train(train, cv::ml::ROW_SAMPLE, labels);
}



