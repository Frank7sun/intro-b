#pragma once

#include "MediaImageProcess.hpp"
#include "MediaAction.hpp"

namespace media
{
	class MediaImage : public MediaCom
	{
	public:

		//コンストラクタとデストラクタの定義
		MediaImage();
		MediaImage(const MediaImage& m);
		~MediaImage();

		/*
		 * Create the color list
		 * @input: the number of list
		*/
		void ColorInit(const int num = -1);

		/*
		 * Get the color list
		 * @output: the number of list
		*/
		std::vector<cv::Vec3b> GetColor();

		/*
		 * Read the image data
		 * @input: image data path
		 * @input: image data type
		*/
		void ReadImage(const std::string path);
		void ReadImage(const std::string path, cv::Mat &image);

		/*
		 * Show the image data
		 * @input: windows caption
		 * @input: image data type
		*/
		void ShowImage(const std::string caption);
		void ShowImage(const std::string caption, cv::Mat &image);
		void ShowImage(const std::string caption, std::vector<cv::Mat> &image);

		/*
		 * Release the image data
		 * @input: image data type
		*/
		void ReleaseImage();
		void ReleaseImage(cv::Mat &image);
		void ReleaseImage(std::vector<cv::Mat> &image);
		template<typename T>
		void ReleaseImage(std::vector<cv::Mat_<T>> &image);

		/*
		 * Release the image data
		 * @input: window caption
		*/
		void ReleaseWindow();
		void ReleaseWindow(const std::string caption);

		/*
		 * Release the image data
		 * @input: image data type
		 * @input: the name of written image
		 * @input: the format of written image (default JPEG)
		*/
		void SaveImage(const std::string caption, std::string format = ".jpg");
		void SaveImage(cv::Mat &image, const std::string caption, const std::string format = ".jpg");
		void SaveImage(std::vector<cv::Mat> &image, const std::string caption, const std::string format = ".jpg");

		/*
		 * Move the window
		 * @input: the window caption
		 * @input: the x point (default 0)
		 * @input: the y point (default 0)
		*/
		void MoveWindow(const std::string caption, const int x = 0, const int y = 0);


		//WebCamera
		void InitVideo(const int camera_num = 0, const int width = 320, const int height = 240);
		void InitVideo(const std::string videoName);
		void UpdateVideo();

		/*
		 * Return the image data
		*/
		cv::Mat GetImage();

		//画像処理関係の関数
		/*
		 * Color extraction function
		 * @input: hue low, hue high [0-180]
		 * @input: saturation low, saturation high [0-255]
		 * @input: value low, value high [0-255]
		 * @input: the image data
		 * @output: the result image
		*/
		void ExtractColor(const int ch1_lower, const int ch1_upper,
			const int ch2_lower, const int ch2_upper,
			const int ch3_lower, const int ch3_upper,
			cv::Mat &result);

		void ExtractColor(const int ch1_lower, const int ch1_upper,
			const int ch2_lower, const int ch2_upper,
			const int ch3_lower, const int ch3_upper,
			cv::Mat &inImage, cv::Mat &result);

		/*
		 * Labeling processing
		 * @input: the image data
		 * @output: the labeled image
		 * @output: the status data
		 * @output: the centroided image
		 * @output: the number of label area
		*/
		void LabelingProcessing(cv::Mat& image);
		void LabelingProcessing(cv::Mat& image, cv::Mat &LabelImg,
			cv::Mat &stats, cv::Mat &centroids, int &nLab);

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
		void DrawLabeling(cv::Mat& result, int mode = -1);
		void DrawLabeling(cv::Mat& image, cv::Mat &LabelImg,
			cv::Mat &stats, cv::Mat &centroids, int &nLab, cv::Mat &result, int mode = -1);

		/*
		 * You can get the squares of labeling area.
		 * @output: the squares of labeling area.
		*/
		std::vector<unsigned int> GetSquare();

		/*
		 * Morphology processing
		 * @input: the image data
		 * @output: the image data
		*/
		void Morphology(cv::Mat &image, cv::Mat &result, std::string mode = "op", int num = 1);

		/*
		 * Edge processing
		 * @input: the image data
		 * @output: the result image data
		*/
		void EdgeDetection(cv::Mat &image, cv::Mat &result, std::string mode = "canny");

		/*----- HoG features -----*/
		/*
		 * Histogram of priented gradient (HoG) feature functions
		 * @input: the resolution
		 * @input: the cell size
		 * @input: the block size
		*/
		void SetHoGParameters(const int _bin, const int _cell, const int _block);

