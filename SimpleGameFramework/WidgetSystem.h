#pragma once
#ifndef __SIMPLE_WIDGET_SYSTEM__
#define __SIMPLE_WIDGET_SYSTEM__

#include "Common.h"
#include "Graphics.h"
#include "TickCounter.h"
#include <vector>
#include <unordered_map>

extern bool gShowWidgetHitBoxAllowed;

namespace sgf {
	class Graphics;
	class WidgetListener;
	class Widget;
	class WidgetManager;

	class WidgetListener {
	public:
		virtual void OnClick(int widgetId) {};
		virtual void MouseHover(int widgetId) {};
		virtual void OnChange(int widgetId) {};//仅对Slider和TextEdit有效
	};

	class WidgetManager {
	public:
		std::vector<Widget*> mWidgets;
		std::unordered_map<int , Widget*> mWidgetMap;

	public:
		void AddWidget(Widget* widget);
		void RemoveWidget(Widget* widget);
		
		Widget* GetWidgetByID(int srcId);
		void ClearWidget();

		void Update(GameAppBase* app);
		void Draw(Graphics* graphics);
	};

	class Widget {
	public:
		FloatRect mRect = {0};
		WidgetListener* mListener = nullptr;
		WidgetManager* mWidgetManager = nullptr;
		Widget* mParent = nullptr;
		std::vector<Widget*> mChilds;
		bool mVisible = true;
		TickCounter mTick;

		bool mIsMouseHover = false;
		bool mIsPlayHoverSound = false;

		//是否被鼠标摁住
		bool mIsMouseCaught = false;

		//是否为模态窗口
		bool mEnabledModel = false;
		int mId;

	public:
		Widget(int id);
		virtual ~Widget();

		void AttachToListener(WidgetListener* target) { mListener = target; };
		void Resize(float x, float y, float width, float height);
		void Resize(FloatRect& rect);

		void MoveToCenter(int screenWidth, int screenHeight);
		void MoveTo(int x, int y) { mRect.mX = x; mRect.mY = y; };
		void Translate(int x, int y);
		bool GetVisible();

		void AppendChild(Widget* child);
		void RemoveChild(Widget* child);
		void RemoveAllChild();

		void UnbindFromParentDirectly();

		void DumpWidgetImage(Graphics* g, const char* outPath);

		std::pair<int, int> GetExactPosition();
		FloatRect GetExactRect();

	public://可以被继承的区域
		virtual void Update() {};
		virtual void Draw(Graphics* graphics) {};

	};

	class TemplateButton: public Widget {
	public:
		sgf::String mLabel;

	public:
		TemplateButton(int buttonId);
		~TemplateButton();
	public:
		virtual void Draw(Graphics* graphics) override;
	};


}


#endif // !__SIMPLE_WIDGET_SYSTEM__
