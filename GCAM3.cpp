#include <wiringPi.h>
#include <pthread>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <gst/gst.h>
#include <mosquitto.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


using namespace std;

typedef struct _CustomData {
  	GstElement *pipeline;
	bool streaming_started;
	//char current_gst_command[100];
         /* Our one and only pipeline */
	
} CustomData;

void heartBeat(struct mosquitto *mosq){
  int rc = mosquitto_connect(mosq, "114.33.252.156", 1883, 10);
  String msg("HEARTBEAT CHARLIE")
  mosquitto_publish(mosq, NULL, "test/t1", msg.length()+1, msg.toStdString().c_str(), 1, false);
}

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
	if(rc) {
		cout << "Error with result code:"<<rc<<endl;
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "USV-CMD/USV-Charlie", 0);
}

void switchCamera(int num){
	if(num == 1){
  		system("i2cset -y 1 0x70 0x00 0x04");
  		digitalWrite(7,0);
  		digitalWrite(0,0);
  		digitalWrite(1,1);
	}else if(num == 2){
  		system("i2cset -y 1 0x70 0x00 0x05");
  		digitalWrite(7,1);
  		digitalWrite(0,0);
  		digitalWrite(1,1);
	}else if(num == 3){
  		system("i2cset -y 1 0x70 0x00 0x06");
  		digitalWrite(7,0);
  		digitalWrite(0,1);
  		digitalWrite(1,0);
	}else if(num == 4){
  		system("i2cset -y 1 0x70 0x00 0x07");
  		digitalWrite(7,1);
  		digitalWrite(0,1);
  		digitalWrite(1,0);
	}
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	CustomData* data = (CustomData*) obj;
	string raw_msg((char*)msg->payload);
	string cap;
	cout<<"New message with topic:"<<raw_msg<<endl;
	size_t space_pos = raw_msg.find(' ');
	if(space_pos == string::npos){
		cap = raw_msg;
	}else{
		cap = string(raw_msg,0, space_pos);
	}
	if(cap.compare(string("START")) == 0){
		if(data->streaming_started == false){
			data->pipeline = gst_parse_launch("gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=-1 ! video/x-raw, width=160, height=120, framerate=12/1 ! videoconvert ! jpegenc ! rtpjpegpay ! udpsink host=10.8.0.4 port=5200", NULL);
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
			data->streaming_started = true;
			cout<<"START..."<<endl;
		}else{
			gst_element_set_state (data->pipeline, GST_STATE_NULL);
  			//gst_object_unref (data->pipeline);
			data->pipeline = gst_parse_launch("gst-launch-1.0 -v v4l2src device=/dev/video0 num-buffers=-1 ! video/x-raw, width=640, height=480, framerate=12/1 ! videoconvert ! jpegenc ! rtpjpegpay ! udpsink host=10.8.0.4 port=5200", NULL);
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		}
	}else if(cap.compare(string("GST")) == 0){
		string gst_command(raw_msg,space_pos+1,raw_msg.length()-space_pos-1);
		if(data->streaming_started == false){
			data->pipeline = gst_parse_launch(gst_command.c_str(), NULL);
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
			data->streaming_started = true;
		}else{
			gst_element_set_state (data->pipeline, GST_STATE_NULL);
			data->pipeline = gst_parse_launch(gst_command.c_str(), NULL);
			gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		}
		//strcpy(data->current_gst_command, gst_command.c_str());
		cout<<"GST_COMMAND : "<<gst_command<<endl;
	
	}else if(cap.compare(string("SWITCH")) == 0){
		gst_element_set_state (data->pipeline, GST_STATE_NULL);
		string cam_id(raw_msg,space_pos+1,raw_msg.length()-space_pos-1);
		if(cam_id.compare(string("A")) == 0){
			switchCamera(1);
			cout<<"SWITCH : to camera : "<<"A"<<endl;
		}else if(cam_id.compare(string("B")) == 0){
			switchCamera(2);
			cout<<"SWITCH : to camera : "<<"B"<<endl;
		}else if(cam_id.compare(string("C")) == 0){
			switchCamera(3);
			cout<<"SWITCH : to camera : "<<"C"<<endl;
		}else if(cam_id.compare(string("D")) == 0){
			switchCamera(4);
			cout<<"SWITCH : to camera : "<<"D"<<endl;
		}else{
			cout<<"SWITCH : error cam_id "<<cam_id<<endl;
		}
		
		//data->pipeline = gst_parse_launch(data->current_gst_command, NULL);
		//gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
		
	}else if(cap.compare(string("QUIT")) == 0){
		if(data->streaming_started == true){
			gst_element_set_state (data->pipeline, GST_STATE_NULL);
  			gst_object_unref (data->pipeline);
			data->streaming_started = false;
			cout<<"quit..."<<endl;
		}
	}else{
		cout<<"No matching cmd:"<<cap<<endl;
	}
}

int main(int argc, char *argv[]) {
	int rc, id=12;
	
	CustomData data;
	
	data.streaming_started=false;

	mosquitto_lib_init();
	/* Initialize GStreamer */
	
  	gst_init (&argc, &argv);
	
   	int fd = open ("/dev/i2c-1",O_RDWR);
    	if(!fd){
        	printf("Couldn't open i2c device, please enable the i2c1 firstly\r\n");
        	return -1;
   	}
  	wiringPiSetup();
  	pinMode(7, OUTPUT); //set GPIO 7 to output
  	pinMode(0, OUTPUT); //set GPIO 11 to output
  	pinMode(1, OUTPUT); //set GPIO 12 to output
  	system("sudo modprobe bcm2835_v4l2");
  	system("i2cset -y 1 0x70 0x00 0x06");
  	int access(const char *filename, int mode);
  	if(access("/dev/video0",0)){
        	printf("Please check your camera connection,then try again.\r\n");
        	exit(0);
   	}
  	digitalWrite(7,0);
  	digitalWrite(0,1);
  	digitalWrite(1,0);


	struct mosquitto *mosq;
	mosq = mosquitto_new( "USV-CMD/USV-Charlie" , true, &data);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	rc = mosquitto_connect(mosq, "114.33.252.156" , 1883, 10);
	if(rc) {
		cout<<"Could not connect to Broker with return code %d\n"<<rc<<endl;
		return -1;
	}
	
	mosquitto_loop_start(mosq);
  	pthread_t heartBeatThread;
	pthread_create(&heartBeatThread, NULL, heartBeat, mosq);
	
	cout<<"Press Enter to quit...\n";
	getchar();
  	pthread_join(heartBeatThread,NULL);
	mosquitto_loop_stop(mosq, true);
	mosquitto_disconnect(mosq);
	
	/* Free resources */
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	if(data.streaming_started){
 		gst_element_set_state (data.pipeline, GST_STATE_NULL);
  		gst_object_unref (data.pipeline);
	}


	return 0;
}