		/* Calclate the HoG features.
		 * @input: the image points
		 * @input: the integrals value
		 * @output: the hog features.
		 */
		void CalcHOGHistgram(const cv::Mat &image, cv::Mat &histogram);


		/* Show the HoG features on result image.
		 * @input: the image points
		 * @input: the integrals value
		 * @output: the hog features.
		 */
		void GetHoGOnImage(const cv::Mat &image, cv::Mat &result);

		
		/* Calclate the optical flow.
		 * @input: the prev image
		 * @input: the next image
		 * @output: the flow histogram.
		 */
		void CalclateOpticalFlow(const cv::Mat &prev, const cv::Mat &next, cv::Mat &flow, std::vector<cv::Point2f> &prev_pts, const int vector_num = 50);

		/* Calclate the optical flow.
		 * @input: the point
		 * @input: the flow
		 * @output: the result.
		 */
		void ShowOpticalFlow(const std::vector<cv::Point2f> &points, const cv::Mat &flow, cv::Mat &result);


		/*--------------------------------
			Face Detect Functions
		---------------------------------*/
		/* Get the features of facial.
		 * @input: the image
		 * @input: the cascade file path
		 * @input: the scale factor, default=1.1.
		 * @output: the facial regions.
		 */
		bool GetFaceFeatures(const cv::Mat &image, std::vector<cv::Rect_<int>>& features, const std::string cascade, const double scaleFactor = 1.1);

		/* Get the features of facial.
		 * @input: the image
		 * @input: the cascade file path
		 * @input: the facial regions.
		 * @input: the scale factorm, default=1.1.
		 * @output: the facial parts regions.
		 */
		bool GetFaceFeatures(const cv::Mat &image, const std::vector<cv::Rect_<int>>& face_regions, std::vector<cv::Rect_<int>>& features, const std::string cascade, const double scaleFactor = 1.1);

		/* Draw the features of facial on the image.
		 * @input: the image
		 * @input: the face featuers
		 * @output: the detected image.
		 */
		void DrawFeatures(cv::Mat &image, std::vector<cv::Rect_<int>>& features);
		/* Draw the features of facial on the image.
		 * @input: the image
		 * @input: the face featuers
		 * @input: the smile score
		 * @output: the detected image.
		 */
		void DrawFeatures(cv::Mat &image, std::vector<cv::Rect_<int>>& features, std::vector<float>& score);

		/* Get the number of detected regions.
		 * @output: The number of detected regions.
		 */
		std::vector<int> GetDetectedCount();

		/* Evaluate a smile level.
		 * @input: the image
		 * @input: the number of detected regions
		 * @output: the smile score.
		 */
		void EvaluateSmile(std::vector<int>& count, std::vector<float>& score);


		/*--------------------------------
			Human Detect Functions
		---------------------------------*/
		/* Get the features of facial.
		 * @input: the image
		 * @input: the HOG descripter
		 * @input: the stride pixel
		 * @input: the padding pixel
		 * @input: the scale factor, default=1.1
		 * @output: the human regions.
		 */
		bool GetHumanFeatures(const cv::Mat &image, cv::HOGDescriptor &hog, std::vector<cv::Rect_<int>>& features, 
			const int stride = 8, const int padding = 16, const double scaleFactor = 1.1);


		/*--------------------------------
		    Machine Learning
		---------------------------------*/
		/* Generate the color histogram.
		 * @input: the image.
		 * @output: the color histogram.
		 */
		bool GenerateColorHistogram(cv::Mat& img, std::vector<double> &hist, int bin = 180, const int channel = 1);
		bool GenerateColorHistogram(cv::Mat& img, cv::Mat &hist, int bin = 180);

		/*----------------
	     * 各要素で同じ数値をカウントする
	     * [in]  : original
	     * [in]  : predicted
	     * [out] : 一致数
	     *------------------*/
		int CalcCountElements(cv::Mat &original, cv::Mat &predicted);

		/*-----------------
		 * 識別精度を計算する
		 * [in] trainラベル
		 * [in] 結果ラベル
		 *----------------*/
		float CalculateAccuracyPercent(cv::Mat &confusionMatrix);


		/*-----------------
		* 識別精度を計算する
		* [in] trainラベル
		* [in] 結果ラベル
		*----------------*/
		float CalculateAccuracyPercent(cv::Mat &original, cv::Mat &predicted);

		//-- k-NN functions
		/* k nearest neighobr alghrithm
		 * @input: k value
		 * @input: algorithm name
		 */
		void kNN(const int _k, const std::string _algorithm = "BRUTE");

