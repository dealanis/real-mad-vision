/*
 * Title: Etapa 01
 * Class: Vision para Robot
 * Instructor: Dr. Jose Luis Gordillo (http://robvis.mty.itesm.mx/~gordillo/)
 * Code: 
 * Institution: Tec de Monterrey, Campus Monterrey
 * Date: 12 February 2013
 * Description: Demonstration of first phase of the project.
 * This programs uses OpenCV http://www.opencv.org/
 */

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
#include <iostream>

#include <queue>
#include "CHeli.h"

#define XIMAGE 0
#define YIMAGE 20
#define XVIDEO 1250
#define XYIQ 0
#define YYIQ 570
#define XHSV 950
#define YHSV 570

#define RGB 0
#define YIQ 1
#define HSV 2


#define maxColors 10


using namespace cv;
using namespace std;

// Here we will store points
vector<Point> points;
Point orig, dest;

/* Create images where captured and transformed frames are going to be stored */
Mat currentImage;
Mat storedImage, backupImage;
Mat blackAndWhiteImage;
Mat binaryImage;
Mat grayScaleImage;
Mat HSVImage;
Mat closing;
//for parrot use
//CHeli *heli;
//CRawImage *image;

//other globals
bool greyScalePointReady;
Point greyScalePoint;



const Vec3b ColorMat[maxColors] = { //BGR
    Vec3b(255, 0, 0 ),      //blue
    Vec3b(0, 255, 0 ),      //green
    Vec3b(0, 0, 255 ),      //red
    Vec3b(255, 255, 0 ),    //cyan
    Vec3b(0, 255, 255 ),    //yellow
    Vec3b(255, 0, 255 ),    //magenta
    Vec3b(0, 154, 255 ),    //orange
    Vec3b(0, 128, 128 ),    //olive
    Vec3b(0, 100, 0 ),      //darkgreen
    Vec3b(128, 0, 0 )       //navy
};



void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param);
void mouseCallbackForBinaryImage(int event, int x, int y, int flags, void* param);
void generateBlackWhite(const Mat &origin, Mat &destination);
void generateBlackWhite1C(const Mat &origin, Mat &destination) ; //Hace la imagen para un solo canal
void generateBinary(const Mat &origin, const unsigned char limit, Mat &destination); //limit va de 0 a 255
void generateGrayScale(Mat &Matrix);
void rawToMat( Mat &destImage, CRawImage* sourceImage);
Vec3b getK(int &x);

void OilDrop(const Mat &dst, Mat &colorDst);

int main(int argc, char *argv[])
{
	/* First, open camera device */
    // Abre webcam
    	
 VideoCapture camera;
 camera.open(0);
 Mat currentImage;

    /* Open parrot camera instead */
    
    //establishing connection with the quadcopter
    // heli = new CHeli();
    
    //this class holds the image from the drone 
    //  image = new CRawImage(320,240);
    

    //init currentImage
   //Mat currentImage = Mat(240, 320, CV_8UC3);



    /* Create main OpenCV window to attach callbacks */
    namedWindow("Video");

    unsigned char key = 0;

/*To use timers
// **Start timer **
clock_t start;

assert((start = clock())!=-1);
*/

 

    while (key != 27)
	{
        /* 1 Obtain a new frame from camera web */
		camera >> currentImage;

        /* 2 Obtain image from Parrot instead */
        //image is captured
      //  heli->renewImage(image);
        // Copy to OpenCV Mat
     // rawToMat(currentImage, image);
        

		if (currentImage.data) 
		{
            /* Show image */
            imshow("Video", currentImage);
            
		}
		else
		{
			cout << "No image data.. " << endl;
		}

        if (key == 'c') { //capture snapshot
            storedImage.release();
            points.clear(); //remove all stored points on the vector
            storedImage = currentImage.clone();
            backupImage = currentImage.clone();
            namedWindow("Image");
            cvMoveWindow("Image", XIMAGE, YIMAGE);
            setMouseCallback("Image", mouseCoordinatesExampleCallback);
            imshow("Image", storedImage);


        }
        /***********************************
            Generate all processing
        ************************************/
        
        if (key == 10 && storedImage.rows > 0) 
        { //executes on enter
            generateBlackWhite1C(storedImage, blackAndWhiteImage);
           // greyScalePointReady = true;  
            namedWindow("Gray Scale Image");
            cvMoveWindow("Gray Scale Image", XIMAGE+200, YIMAGE+200);
            imshow("Gray Scale Image", blackAndWhiteImage);

            generateGrayScale(grayScaleImage);
            namedWindow("Grey Scale");
            cvMoveWindow("Grey Scale",XIMAGE,YIMAGE+100);
            imshow("Grey Scale", grayScaleImage);
            setMouseCallback("Grey Scale", mouseCallbackForBinaryImage);

            //initialization
            greyScalePoint.x = 200; //catches a 0.5 level in grey scale
            greyScalePoint.y = 0;
            greyScalePointReady = true;
            orig.x = dest.x = 0;
                   

        }
               

        if (greyScalePointReady && blackAndWhiteImage.data) 
        {   //executes on click on greyscale window
            //in grayScaleImage all channels are equal, pick only one channel
            unsigned char value = grayScaleImage.at<Vec3b>(greyScalePoint.y, greyScalePoint.x)[0];
            cout << "Gray Scale value: " << (int)value << endl;
            generateBinary(blackAndWhiteImage, value, binaryImage);
            greyScalePointReady = false;  

            //Closing is obtained by the dilation of an image followed by an erosion. 
            Mat dilateFilter;
            dilate(binaryImage, dilateFilter, Mat (10, 10, CV_8U));
            erode(dilateFilter, closing, Mat (10, 10, CV_8U));
            imshow("closing", closing);   
        

        }
        
        if (key=='s' && closing.rows > 0)
        {

        Mat oil;
        OilDrop(closing, oil);
        imshow("oildrop", oil);
        }


        /*************************************/
        key = waitKey(5);
	}
} //main

