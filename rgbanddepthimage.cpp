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

int main( int argc, char ** argv )
{
    VideoFrameRef oniDepthImg;
    Status result = STATUS_OK;
    result = OpenNI::initialize();


    // open device
    Device device;
    result = device.open( openni::ANY_DEVICE );
    
    /*
    const SensorInfo* const sensor = device.getSensorInfo(openni::SENSOR_COLOR);
    const Array<VideoMode>& sm = sensor->getSupportedVideoModes();

    for(int i=0; i< sm.getSize(); i++){
	cout << sm[i].getResolutionX() << " " << sm[i].getResolutionY() << " " << sm[i].getFps() << " ";
	cout << sm[i].getPixelFormat() << endl;
    } 
    */   

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
    
    VideoStream* pStream = &oniDepthStream;
    result = oniDepthStream.start();

    //set the RGB capture
    VideoCapture cap;
    cap.open(0);
    if(!cap.isOpened()){
	cout << "Open Failed\n";
        return false;
    }
	    
    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
    
    cout << 
    cap.get(CV_CAP_PROP_BRIGHTNESS) << endl <<
    cap.get(CV_CAP_PROP_CONTRAST) << endl <<
    cap.get(CV_CAP_PROP_SATURATION) << endl <<
    cap.get(CV_CAP_PROP_HUE) << endl ;
    
    cap.set(CV_CAP_PROP_BRIGHTNESS, 0.7);
    cap.set(CV_CAP_PROP_CONTRAST, 0.6);
    cap.set(CV_CAP_PROP_SATURATION, 0.7);
    cap.set(CV_CAP_PROP_HUE, 0.497);
    
    // start depth streama
    result = oniDepthStream.start();
    
    int zzz = 0;
    
    while( true )
    {
	
        
	while(zzz++ <= 30){
		Mat frame;
        	cap>>frame;
		cout << zzz << endl;
		continue;
	}
	Mat frame, rgb;
	cap >> frame;

        cvtColor(frame,rgb,CV_RGB2GRAY);
        //cv::imshow( "RGB", frame );
	    
	int changedStreamDummy;
	Status rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, 1);
	    
        if (rc == STATUS_OK){
		oniDepthStream.readFrame( &oniDepthImg ) ;
		
		const openni::DepthPixel* dt = (DepthPixel*) oniDepthImg.getData();
		int size = oniDepthImg.getDataSize();
		
		cout << size << endl;
		
		std::string tmp = "depthdata" + std::string(argv[1]) + ".txt";
		ofstream depthout( tmp.c_str() , ios::trunc);
		tmp = "colordata" + std::string(argv[1]) + ".txt";
		ofstream colorout(tmp.c_str(), ios::trunc);
		
		
		cout << "saving" << endl;
		
		uchar* start = frame.data;
		
		float wx,wy,wz;
		
		for(int x=0; x<HEIGHT; ++x){
			for(int y=0; y<WIDTH; ++y){
				openni::CoordinateConverter::convertDepthToWorld(oniDepthStream, x, y, dt[x*WIDTH + y], &wx, &wy, &wz);
				depthout << wx << ", " << wy << ", " << wz << endl;
				colorout << start[(x*WIDTH + y)*3 + 0] + 0 << ", " << start[(x*WIDTH + y)*3 + 1] + 0 << ", " << start[(x*WIDTH + y)*3 + 2] + 0<< ", " << endl;
			}
		}
		
		
		
		depthout.close();
		colorout.close();
		
		
		
            cv::Mat cDepthImg( oniDepthImg.getHeight(), oniDepthImg.getWidth(),
                CV_16UC1, (void*)oniDepthImg.getData() );
		
            cv::Mat c8BitDepth,c24Bit;
            cDepthImg.convertTo( c8BitDepth, CV_8U, 255.0 / (8000) );
	    
            //cv::imshow( "c8BitDepth", c8BitDepth );
	    
		cv::imwrite("depth.png", c8BitDepth);
		cv::imwrite("color.png", frame);
		return 0;
		
            //Mat dst;
            //addWeighted(c8BitDepth,0.5,rgb,0.5,0,dst);
            //cv::imshow( "Orbbec", dst );

        }
        waitKey(10);
    }

    oniDepthStream.destroy();

    device.close();
    OpenNI::shutdown();
    return 0;
}
