//
//  ofxUISchedule.h
//  ofxUISchedule
//
//  Created by SeJun Jeong on 13. 1. 29..
//
//

#ifndef ofxUISchedule_h
#define ofxUISchedule_h

#define SCROLL_WIDTH 15
#define SEC_ONE_DAY (24*60*60)
#define TIME_LABEL_WIDTH 50

#include "ofxUI.h"
#include "ofxUIMediaAsset.h"

class ofxUISchedule : public ofxUICanvas
{
public:
    ofxUISchedule() : ofxUICanvas()
    {
        
    }
    ofxUISchedule(string _name, ofxSchedule _schedule, float w, float h, float x = 0, float y = 0, int _size = OFX_UI_FONT_SMALL) : ofxUICanvas(x, y, w, h){
        useReference = false;
        initDirList(_name, &_schedule);
    }
    ofxUISchedule(string _name, ofxSchedule *_schedule, float w, float h, float x = 0, float y = 0, int _size = OFX_UI_FONT_SMALL) : ofxUICanvas(x, y, w, h){
        useReference = true;
        initDirList(_name, _schedule);
    }
    ofxUISchedule(string _name, float w, float h, float x = 0, float y = 0, int _size = OFX_UI_FONT_SMALL) : ofxUICanvas(x, y, w, h){
        useReference = false;
        ofxSchedule _schedule;
        initDirList(_name, &_schedule);
    }
    
    void initDirList(string _name, ofxSchedule *_schedule)
    {
        kind = OFX_UI_WIDGET_DIRLIST;
        setWidgetSpacing(0);
		
		name = _name;
		
        if(useReference)
        {
            schedule = _schedule;
        }
        else
        {
            schedule = new ofxSchedule();
			ofLogError("ofxUIScehdule")<<"NO REFERENCE NOT IMPLEMENTED";
            //*schedule = *_schedule;
        }
		
		selectedAsset = NULL;
		
		label = new ofxUILabel(0,0,(name+" LABEL"), name, OFX_UI_FONT_MEDIUM);
		addEmbeddedWidget(label);
		
		listUI = (ofxUIScrollableCanvas* )addWidgetLeft(new ofxUIScrollableCanvas(padding, 0, rect->width - SCROLL_WIDTH-padding, rect->height));
        listUI->setDamping(0.1);
		listUI->setScrollableDirections(false, false);
		listUI->setStickyDistance(5);
		listUI->setWidgetSpacing(0);
		listUI->setPadding(0);
		listUI->setDrawBack(false);
		
		secHeight = listUI->getRect()->getHeight()/SEC_ONE_DAY/2;
		listUI->getSRect()->setHeight(secHeight*SEC_ONE_DAY);

		float yy =0;
        for(int i=0; i<schedule->numEvents(); i++){
			ofxScheduleEvent * event = schedule->getEvent(i);
			float h = event->getDuration().getUnixTime()*secHeight;
			float y = yy + event->getBeginTime().getUnixTime()*secHeight;
			ofxUIMediaAsset * widget = new ofxUIMediaAsset(event->getMessage(),listUI->getRect()->width-TIME_LABEL_WIDTH, h, TIME_LABEL_WIDTH, y);
            listUI->addWidget(widget);
			widget->setColorBack(ofColor::fromHsb((y/listUI->getSRect()->height)*255, 255, 255, 128));
			
			yy+=widget->getRect()->height;

        }
		
		scrollbar = (ofxUIRangeSlider *) addWidgetRight(new ofxUIRangeSlider("SCROLLBAR", 24, 0, 24, 12, SCROLL_WIDTH, rect->getHeight()));
		scrollbar->setLabelVisible(false);
		scrollbar->setLabelPrecision(0);
		
		scrollbar->setValueHigh(0);
		scrollbar->setValueLow(24);
		
		refreshView();
		
		ofAddListener(newGUIEvent, this, &ofxUISchedule::guiEvent);
		ofAddListener(listUI->newGUIEvent, this, &ofxUISchedule::guiEvent);
		
		setDrawOutline(true);
		setDrawFill(true);
		setColorOutlineHighlight(ofColor::red);
    }
	
	void update(){

		//scrollbar->setValueHigh(ofMap(listUI->getScrollPosMin().y, 0, 1, 0, 24,true));
		//scrollbar->setValueLow(ofMap(listUI->getScrollPosMax().y, 0, 1, 0, 24,true));
		
		ofxUICanvas::update();
	}
	
