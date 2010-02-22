#ifndef ZGUILAPCHOSER_H__
#define ZGUILAPCHOSER_H__

class ZGuiLapChoser 
{
public:
	ZGuiLapChoser()
	{
		
	}
	virtual ~ZGuiLapChoser()
	{
	}

	void Init(CEGUI::Window *parent, const char *szPrefix, const char *szText, int aMin, int aMax, int value)
	{
		WindowManager &winMgr = WindowManager::getSingleton();

		mSB = (Scrollbar*)(winMgr.createWindow("TaharezLook/HorizontalScrollbar", String(szPrefix)+szText));
		parent->addChildWindow(mSB);
		
		mText = szText;

		txtlib = winMgr.createWindow("TaharezLook/StaticText", String(szPrefix)+String("SoloStatString")+szText);

		txtlib->setProperty("FrameEnabled", "false");
		txtlib->setProperty("BackgroundEnabled", "true");
		txtlib->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
		txtlib->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 1.f )));
		txtlib->setVerticalAlignment(VA_CENTRE);
		txtlib->setHorizontalAlignment(HA_CENTRE);

		mSB->addChildWindow(txtlib);
		mSB->setDocumentSize(float(aMax-aMin));
		mSB->setScrollPosition(float(value-aMin));

		mSB->getThumb()->hide();

		mSB->subscribeEvent ( Scrollbar::EventScrollPositionChanged, Event::Subscriber (&ZGuiLapChoser::valueChanged, this));

		mMin = aMin;

		UpdateText();
	}

    void setPosition(const UVector2& uv2)
	{
		mSB->setPosition(uv2);
		//(cegui_reldim(0.45f), cegui_reldim( 0.30f)));
	}
    void setSize(const UVector2& uv2)
	{
		mSB->setSize(uv2);
	}//UVector2(cegui_reldim(0.25f), cegui_reldim( 0.20f)));
	int GetValue()
	{
		return int(mSB->getScrollPosition() + mMin);
	}

protected:
/*
	bool handleIncreaseClicked (const EventArgs &e)
	{
		Scrollbar::handleIncreaseClicked(e);
		UpdateText();
		return true;
	}

	bool handleDecreaseClicked (const EventArgs &e)
	{
		Scrollbar::handleDecreaseClicked(e);
		UpdateText();
		return true;
	}
*/
	void UpdateText()
	{
		tstring vTxt;
		vTxt.Printf(mText.c_str(), int(mSB->getScrollPosition())+mMin);
		txtlib->setText(vTxt.c_str());
	}

	bool valueChanged(const CEGUI::EventArgs& e)
	{
		UpdateText();
		return true;
	}


	tstring mText;
	Window *txtlib;
	CEGUI::Scrollbar *mSB;
	int mMin;
};

#endif