void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param)
{

    switch (event)
    {
        case CV_EVENT_LBUTTONDOWN:
            cout << "  Mouse X, Y: " << x << ", " << y << 
                " color: R " << (int)storedImage.at<Vec3b>(y, x)[2] <<
                " G " << (int)storedImage.at<Vec3b>(y, x)[1] <<
                " B " << (int)storedImage.at<Vec3b>(y, x)[0];
            cout << endl;

            break;

        case CV_EVENT_MOUSEMOVE:
            
            break;
        case CV_EVENT_LBUTTONUP:
            
            break;
    }
} //mouseCoordinatesExampleCallback

void mouseCallbackForBinaryImage(int event, int x, int y, int flags, void* param) 
{
    switch (event)
    {
        case CV_EVENT_LBUTTONDOWN:
            greyScalePoint.x = x;
            greyScalePoint.y = y;
            greyScalePointReady = true;
            break;
        default:
            break;
    }
} //mouseCallbackForBinaryImage

void generateBlackWhite(const Mat &origin, Mat &destination) 
{
    int average;

    if (destination.empty())
        destination = Mat(origin.rows, origin.cols, origin.type());

    int channels = origin.channels();
    
    for (int y = 0; y < origin.rows; ++y) 
    {
        uchar* sourceRowPointer = (uchar*) origin.ptr<uchar>(y);
        uchar* destinationRowPointer = (uchar*) destination.ptr<uchar>(y);
        for (int x = 0; x < origin.cols; ++x) {
            average = ( sourceRowPointer[x * channels] + sourceRowPointer[x * channels + 1] + sourceRowPointer[x * channels + 2] ) / 3;
            destinationRowPointer[x * channels    ] = (unsigned char)average;
            destinationRowPointer[x * channels + 1] = (unsigned char)average;
            destinationRowPointer[x * channels + 2] = (unsigned char)average;
        }
    }

} //generateBlackWhite

void generateBlackWhite1C(const Mat &origin, Mat &destination) 
{
    int average;

    if (destination.empty())
        destination = Mat(origin.rows, origin.cols, CV_8UC1);

    int channels = origin.channels();
    
    for (int y = 0; y < origin.rows; ++y) 
    {
        uchar* sourceRowPointer = (uchar*) origin.ptr<uchar>(y);
        uchar* destinationRowPointer = (uchar*) destination.ptr<uchar>(y);
        for (int x = 0; x < origin.cols; ++x) {
            average = ( sourceRowPointer[x * channels] + sourceRowPointer[x * channels + 1] + sourceRowPointer[x * channels + 2] ) / 3;
            destinationRowPointer[x] = (unsigned char)average; 
            //Solamente escribe el promedio en el unico canal de la imagen
        }
    }
}//generate blackwhite

void generateBinary(const Mat &origin, const unsigned char limit, Mat &destination) 
{
    if (destination.empty())
        destination = Mat(origin.rows, origin.cols, origin.type());

    int channels = origin.channels();
    
    for (int y = 0; y < origin.rows; ++y) 
    {
        uchar* sourceRowPointer = (uchar*) origin.ptr<uchar>(y);
        uchar* destinationRowPointer = (uchar*) destination.ptr<uchar>(y);
        for (int x = 0; x < origin.cols; ++x) {
            for (int i = 0; i < channels; ++i) {
                if (sourceRowPointer[x * channels + i] < limit )
                    destinationRowPointer[x * channels + i] = 0;
                else
                    destinationRowPointer[x * channels + i] = 255;
            }
        }
    }


} //generateBinary



