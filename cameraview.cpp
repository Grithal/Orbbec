#include <iostream>
#include <OpenNI.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

using namespace openni;

const int WIDTH = 640;
const int HEIGHT = 480;

int main( int /*argc*/, char ** /*argv*/ )
{
    VideoFrameRef oniDepthImg;
    Status result = STATUS_OK;
    result = OpenNI::initialize();


    // open device
    Device device;
    result = device.open( openni::ANY_DEVICE );
    
    const SensorInfo* const sensor = device.getSensorInfo(openni::SENSOR_DEPTH);
    const Array<VideoMode>& sm = sensor->getSupportedVideoModes();

    for(int i=0; i< sm.getSize(); i++){
	cout << sm[i].getResolutionX() << " " << sm[i].getResolutionY() << " " << sm[i].getFps() << " ";
	cout << sm[i].getPixelFormat() << endl;
    }    

    //// create depth stream
    VideoStream oniDepthStream;
    result = oniDepthStream.create( device, openni::SENSOR_DEPTH );

    //const SensorInfo const sensor = oniDepthStream.getSensorInfo();

//    // set depth video mode
    VideoMode modeDepth;
    modeDepth.setResolution( WIDTH, HEIGHT );
    modeDepth.setFps( 30 );
    modeDepth.setPixelFormat( PIXEL_FORMAT_DEPTH_1_MM );
    oniDepthStream.setVideoMode(modeDepth);
    oniDepthStream.setMirroringEnabled(false);
    device.setImageRegistrationMode( /*ImageRegistrationMode::*/IMAGE_REGISTRATION_DEPTH_TO_COLOR );

    //set the RGB capture
    VideoCapture cap;
    cap.open(0);

    if(!cap.isOpened())
        return false;

    VideoStream* pStream = &oniDepthStream;
    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
    // start depth stream
    result = oniDepthStream.start();
    while( true )
    {

        Mat frame,rgb;
        cap>>frame;

        /*if(frame.empty())
            break;*/
        cvtColor(frame,rgb,CV_RGB2GRAY);
        cv::imshow( "RGB", frame );
	int changedStreamDummy;
	
	Status rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, 1);
       // if( oniDepthStream.readFrame( &oniDepthImg ) == STATUS_OK )
        if (rc == STATUS_OK){
		oniDepthStream.readFrame( &oniDepthImg ) ;
            cv::Mat cDepthImg( oniDepthImg.getHeight(), oniDepthImg.getWidth(),
                CV_16UC1, (void*)oniDepthImg.getData() );

            cv::Mat c8BitDepth,c24Bit;
            cDepthImg.convertTo( c8BitDepth, CV_8U, 255.0 / (8000) );
            cv::imshow( "c8BitDepth", c8BitDepth );
		cv::imwrite("asd.png", c8BitDepth);
            Mat dst;
            addWeighted(c8BitDepth,0.5,rgb,0.5,0,dst);
            cv::imshow( "Orbbec", dst );

        }
        waitKey(10);
    }

    oniDepthStream.destroy();

    device.close();
    OpenNI::shutdown();
    return 0;
}
