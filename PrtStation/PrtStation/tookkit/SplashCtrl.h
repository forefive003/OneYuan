#pragma once
class CSplashCtrl : public CWnd
{
public:
	CSplashCtrl();
	~CSplashCtrl();
	void ShowSplashWindow();
	void HideSplashWindow();

protected:
	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_bitmap;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};

