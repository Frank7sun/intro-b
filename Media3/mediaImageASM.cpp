#include "MediaImageASM.hpp"

using namespace media;

static CvMemStorage* __storage = NULL; 		/**< Memory storage */
static CvHaarClassifierCascade* __cascade = NULL;	/**< Haar cascade classifier */

/*--------------------------------
	Active Shape Model
---------------------------------*/
/* Initalize the ASM.
 */
void MediaImage::InitASM(const char* asm_model, const char* cascade)
{
	asm_model = asm_model;
	cascade = cascade;

	int n_iteration = 20;
	asm_flag = false;

	double t = (double)cvGetTickCount();
	if (fit_asm.Read(asm_model) == false)
	{
		ErrorLog("");
		throw std::runtime_error("モデルファイルを開けませんでした\n");
	}
	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency()*1000.);
	printf("ASM model file read time cost: %.2f millisec\n", t);

	t = (double)cvGetTickCount();
	if (init_detect_cascade(cascade_name) == false)
	{
		ErrorLog("");
		throw std::runtime_error("カスケードファイルを開けませんでした\n");
	}
	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency()*1000.);
	printf("Opencv haar-like file read time cost: %.2f millisec\n", t);
}


/* Calclate the ASM.
 * @input: the asm model.
 * @input: the cascade model.
 */
void MediaImage::CalcASM()
{
	CalcASM(image);
}
void MediaImage::CalcASM(cv::Mat& img)
{
	/*
	  ASM library uses IplImage type, 
	  so I translate cv::Mat type to IplImage type.
	*/
	if (!CheckImage(img))
	{
		ErrorLog("");
		throw std::runtime_error("No image data");
	}
	asmImage = img.clone();
	IplImage asm_img = asmImage;

	int nFaces = 0;
	int j = 0;

	//Firstly, we detect face by using Viola_jones haarlike-detector
	asm_flag = detect_all_faces(&detshapes, nFaces, &asm_img);

	shapes = new asm_shape[nFaces];

	//Secondly, we initialize shape from the detected box
	if (asm_flag)
	{
		//InitShapeFromDetBox(shape, detshape, fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());
		for (int i = 0; i < nFaces; i++)
		{
			InitShapeFromDetBox(shapes[i], detshapes[i], fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());
		}
	}

	//Thirdly, we do image alignment 
	for (int i = 0; i < nFaces; i++)
	{
		asm_flag = fit_asm.ASMSeqSearch(shapes[i], &asm_img, j, true, n_iteration);
	}

	for (int i = 0; i < nFaces; i++)
	{
		for (int k = 0; k < 68; k++)
		{
			GetFacePosition(shapes[i], k, face_point[i][k]);
		}
	}
}

/*
 * Calclate the integral
 * @input: the asm shape data
 * @input: the potision number
 * @input: the face position
 * @output: the face position
*/
void MediaImage::GetFacePosition(const asm_shape shape, const int position_num, cv::Point2d &face_position)
{
	face_position.x = shape[position_num].x;
	face_position.y = shape[position_num].y;
}

/*
 * @output: the draw asm features on the image
 */
void MediaImage::GetASM(cv::Mat& asm_image)
{
	asm_image = asmImage.clone();
}


/*--------------------------------
	Active Shape Model Library
---------------------------------*/
bool init_detect_cascade(const char* cascade_name)
{
	__cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
	if (__cascade == 0)
	{
		printf("ERROR(%s, %d): Can't load cascade file!\n", __FILE__, __LINE__);
		return false;
	}
	__storage = cvCreateMemStorage(0);
	return true;
}


void destory_detect_cascade()
{
	cvReleaseMemStorage(&__storage);
	cvReleaseHaarClassifierCascade(&__cascade);
}

bool detect_all_faces(asm_shape** Shapes, int& n_shapes, const IplImage* image)
{
	IplImage* pWork = cvCreateImage
	(cvSize(image->width / 2, image->height / 2), image->depth, image->nChannels);
	cvPyrDown(image, pWork, CV_GAUSSIAN_5x5);

	CvSeq* pFaces = cvHaarDetectObjects(pWork, __cascade, __storage,
		1.1, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30));

	cvReleaseImage(&pWork);

	n_shapes = 0;
	if (0 == pFaces->total)//can't find a face
		return false;

	*Shapes = new asm_shape[pFaces->total];
	n_shapes = pFaces->total;
	for (int iFace = 0; iFace < pFaces->total; iFace++)
	{
		(*Shapes)[iFace].Resize(2);
		CvRect* r = (CvRect*)cvGetSeqElem(pFaces, iFace);

		CvPoint pt1, pt2;
		pt1.x = r->x * 2;
		pt2.x = (r->x + r->width) * 2;
		pt1.y = r->y * 2;
		pt2.y = (r->y + r->height) * 2;

		(*Shapes)[iFace][0].x = r->x*2.0;
		(*Shapes)[iFace][0].y = r->y*2.0;
		(*Shapes)[iFace][1].x = (*Shapes)[iFace][0].x + 2.0*r->width;
		(*Shapes)[iFace][1].y = (*Shapes)[iFace][0].y + 2.0*r->height;
	}
	return true;
}

