//
//  ofxSchedule.h
//  LEDManager
//
//  Created by SeJun Jeong on 13. 2. 4..
//
//

#ifndef LEDManager_ofxSchedule_h
#define LEDManager_ofxSchedule_h

#include <sys/time.h>
#include "ofxXmlSettings.h"

/* millis
 timeval t;
 gettimeofday(&t, NULL);
 cout << "	 "<< t.tv_usec<<endl;
*/

enum ofxScheduleTimeType {
	OFX_SCHEDULE_ABSOULTE,
	OFX_SCHEDULE_RELATIVE,
	OFX_SCHEDULE_REPEAT,
	OFX_SCHEDULE_EVERY
};

ofxScheduleTimeType getScheduleTimeTypeFromString(string type){
	if(type == "absolute"){
		return OFX_SCHEDULE_ABSOULTE;
	}else if(type == "relative"){
		return OFX_SCHEDULE_RELATIVE;
	}else if(type == "repeat"){
		return OFX_SCHEDULE_REPEAT;
	}else if(type == "every"){
		return OFX_SCHEDULE_EVERY;
	}
}

class ofxScheduleTime {
public:
	ofxScheduleTime(){
		init(tm());
	}
	
	static time_t normalizeTime(time_t t){
		tm l = *localtime(&t);
		l.tm_year = 70;
		l.tm_mon = 0;
		l.tm_mday = 1;
		l.tm_isdst = 0;
		return mktime(&l)-getLocalEpoch();
	}
	
	static time_t getLocalEpoch(){
		static tm utc;
		utc.tm_year = 70;
		utc.tm_mon = 0;
		utc.tm_mday = 1;
		utc.tm_hour = 0;
		utc.tm_min = 0;
		utc.tm_sec = 0;
		utc.tm_isdst = 0;
		return mktime(&utc);
	}
	
	ofxScheduleTime(int hour, int min, int sec, ofxScheduleTimeType newType=OFX_SCHEDULE_ABSOULTE){
		tm newTime;
		newTime.tm_hour = hour;
		newTime.tm_min = min;
		newTime.tm_sec = sec;
		init(newTime, newType, true);
	}
	
	ofxScheduleTime(tm newTime, ofxScheduleTimeType newType=OFX_SCHEDULE_ABSOULTE, bool local = true){
		init(newTime, newType, local);
	}
	
	ofxScheduleTimeType getType(){
		return type;
	}
	
	int getHour(){
		return time.tm_hour;
	}
	int getMinute(){
		return time.tm_min;
	}
	int getSec(){
		return time.tm_sec;
	}
	
	int getMillis(){
		return -1;
	}
	
	float operator / (ofxScheduleTime& other){
		time_t othert = mktime(&other.time);
		if(othert == 0) return;
		time_t unix = mktime(&time)/othert;
		return unix;
	}
		
	ofxScheduleTime operator * (float& factor){
		time_t unix = mktime(&time)*factor;
		ofxScheduleTime time;
		time.setTime(unix);
		return time;
	}
	
	operator float(){
		
	}
	
	operator int(){
		return getMillis();
	}
	
	void setTime(tm newTime){
		time = newTime;
	}
	
	void setTime(time_t unix){
		time = *localtime(&unix);
	}
	
	tm getTime(){
		return time;
	}
	
	time_t getUnixTime(){
		return mktime(&time);
	}
	
private:
	ofxScheduleTimeType type;
	tm time;
		void init(tm newTime, ofxScheduleTimeType newType = OFX_SCHEDULE_ABSOULTE, bool local = true){
			time = newTime;
			type = newType;
			static tm utc;
			utc.tm_year = 70;
			utc.tm_mon = 0;
			utc.tm_mday = 1;
			utc.tm_hour = 0;
			utc.tm_min = 0;
			utc.tm_sec = 0;
			utc.tm_isdst = 0;
			static const float utci = mktime(&utc);
			if(local) {
				//if(time.tm_year < 70 || time.tm_year>1000)
				time.tm_year = 70;
				time.tm_mon = 0;
				time.tm_mday =1;
				time.tm_isdst =-1;
				time_t normal = mktime(&time) - utci;
				time = *localtime(&normal);
			}
		}
};

class ofxScheduleEvent {
public:
	ofxScheduleEvent():isOn(false),loop(1){}
	ofxScheduleEvent(string type, string message):type(type),message(message),isOn(false),loop(1){}
	
