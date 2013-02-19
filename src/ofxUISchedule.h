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
#include "ofxUINumberDialer.h"
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
		listUI->setDrawBack(false);
		
		secHeight = listUI->getRect()->getHeight()/SEC_ONE_DAY/2;
		listUI->getSRect()->setHeight(secHeight*SEC_ONE_DAY);
		
		scrollbar = (ofxUIRangeSlider *) addWidgetRight(new ofxUIRangeSlider("SCROLLBAR", 24, 0, 24, 12, SCROLL_WIDTH, rect->getHeight()));
		scrollbar->setLabelVisible(false);
		scrollbar->setLabelPrecision(0);
		
		scrollbar->setValueHigh(0);
		scrollbar->setValueLow(24);
		
		updateSchedule();
		
		panel = new ofxUICanvas();
		panel ->addLabel("LABEL BEGIN", "BEGIN");
		ofxUINumberDialer *d;
		d = (ofxUINumberDialer *) panel ->addWidgetDown(new ofxUINumberDialer(0.f, 23.f, 0.f, 0, "BEGIN HOUR", OFX_UI_FONT_SMALL));
		d->setSign(' ');
		d = (ofxUINumberDialer *) panel ->addWidgetRight(new ofxUINumberDialer(-1.f, 60.f, 0.f, 0, "BEGIN MINUTE", OFX_UI_FONT_SMALL));
		d->setSign(':');
		d = (ofxUINumberDialer *) panel ->addWidgetRight(new ofxUINumberDialer(-1.f, 60.f, 0.f, 0, "BEGIN SECOND", OFX_UI_FONT_SMALL));
		d->setSign(':');
		panel ->addLabel("LABEL DURATION", "DURATION");
		d = (ofxUINumberDialer *) panel ->addWidgetDown(new ofxUINumberDialer(0.f, 23.f, 0.f, 0, "DURATION HOUR", OFX_UI_FONT_SMALL));
		d->setSign(' ');
		d = (ofxUINumberDialer *) panel ->addWidgetRight(new ofxUINumberDialer(-1.f, 60.f, 0.f, 0, "DURATION MINUTE", OFX_UI_FONT_SMALL));
		d->setSign(':');
		d = (ofxUINumberDialer *) panel ->addWidgetRight(new ofxUINumberDialer(-1.f, 60.f, 0.f, 0, "DURATION SECOND", OFX_UI_FONT_SMALL));
		d->setSign(':');
		panel ->addLabel("LABEL END", "END");
		d = (ofxUINumberDialer *) panel ->addWidgetDown(new ofxUINumberDialer(0.f, 23.f, 0.f, 0, "END HOUR", OFX_UI_FONT_SMALL));
		d->setSign(' ');
		d = (ofxUINumberDialer *) panel ->addWidgetRight(new ofxUINumberDialer(-1.f, 60.f, 0.f, 0, "END MINUTE", OFX_UI_FONT_SMALL));
		d->setSign(':');
		d = (ofxUINumberDialer *) panel ->addWidgetRight(new ofxUINumberDialer(-1.f, 60.f, 0.f, 0, "END SECOND", OFX_UI_FONT_SMALL));
		d->setSign(':');
		panel ->addLabel("LABEL LOOP", "LOOP");
		d = (ofxUINumberDialer *) panel ->addWidgetDown(new ofxUINumberDialer(0.f, 100, 0.f, 2, "LOOP", OFX_UI_FONT_SMALL));
		d->setSign(' ');
		panel->addButton("DEL", false);
		panel->setDrawBack(true);
		panel->setColorBack(ofColor(32,32,32,192));
		panel->disableAppDrawCallback();
		panel->disable();
		panel->autoSizeToFitWidgets();
		
				
		ofAddListener(newGUIEvent, this, &ofxUISchedule::guiEvent);
		ofAddListener(listUI->newGUIEvent, this, &ofxUISchedule::guiEvent);
		ofAddListener(panel->newGUIEvent, this, &ofxUISchedule::panelEvent);
		
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
				if(widget != asset)
					widget->setValue(false);
				else
					selectedEvent = schedule->getEvent(i);
			}
			if(asset->getValue()) {
				selectedAsset = asset;
				showPanelAt(asset);
			} else {
				selectedAsset = NULL;
				selectedEvent = NULL;
				hidePanel();
			}
			
			refreshView();
			triggerEvent(this);
		}else{
			
		}
	}
	
	void panelEvent(ofxUIEventArgs &e)
	{
		if(e.widget->getName() == "BEGIN HOUR"){
			ofxUINumberDialer * d = (ofxUINumberDialer*) e.widget;
			selectedEvent->getBeginTime()->setHour((int)d->getValue());
			refreshView();
		}else if(e.widget->getName() == "BEGIN MINUTE"){
			ofxUINumberDialer * d = (ofxUINumberDialer*) e.widget;
			int i = d->getValue();
			if(i==60){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("BEGIN HOUR");
				p->setValue(p->getValue()+1);
				d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}else if(i==-1){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("BEGIN HOUR");
				p->setValue(p->getValue()-1);
				if(p->getValue() != 0)d->setValue(59);
				else d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}
			selectedEvent->getBeginTime()->setMinute((int)d->getValue());
			refreshView();
		}else if(e.widget->getName() == "BEGIN SECOND"){
			ofxUINumberDialer * d = (ofxUINumberDialer*) e.widget;
			int i = d->getValue();
			if(i==60){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("BEGIN MINUTE");
				p->setValue(p->getValue()+1);
				d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}else if(i==-1){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("BEGIN MINUTE");
				p->setValue(p->getValue()-1);
				if(p->getValue() != 0)d->setValue(59);
				else d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}
			selectedEvent->getBeginTime()->setSec((int)d->getValue());
			refreshView();
		}else if(e.widget->getName() == "DURATION HOUR"){///////////////////////////////////////////////////
			ofxUINumberDialer * d = (ofxUINumberDialer*) e.widget;
			selectedEvent->getDuration()->setHour((int)d->getValue());
			refreshView();
		}else if(e.widget->getName() == "DURATION MINUTE"){
			ofxUINumberDialer * d = (ofxUINumberDialer*) e.widget;
			int i = d->getValue();
			if(i==60){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("DURATION HOUR");
				p->setValue(p->getValue()+1);
				d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}else if(i==-1){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("DURATION HOUR");
				p->setValue(p->getValue()-1);
				if(p->getValue() != 0)d->setValue(59);
				else d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}
			selectedEvent->getDuration()->setMinute((int)d->getValue());
			refreshView();
		}else if(e.widget->getName() == "DURATION SECOND"){
			ofxUINumberDialer * d = (ofxUINumberDialer*) e.widget;
			int i = d->getValue();
			if(i==60){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("DURATION MINUTE");
				p->setValue(p->getValue()+1);
				d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}else if(i==-1){
				ofxUINumberDialer * p = (ofxUINumberDialer*) panel->getWidget("DURATION MINUTE");
				p->setValue(p->getValue()-1);
				if(p->getValue() != 0)d->setValue(59);
				else d->setValue(0);
				ofxUIEventArgs e = ofxUIEventArgs(p);
				panelEvent(e);
			}
			selectedEvent->getDuration()->setSec((int)d->getValue());
			refreshView();
		}
		
		
		
		else if(e.widget->getName() == "DEL"){
			ofxUIButton *b = (ofxUIButton*)e.widget;
			if(b->getValue()){
				schedule->removeSchedule(selectedEvent);
				updateSchedule();
				selectedEvent = NULL;
				hidePanel();
			}
		}

			
	}
	
	void showPanelAt(ofxUIWidget * widget){
		ofxUIRectangle * wrect = widget->getRect();
		ofxUIRectangle * prect = panel->getRect();
		//prect->setX(-prect->getWidth());
		prect->setY(wrect->getHeight());
		prect->setWidth(wrect->getWidth());
		prect->setParent(wrect);
		showPanel();
	}
	
	void showPanel(){
		panel->enable();
		ofxUINumberDialer *d;
		d = (ofxUINumberDialer *) panel->getWidget("BEGIN HOUR");
		d->setValue(selectedEvent->getBeginTime()->getHour());
		d = (ofxUINumberDialer *) panel->getWidget("BEGIN MINUTE");
		d->setValue(selectedEvent->getBeginTime()->getMinute());
		d = (ofxUINumberDialer *) panel->getWidget("BEGIN SECOND");
		d->setValue(selectedEvent->getBeginTime()->getSec());
		
		d = (ofxUINumberDialer *) panel->getWidget("DURATION HOUR");
		d->setValue(selectedEvent->getDuration()->getHour());
		d = (ofxUINumberDialer *) panel->getWidget("DURATION MINUTE");
		d->setValue(selectedEvent->getDuration()->getMinute());
		d = (ofxUINumberDialer *) panel->getWidget("DURATION SECOND");
		d->setValue(selectedEvent->getDuration()->getSec());
		
		d = (ofxUINumberDialer *) panel->getWidget("END HOUR");
		d->setValue(selectedEvent->getEndTime()->getHour());
		d = (ofxUINumberDialer *) panel->getWidget("END MINUTE");
		d->setValue(selectedEvent->getEndTime()->getMinute());
		d = (ofxUINumberDialer *) panel->getWidget("END SECOND");
		d->setValue(selectedEvent->getEndTime()->getSec());
		
		d = (ofxUINumberDialer *) panel->getWidget("LOOP");
		d->setValue(selectedEvent->getLoop());
	}
	
	void hidePanel(){
		panel->disable();
	}
	
	void updateSchedule(){
		listUI->removeWidgets();
		float yy =0;
		ofxUIMediaAsset * widget = NULL;
        for(int i=0; i<schedule->numEvents(); i++){
			ofxScheduleEvent * event = schedule->getEvent(i);
			float h = event->getDuration()->getUnixTime()*secHeight;
			float y = yy + event->getBeginTime()->getUnixTime()*secHeight;
			widget = new ofxUIMediaAsset(event->getMessage(),listUI->getRect()->width-TIME_LABEL_WIDTH, h, TIME_LABEL_WIDTH, y);
            listUI->addWidget(widget);
			widget->setColorBack(ofColor::fromHsb((y/listUI->getSRect()->height)*255, 255, 255, 128));
			
			yy+=widget->getRect()->height;
			
        }
		if(widget != NULL) {
			widget->setValue(true);
			ofxUIEventArgs e = ofxUIEventArgs(widget);
			guiEvent(e);
		}
		refreshView();
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
			float h = event->getDuration()->getUnixTime()*secHeight;
			ofxUIMediaAsset * widget = (ofxUIMediaAsset*)listedWidgets[i];
			widget->getRect()->height = h;
			widget->getRect()->y = yy + event->getBeginTime()->getUnixTime()*secHeight; //(h*i);
			widget->setParent(listUI);
			yy=widget->getRect()->y+widget->getRect()->height;
		}
		listUI->getSRect()->setHeight(secHeight*SEC_ONE_DAY);
		listUI->setScrollPosY(1-scrollbar->getPercentValueHigh());
		
		//
		if(selectedAsset){
			ofxUIRectangle *rect = panel->getRect();
			rect->setY(selectedAsset->getRect()->getHeight());
			if(rect->getRelativeMaxY()>ofGetHeight())
				rect->setY(-rect->getHeight());
		}
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
	
	void draw(){
		ofxUICanvas::draw();
		if(panel->isEnabled()) panel->draw();
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
				float h = event->getDuration()->getUnixTime()/(float)SEC_ONE_DAY*scrollbar->getRect()->getHeight();
				float y = yy + event->getBeginTime()->getUnixTime()/(float)SEC_ONE_DAY*scrollbar->getRect()->getHeight();
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
	ofxScheduleEvent * selectedEvent;
	
	ofxUILabel *label;
	
	ofxUICanvas *panel;
};


#endif