	void guiEvent(ofxUIEventArgs &e)
	{
		if(e.widget->getName() == "SCROLLBAR")
		{
			refreshView();
		}
		else if(e.widget->getKind() == OFX_UI_WIDGET_MEDIAASSET)
		{
			ofxUIMediaAsset *asset = (ofxUIMediaAsset*) e.widget;
			vector <ofxUIWidget *> listedWidgets = listUI->getWidgetsOfType(OFX_UI_WIDGET_MEDIAASSET);
			for(int i=0; i<listedWidgets.size(); i++){
				ofxUIMediaAsset * widget = (ofxUIMediaAsset*)listedWidgets[i];
				if(widget != e.widget)
					widget->setValue(false);
			}
			if(asset->getValue()) selectedAsset = asset;
			else selectedAsset = NULL;
			
			refreshView();
			triggerEvent(this);
		}else{
			
		}
	}
	
	void refreshView(){		
		float diff = (scrollbar->getPercentValueHigh()-scrollbar->getPercentValueLow());
		float listHeight = listUI->getRect()->getHeight();
		float diffValue = (diff*24);
		secHeight = listHeight/(diffValue*60*60);
		//float h = round(listHeight/max(diffValue, 1.0f));
		
		
		float yy =0;
		
		vector <ofxUIWidget *> listedWidgets = listUI->getWidgetsOfType(OFX_UI_WIDGET_MEDIAASSET);
		for(int i=0; i<listedWidgets.size(); i++){
			ofxScheduleEvent * event = schedule->getEvent(i);
			float h = event->getDuration().getUnixTime()*secHeight;
			ofxUIMediaAsset * widget = (ofxUIMediaAsset*)listedWidgets[i];
			widget->getRect()->height = h;
			widget->getRect()->y = yy + event->getBeginTime().getUnixTime()*secHeight; //(h*i);
			widget->setParent(listUI);
			yy=widget->getRect()->y+widget->getRect()->height;
		}
		listUI->getSRect()->setHeight(secHeight*SEC_ONE_DAY);
		//listUI->autoSizeToFitWidgets();
		listUI->setScrollPosY(1-scrollbar->getPercentValueHigh());
	}
	
	void drawOutline(){
		if(draw_outline)
        {
            ofNoFill();
            ofSetColor(color_outline);
			float x = rect->getX();
			float width = rect->getWidth()-SCROLL_WIDTH-10;
			float sy = listUI->getSRect()->y - rect->getY();
			for(int i=0; i<24; i++){
				float y = i*60*60*secHeight - sy;
				ofLine(x+TIME_LABEL_WIDTH,y,x+width,y);
				ofDrawBitmapString(ofToString(i,2,'0')+":00", x+5, y+5);
			}
            //rect->draw();
        }
	}
	
	void drawOutlineHighlight()
    {
        //if(draw_outline_highlight)
        {
            ofNoFill();
            ofSetColor(color_outline_highlight);
			time_t now = ofxScheduleTime::normalizeTime(ofGetUnixTime());
			float tWidth = 5;
			float sy = listUI->getSRect()->y - rect->getY();
			float x = rect->getX();
			float y = now*secHeight - sy;
			float width = rect->getWidth()-SCROLL_WIDTH-tWidth;
			ofTriangle(x, y-tWidth, x+tWidth, y, x, y+tWidth);
			ofLine(x+tWidth,y,x+width,y);
			
			//scrollbar Current Time Indicator
			sy = scrollbar->getRect()->getY();
			x = scrollbar->getRect()->getX();
			y = now/(float)SEC_ONE_DAY*scrollbar->getRect()->getHeight()+sy;
			width = SCROLL_WIDTH;
			ofLine(x,y,x+width,y);
            //rect->draw();
        }
    }
	
	void drawFill()
    {
        if(draw_fill)
        {
            ofFill();
			float yy=scrollbar->getRect()->getY();
			float x = scrollbar->getRect()->getX();
			
			vector <ofxUIWidget *> listedWidgets = listUI->getWidgetsOfType(OFX_UI_WIDGET_MEDIAASSET);
			for(int i=0; i<listedWidgets.size(); i++){
				ofxUIMediaAsset * widget = (ofxUIMediaAsset*)listedWidgets[i];
				ofxScheduleEvent * event = schedule->getEvent(i);
				float h = event->getDuration().getUnixTime()/(float)SEC_ONE_DAY*scrollbar->getRect()->getHeight();
				float y = yy + event->getBeginTime().getUnixTime()/(float)SEC_ONE_DAY*scrollbar->getRect()->getHeight();
				yy=y+h;
				ofSetColor(widget->getColorBack());
				ofRect(x, y, SCROLL_WIDTH, h);
			}			
            //ofSetColor(color_fill);
            //rect->draw();
        }
    }
	
	ofxUIMediaAsset * getSelected(){
		return selectedAsset;
	}
	
	bool isDraggable()
    {
        return true;
    }
	
protected:
	ofxUIScrollableCanvas *listUI;
	ofxUIRangeSlider* scrollbar;
	
    ofxSchedule *schedule;
    bool useReference;
    bool drawLabel;
	
	float secHeight;
	
	ofxUIMediaAsset * selectedAsset;
	
	ofxUILabel *label;
};


#endif