		/* The setting k using k nearest neighobr alghrithm
		 * @input: k value
		 */
		void SetK(const int _k);

		/* The setting algorithm name using k nearest neighobr alghrithm
		 * @input: algorithm name
		 */
		void SetAlgorithm(const std::string _algorithm);

		/*-------------------------------
		 * Training the k-NN
		 * @input: the training data
		 * @input: the label data
		 * @output: the result of k-NN
		 *-------------------------------*/
		bool Train(const cv::Mat train, const cv::Mat labels,
			cv::Ptr<cv::ml::KNearest> &kNearest);


		//-- SVM functions
		/* support vector machine alghrithm
		*   @input: type : C_SVC
		*   @input: kernel : LINEAR
		*   @input: C : 1.0
		*   @input: degree : 1.2
		*   @input: gamma : 0.0001
		*   @input: coef0 : 1.0
		*-------------------*/
		void SupperVctorMachine(
			const std::string _type = "C_SVC", const std::string _kernel = "LINEAR",
			const double _C = 1.0, const double _degree = 1.2,
			const double _gamma = 0.0001, const double _coef0 = 1.0);

		/* Set the kernel type.
		 * @input: kernel type
		 *-------------------*/
		void SetKernel(const std::string _karnel);

		/* Set the model type
		 * @input: model type
		 *------------------*/
		void SetType(const std::string _type);

		/* Set the C value
		 * @input: C value
		 *------------------*/
		void SetC(const double _C);

		/* Set the gamma value
		 * @input: gamma value
		 *------------------*/
		void SetGamma(const double _gamma);

		/* Set the degree
		 * @input: degree
		 *------------------*/
		void SetDegree(const double _degree);

		/*Set the Ceof0 value
		 * @input: coef0
		 *------------------*/
		void SetCoef(const double _coef0);

		/*-------------------------------
		* Training the SVM
		* @input: training data
		* @input: label data
		* @output: result of svm
		*-------------------------------*/
		void Train(const cv::Mat train, const cv::Mat labels,
			cv::Ptr<cv::ml::SVM> &svm);



		/*--------------------------------
		    OpenNI and NiTE
		---------------------------------*/
		/* Initalize the OpenNI and NiTE
		*-------------------------------*/
		void InitOpenNI(int _X = 320);

		/* Update the OpenNI and NiTE
		*-------------------------------*/
		void UpDate();

		/* Convert the color stream to the opencv type.
		 * input: the color stream
		 * output: the matrix data
		 * ------------------------------------------*/
		void ChangeColorStream(const openni::VideoFrameRef &videoFrame,
			cv::Mat &image);

		/* Convert the depth stream to the opencv type.
		 * Convert the 0 - 10,000 meter to 8 bit.
		 * input: the depth stream
		 * output: the matrix data
		 * ------------------------------------------*/
		void ChangeDepthStream(const openni::VideoFrameRef &videoFrame,
			cv::Mat &image);

		/* Draw the skeleton information on the image.
		 *-------------------------------*/
		void CreateSkeleton();
		void CreateSkeleton(cv::Mat& image);

		/* Get the depth data.
		 * input: the depth stream
		 * output: the depth distane
		 * ------------------------------------------*/
		template<typename T>
		void GetDepthData(const openni::VideoFrameRef &videoFrame,
			const T x, const T y, T &depthDist);
		void GetDepthData(const openni::VideoFrameRef &videoFrame,
			const cv::Point2i &point, int &depthDist);

		//NiTE functions 
		/* Convert the depth stream to the opencv type.
		 * Convert the 0 - 10,000 meter to 8 bit.
		 * input: the depth stream
		 * output: the matrix data
		 * ------------------------------------------*/
		void ChangeUserTrackerStream(nite::UserTrackerFrameRef userTracke,
			cv::Mat &image);

		/* Get the skeleton point of user.
		 * input: user information
		 * output: user position
		 * ------------------------------------------*/
		void GetUserSkeletionPosition(
			const openni::VideoFrameRef &videoFrame,
			nite::UserTracker &userTracker,
			const nite::UserData user,
			nite::Point3f *userPosition);
		void GetUserSkeletionPosition(
			const openni::VideoFrameRef &videoFrame,
			nite::UserTracker &userTracker,
			const nite::UserData user,
			int userID);

		/* -------------------------------------------
		 * Draw the skeleton in the image.
		 * input: the color stream.
		 * output: image data.
		 * ------------------------------------------*/
		void DrawSkeletonPoint(
			cv::Mat &image,
			nite::UserTracker &userTracker,
			const nite::UserData user);