void free_shape_memeory(asm_shape** shapes)
{
	delete[](*shapes);
}


bool detect_one_face(asm_shape& Shape, const IplImage* image)
{
	asm_shape* detShape;
	int nfaces;

	bool flag = detect_all_faces(&detShape, nfaces, image);

	if (!flag) return false;

	// get most central face
	int iSelectedFace = 0;
	double x0 = image->width / 2., y0 = image->height / 2.;
	double x, y, d, D = 1e307;
	// max abs dist from center of face to center of image
	for (int i = 0; i < nfaces; i++)
	{
		x = (detShape[i][0].x + detShape[i][1].x) / 2.;
		y = (detShape[i][0].y + detShape[i][1].y) / 2.;
		d = sqrt((x - x0)*(x - x0) + (y - y0)*(y - y0));
		if (d < D)
		{
			D = d;
			iSelectedFace = i;
		}
	}
	Shape = detShape[iSelectedFace];
	delete[]detShape;

	return true;
}

/************************************************************************/
/* some stuff for video file                                            */
/************************************************************************/
static CvCapture* v_capture = NULL;		/**< Used for function cvQueryFrame */
static IplImage* v_capimg = NULL;		/**< Original image captured from video */
static IplImage *v_image = NULL;		/**< Flipped Image captured from video (when nessary) */

/************************************************************************/
/* some stuff for live camara                                            */
/************************************************************************/
static CvCapture* c_capture = NULL;		/**< Used for function cvQueryFrame */
static IplImage* c_capimg = NULL;		/**< Original image captured from video */
static IplImage *c_image = NULL;		/**< Flipped Image captured from video (when nessary) */


int open_video(const char* filename)
{
	v_capture = cvCaptureFromAVI(filename);
	if (!v_capture)
	{
		fprintf(stderr, "ERROR(%s, %d): CANNOT open video file %s!\n",
			__FILE__, __LINE__, filename);
		return -1;
	}

	cvSetCaptureProperty(v_capture, CV_CAP_PROP_POS_FRAMES, 0);
	v_capimg = cvQueryFrame(v_capture);
	v_image = cvCreateImage(cvGetSize(v_capimg), v_capimg->depth, v_capimg->nChannels);

	return (int)cvGetCaptureProperty(v_capture, CV_CAP_PROP_FRAME_COUNT);
}


void close_video()
{
	cvReleaseCapture(&v_capture);
	v_capture = 0;
	cvReleaseImage(&v_image);
	v_image = 0;
}

IplImage* read_from_video(int frame_no)
{
	if (frame_no < -1)
		return 0;

	else
	{
		cvSetCaptureProperty(v_capture, CV_CAP_PROP_POS_FRAMES, frame_no);
		v_capimg = cvQueryFrame(v_capture);
	}

	if (v_capimg == 0)
		return 0;

	if (v_capimg->origin == 0)
		cvCopy(v_capimg, v_image);
	else
		cvFlip(v_capimg, v_image);

	return v_image;
}

bool open_camera(int index)
{
	c_capture = cvCaptureFromCAM(index);
	if (!c_capture)	return false;

	cvGrabFrame(c_capture);
	c_capimg = cvQueryFrame(c_capture);
	c_image = cvCreateImage(cvGetSize(c_capimg), c_capimg->depth, c_capimg->nChannels);

	return true;
}

void close_camera()
{
	cvReleaseCapture(&c_capture);
	c_capture = 0;
	cvReleaseImage(&c_image);
	c_image = 0;
}

IplImage* read_from_camera()
{
	c_capimg = cvQueryFrame(c_capture);
	if (c_capimg == 0)
		return 0;

	if (c_capimg->origin == 0)
		cvCopy(c_capimg, c_image);
	else
		cvFlip(c_capimg, c_image);

	return c_image;
}
