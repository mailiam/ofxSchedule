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
	
	static time_t normalizeTime(time_t t){
		tm l = *localtime(&t);
		l.tm_year = 70;
		l.tm_mon = 0;
		l.tm_mday = 1;
		l.tm_isdst = 0;
		time_t e = getLocalEpoch();
		
		if(t<0){
			if(t < -e){
				return toEpoch(&l)-e;
			}else{
				return toEpoch(&l)+e;
			}
		}else{
			if(t < -e){
				return toEpoch(&l)+e;
			}else{
				return toEpoch(&l)-e;
			}
		}
		
	}
	
	static tm getLocal(){
		static tm utc;
		utc.tm_year = 70;
		utc.tm_mon = 0;
		utc.tm_mday = 1;
		utc.tm_hour = 0;
		utc.tm_min = 0;
		utc.tm_sec = 0;
		utc.tm_isdst = 0;
		return utc;
	}
	
	static time_t getLocalEpoch(int utcdiff = 0){
		static tm utc = getLocal();
		return toEpoch(&utc,utcdiff);
	}
	
	static time_t toEpoch(const struct tm *ltm, int utcdiff = 0){
		const int mon_days [] =
		{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		long tyears, tdays, leaps, utc_hrs;
		int i;
		
		tyears = ltm->tm_year - 70 ; // tm->tm_year is from 1900.
		leaps = (tyears + 2) / 4; // no of next two lines until year 2100.
		//i = (ltm->tm_year – 100) / 100;
		//leaps -= ( (i/4)*3 + i%4 );
		tdays = 0;
		for (i=0; i < ltm->tm_mon; i++) tdays += mon_days[i];
		
		tdays += ltm->tm_mday-1; // days of month passed.
		tdays = tdays + (tyears * 365) + leaps;
		
		utc_hrs = ltm->tm_hour + utcdiff; // for your time zone.
		return (tdays * 86400) + (utc_hrs * 3600) + (ltm->tm_min * 60) + ltm->tm_sec;
	}
	
	ofxScheduleTime(int hour, int min, int sec, ofxScheduleTimeType newType=OFX_SCHEDULE_ABSOULTE){
		tm newTime;
		newTime.tm_hour = hour - 9;
		newTime.tm_min = min;
		newTime.tm_sec = sec;
		init(newTime, newType, true);
	}
	
	ofxScheduleTime(tm newTime, ofxScheduleTimeType newType=OFX_SCHEDULE_ABSOULTE, bool local = true){
		init(newTime, newType, local);
	}
	
	ofxScheduleTime(float time=0, ofxScheduleTimeType newType=OFX_SCHEDULE_RELATIVE){
		time_t uTime = time + getLocalEpoch(-9);
		tm newTime =* localtime(&uTime);
		init(newTime, newType, false);
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
	
	void setHour(int h){
		time.tm_hour = h;
	}
	void setMinute(int m){
		time.tm_min = m;
	}
	void setSec(int s){
		time.tm_sec = s;
	}
	
	int getMillis(){
		return -1;
	}
	
	float operator / (ofxScheduleTime& other){
		time_t othert = toEpoch(&other.time);
		if(othert == 0) return;
		time_t unix = toEpoch(&time)/othert;
		return unix;
	}
	
	ofxScheduleTime operator * (float& factor){
		time_t unix = toEpoch(&time)*factor;
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
		return toEpoch(&time);
	}
	
private:
	ofxScheduleTimeType type;
	tm time;
	void init(tm newTime, ofxScheduleTimeType newType = OFX_SCHEDULE_ABSOULTE, bool local = true){
		time = newTime;
		type = newType;
		if(local) {
			static tm utc;
			utc.tm_year = 70;
			utc.tm_mon = 0;
			utc.tm_mday = 1;
			utc.tm_hour = 0;
			utc.tm_min = 0;
			utc.tm_sec = 0;
			utc.tm_isdst = 0;
			static const time_t utci = toEpoch(&utc);
			
			//if(time.tm_year < 70 || time.tm_year>1000)
			time.tm_year = 70;
			time.tm_mon = 0;
			time.tm_mday =1;
			time.tm_isdst =-1;
			time_t normal = toEpoch(&time) - utci;
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
	ofxScheduleTime *getBeginTime(){
		return &beginTime;
	}
	ofxScheduleTime *getDuration(){
		return &duration;
	}
	ofxScheduleTime *getEndTime(){
		return &duration;
	}
	float getLoop(){
		return loop;
	}
	
	string setType(string name){
		return type = name;
	}
	string setMessage(string m){
		return message = m;
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
	ofxSchedule(){
	}
	
	void start(){
		startThread();
	}
	
	void stop(){
		stopThread();
	}
	
	bool isRunning(){
		return isThreadRunning();
	}
	
	void setPlay(bool play){
		if(play) start();
		else stop();
	}
	
	void addSchedule(ofxScheduleEvent * schedule){
		if(lock()){
			events.push_back(schedule);
			unlock();
		}
	}
	
	void removeSchedule(ofxScheduleEvent * schedule){
		if(lock()){
			for(int i=0; i<events.size(); i++){
				if (events[i] == schedule){
					events.erase(events.begin()+i);
					break;
				}
			}
			if (schedule->getIsOn()) {
				ofLogVerbose("ofxSchedule")<< ofGetTimestampString() <<":ENDING	" << schedule->getType() << " < " << schedule->getMessage();
				ofNotifyEvent(endEvent, *schedule, this);
				schedule->setIsOn(false);
			}
			delete schedule;
			unlock();
		}
	}
	
	void loadSchedule(string path){
		XML.loadFile(path);
		XML.pushTag("schedule");
		int num = XML.getNumTags("event");
		ofLogVerbose("ofxSchedule") << num << " events loaded";
		for(int i=0; i<num; i++){
			XML.pushTag("event",i);
			ofxScheduleEvent * event = new ofxScheduleEvent();
			
			event->setType(XML.getValue("type", "default type"));
			event->setMessage(XML.getValue("message", "default message"));
			
			
			XML.pushTag("begin");
			event->setBeginTime(ofxScheduleTime(XML.getValue("time:hour", 0),
												XML.getValue("time:minute", 0),
												XML.getValue("time:second", 0),
												getScheduleTimeTypeFromString(XML.getAttribute("time", "type", "relative"))
												));
			XML.popTag();
			
			XML.pushTag("duration");
			event->setDuration(ofxScheduleTime(XML.getValue("time:hour", 0),
											   XML.getValue("time:minute", 0),
											   XML.getValue("time:second", 0)
											   ));
			XML.popTag();
			
			XML.pushTag("end");
			if(XML.tagExists("loop")){
				event->setLoop(XML.getValue("loop", 0.0f));
			}else if(XML.tagExists("time")){
				event->setEndTime(ofxScheduleTime(XML.getValue("time:hour", 0),
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
	
	int numEvents(){
		return events.size();
	}
	
	ofxScheduleEvent* getEvent(int i){
		lock();
		///ofxScheduleEvent event = ofxScheduleEvent(*events[i]);
		unlock();
		return events[i];
	}
	
	ofEvent<ofxScheduleEvent> beginEvent;
	ofEvent<ofxScheduleEvent> endEvent;
private:
	void threadedFunction(){
		while(isThreadRunning()){
			time_t now =ofxScheduleTime::normalizeTime(time(NULL));
			time_t begin = 0;//previousTime + liveEvents.front()->getBeginTime()->getUnixTime();
			time_t end = 0;//begin + liveEvents.front()->getDuration()->getUnixTime();
			
			for(int i=0; i<events.size(); i++){
				ofxScheduleEvent *e = events[i];
				begin += e->getBeginTime()->getUnixTime();
				end = begin+e->getDuration()->getUnixTime();
				if(now >= begin){
					if(!e->getIsOn() && now < end){
						ofLogVerbose("ofxSchedule")<< ofGetTimestampString() <<":BEGINNING	" << e->getType() << " < " << e->getMessage();

						ofNotifyEvent(beginEvent, *e, this);
						e->setIsOn(true);
					}else if(now >= end && e->getIsOn()){
						ofLogVerbose("ofxSchedule")<< ofGetTimestampString() <<":ENDING	" << e->getType() << " < " << e->getMessage();
						ofNotifyEvent(endEvent, *e, this);
						e->setIsOn(false);
					}
				}else if(e->getIsOn()){
					ofLogVerbose("ofxSchedule")<< ofGetTimestampString() <<":ENDING	" << e->getType() << " < " << e->getMessage();
					ofNotifyEvent(endEvent, *e, this);
					e->setIsOn(false);
				}
				begin = end;
				ofSleepMillis(1);
			}
		}
	}
	vector<ofxScheduleEvent*> events;
	ofxXmlSettings XML;
};

#endif