void generateGrayScale(Mat &Matrix) 
{
    Matrix.create(25,255,CV_8UC3);

    int channels = Matrix.channels();
    
    for (int y = 0; y < Matrix.rows; ++y) 
    {
        uchar* rowPointer = (uchar*) Matrix.ptr<uchar>(y);
        for (int x = 0; x < Matrix.cols; ++x)
            for (int i = 0; i < channels; ++i)
            {
                rowPointer[x * channels + i] = x;
            }
    }
} //generateGrayScale

// Convert CRawImage to Mat *for parrot use*
void rawToMat( Mat &destImage, CRawImage* sourceImage)
{   
    uchar *pointerImage = destImage.ptr(0);
    
    for (int i = 0; i < 240*320; i++)
    {
        pointerImage[3*i] = sourceImage->data[3*i+2];
        pointerImage[3*i+1] = sourceImage->data[3*i+1];
        pointerImage[3*i+2] = sourceImage->data[3*i];
    }
} //rawToMat

Vec3b getK(int &x) { 

    Vec3b vector = ColorMat[x];
    x++;
    if (x >= maxColors)
        x = 0;
    return vector;
}


//Imagen binaria, Imagen destino
void OilDrop(const Mat &dst, Mat &colorDst) {
    Vec3b k;
    uchar aux;
    int x=0;
    Vec3b negro=Vec3b(0, 0, 0); //Use to compare with black color
    
    int state=1;  //Variable for state machine for exploring
    int factor=1; // Variable to explore in spiral

    int areas[10]; //Variable to get the size of the blobs

    if (colorDst.empty())
    colorDst = Mat(dst.rows, dst.cols,  CV_8UC3);
    colorDst=Scalar::all(0);  //Start with the empty image
    
    for (int nareas = 0; nareas < 8; nareas++) { //color 5 areas max
        int seedCol = dst.cols/2;
        int seedRow = dst.rows/2;
        Point seed = Point(seedCol, seedRow);   //first seed, on the middle of the image

        int c = 30;

        do {

            switch(state)
            {   
                case 1:
                seedRow += factor;  //move right
                factor *= 2;        //increase factor
                state = 2;
                break;
                
                case 2:
                seedCol -= factor;  //move up
                factor *= 2;        //increase factor
                state = 3;
                break;
                
                case 3:
                seedRow -= factor;  //move left
                factor *= 2;        //increase factor
                state = 4;
                break;
                
                case 4:
                seedCol += factor;  //move down
                factor *= 2;        //increase factor
                state = 1;
                break;
                
                case 5:
                seedCol = rand() % dst.cols;    //random
                seedRow = rand() % dst.rows;
                c--;
                break;
            }

            if(seedRow >= dst.rows || seedRow < 0)
            {
                seedRow = dst.rows/2;
                state = 5;  //Change to random
            }

            if(seedCol >= dst.cols || seedCol < 0)
            {
                seedCol = dst.cols/2;
                state = 5;  //Change to random
            }

            seed = Point(seedCol, seedRow); //Generate seed

      
        } while ((dst.at<uchar>(seed) != 255 || (colorDst.at<Vec3b>(seed) != negro) ) && c > 0); //find a white starting point, if it doesn't find one in c chances, skip

        if (c > 0) {
        
            k = getK(x);
            areas[x-1]=1;
            queue<Point> Fifo;
            Fifo.push(seed);

            colorDst.at<Vec3b>(seed) = k;   // color the seed pixel
            Point coord;
            
            while (!Fifo.empty()) {
                for (int state = 0; state < 4; state++) {   //check all 4 sides
                    switch (state) {
                    case 0: //north
                        coord.x = Fifo.front().x;
                        coord.y = Fifo.front().y - 1;
                        break;
                    case 1: //west
                        coord.x = Fifo.front().x - 1;
                        coord.y = Fifo.front().y;
                        break;
                    case 2: //south
                        coord.x = Fifo.front().x;
                        coord.y = Fifo.front().y + 1;
                        break;
                    case 3: //east
                        coord.x = Fifo.front().x + 1;
                        coord.y = Fifo.front().y;
                        break;
                    }

                    if (coord.y >= 0 && coord.y < dst.rows && coord.x >= 0 && coord.x < dst.cols) { //is in range
                        Vec3b temp = colorDst.at<Vec3b>(coord);
                        if (temp != k) {                            //AND is not colored with k on the destination yet
                         //   aux = dst.at<uchar>(coord);
                            if (dst.at<uchar>(coord) != 0) {        //AND is not 0 in the binary image
                                colorDst.at<Vec3b>(coord) = k;      //color the destination
                                Fifo.push(Point(coord));            //enqueue this point for analysis
                                areas[x-1]++;
                            }
                        }
                    }
                }

                Fifo.pop();
            }

        }
    }
    cout<<"Region \tTamaño "<<endl;
    for (int i=0;i<x;i++)
    cout<<i+1<<"\t"<<areas[i]<<endl;
}