	string getType(){
		return type;
	}	
	string getMessage(){
		return message;
	}
	ofxScheduleTime getBeginTime(){
		return beginTime;
	}
	ofxScheduleTime getDuration(){
		return duration;
	}
	ofxScheduleTime getEndTime(){
		return duration;
	}
	float getLoop(){
		return loop;
	}
	
	string setType(string name){
		return type = name;
	}
	string setMessage(string m){
		return type = m;
	}
	ofxScheduleTime setBeginTime(ofxScheduleTime time){
		return beginTime = time;
	}
	
	ofxScheduleTime setDuration(ofxScheduleTime time){
		//if(endTime.getType() == OFX_SCHEDULE_RELATIVE){
		endTime = time*loop;
		//}
		return duration = time;
	}
	
	float setLoop(float times){
		//endTime;
		//if(duration)loop = time/duration;
		//else loop = -1;
		endTime = duration*times;
		return loop = times;
	}
	
	ofxScheduleTime setEndTime(ofxScheduleTime time){
		if(duration.getUnixTime()>0)loop = time/duration;
		//else loop = 1;
		return endTime = time;
	}
	
	bool getIsOn(){
		return isOn;
	}
	
	void setIsOn(bool on){
		isOn = on;
	}
	
private:
	string type;
	string message;
	ofxScheduleTime beginTime;
	ofxScheduleTime duration;
	ofxScheduleTime endTime;
	float loop;
	bool isOn;
};


class ofxSchedule : ofThread{
public:
	ofxSchedule():previousTime(0){
	}
	
	void start(){
		startThread();
	}
	
	void addSchedule(ofxScheduleEvent schedule){
		events.push_back(&schedule);
		liveEvents.push_back(&schedule);
	}
	
	void loadSchedule(string path){
		XML.loadFile(path);
		XML.pushTag("schedule");
		int num = XML.getNumTags("event");
		ofLogVerbose("ofxSchedule") << num << " events loaded";
		for(int i=0; i<num; i++){
			XML.pushTag("event",i);
			ofxScheduleEvent event;
			
			event.setType(XML.getValue("type", "default type"));
			event.setMessage(XML.getValue("message", "default message"));
			
			
			XML.pushTag("begin");
			event.setBeginTime(ofxScheduleTime(XML.getValue("time:hour", 0),
									XML.getValue("time:minute", 0),
									XML.getValue("time:second", 0),
									getScheduleTimeTypeFromString(XML.getAttribute("time", "type", "relative"))
									));
			XML.popTag();
			
			XML.pushTag("duration");
			event.setDuration(ofxScheduleTime(XML.getValue("time:hour", 0),
									XML.getValue("time:minute", 0),
									XML.getValue("time:second", 0)
									));
			XML.popTag();
			
			XML.pushTag("end");
			if(XML.tagExists("loop")){
				event.setLoop(XML.getValue("loop", 0.0f));
			}else if(XML.tagExists("time")){
				event.setEndTime(ofxScheduleTime(XML.getValue("time:hour", 0),
												   XML.getValue("time:minute", 0),
												   XML.getValue("time:second", 0),
												   getScheduleTimeTypeFromString(XML.getAttribute("time", "type", "relative"))
												   ));
			}
			XML.popTag();
			
			addSchedule(event);
			XML.popTag();
		}
		XML.popTag();
	}
	
	ofEvent<ofxScheduleEvent> beginEvent;
	ofEvent<ofxScheduleEvent> endEvent;
private:
	void threadedFunction(){
		while(isThreadRunning()){
			if(liveEvents.size()>0){
				time_t now =ofxScheduleTime::normalizeTime(time(NULL));
				time_t begin = previousTime + liveEvents.front()->getBeginTime().getUnixTime();
				time_t end = begin + liveEvents.front()->getDuration().getUnixTime();
				if(now >= begin){
					if(!liveEvents.front()->getIsOn()){
						ofNotifyEvent(beginEvent, *liveEvents.front(), this);
						liveEvents.front()->setIsOn(true);
 					}
					if(now >= end){
						ofNotifyEvent(endEvent, *liveEvents.front(), this);
						previousTime = end;
						liveEvents.pop_front();
					}
				}
			}
		}
	}
	time_t previousTime;
	deque<ofxScheduleEvent*> liveEvents; //vector<ofxScheduleEvent*> liveEvents;
	vector<ofxScheduleEvent*> events;
	ofxXmlSettings XML;
};

#endif