		/* Draw the skeletion line.
		 * ------------------------------------------*/
		void DrawLine(const nite::SkeletonJoint& joint1, const nite::SkeletonJoint& joint2,
			const nite::UserTracker &userTracker, cv::Mat &image);

		/* Draw the skeletion.
		 * ------------------------------------------*/
		void DrawSkeleton(
			cv::Mat &image,
			nite::UserTracker &userTracker,
			const nite::UserData user);

		/*
		 * Return the depth data
		*/
		cv::Mat GetDepth();
		cv::Mat GetDepthDist();

		/* Get the user points.
		 * output: the user points
		 * ------------------------------------------*/
		std::vector<std::vector<nite::Point3f> > GetUserPoint();
		
		/* Get the user detection.
		 * output: the user detection
		 * ------------------------------------------*/
		std::vector<bool> CheckUser();

		//ジェスチャー取得
		MediaGesture GetGestures(const int userNum = 0);
		//行動取得
		MediaAction GetAction(const int userNum = 0);


		//強制認識関数
		bool PleaseGesture(const MediaGesture gesuture, const int timeout = -1, const int userNum = 0);
		bool PleaseAction(const MediaAction action, const int timeout = -1, const int userNum = 0);

		//行動情報に関する関数
		void ResetActionLog();
		bool SetActionLog(const MediaGesture gesture, const int userNum);
		bool AnalysisActionModel(const std::vector<MediaGesture> recogAction, const int userNum);

	private:

		/*
		 * Check the image data
		 * @input: image
		 * @output: exist-true, none-false
		*/
		bool CheckImage(cv::Mat &image);

		/*
		 * Histogram of priented gradient (HoG) feature functions
		 * @input: the image data
		 * @output: the result image data
		*/
		void CalculateIntegralHOG(const cv::Mat& image);

		/* Calclate the HoG features.
		 * @input: the image points
		 * @input: the integrals value
		 * @output: the hog features.
		*/
		bool GetHOG(const cv::Point pt, cv::Mat &histgram);

		/*
		 * Calclate the integral
		 * @input: the cell image
		 * @input: the cell image
		 * @input: the integrals value
		 * @output: the cell image
		*/
		void CalculateHOGInCell(cv::Mat &hogCell, const cv::Rect roi, const std::vector<cv::Mat>& integrals);

		//色の格納
		std::vector<cv::Vec3b> colors;

		//共通の画像データ
		cv::Mat image;

		//共通のビデオデータ
		cv::VideoCapture capture;

		//ラべリング処理
		cv::Mat LabelImg;
		cv::Mat stats;
		cv::Mat centroids;
		int nLab;

		std::vector<unsigned int> squares;
		std::map<int, int> area;

		//HoG
		int N_BIN;
		int HoG_THETA;

		int CELL_SIZE;
		int BLOCK_SIZE;
		double R;

		std::vector<cv::Mat> integrals;
		std::vector<cv::Mat> HOGInBlocks;
		std::vector<cv::Point> centers;

		/*--------------------------------
			FaceDetection
		---------------------------------*/
		std::vector<int> faces_count;
		std::vector<int> parts_count;

		std::vector<int> human_count;
		/*--------------------------------
			Machine Learning
		---------------------------------*/
		//-- k-NN functions
		std::string algorithm;
		int k;


		//-- SVM functions
		double gamma;
		double C;
		double degree;
		double coef0;
		std::string kernel;
		std::string type;


		/*--------------------------------
			OpenNI NiTEに関する変数
		---------------------------------*/
		void ChangeResolution(openni::VideoStream& stream);

		//画像サイズ
		int xtionX;

		//深度データ
		cv::Mat depth, depthDist;

		//ドライバの状態
		openni::Status rc;

		//デバイス情報
		openni::Device device;

		//センサから得られたRGB-Dデータ
		openni::VideoStream depthStream, colorStream;

		//ユーザートラッカー変数
		nite::UserTracker userTracker;
		//フレームの更新処理
		openni::VideoFrameRef colorFrame, depthFrame;
		nite::UserTrackerFrameRef userFrame;
		//関節の座標
		//nite::Point3f userPositions[15][15];
		std::vector<std::vector<nite::Point3f> > userSkeletonPosition;
		std::vector<bool> userCheck = std::vector<bool>(15);

		
		//std::vector<Skeletons> userPositions;

		//フラグ
		bool skeletonFlag;
		//行動ログ
		MediaGesture gestureLog[15][ACTION_LENGTH];
		std::vector<std::vector<MediaGesture>> log;

	};
}