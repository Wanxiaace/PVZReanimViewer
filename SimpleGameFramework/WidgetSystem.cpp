#include "WidgetSystem.h"

bool gShowWidgetHitBoxAllowed = false;

void sgf::WidgetManager::AddWidget(Widget* widget)
{
	if (widget->mWidgetManager)
		return;
	mWidgets.push_back(widget);
	mWidgetMap[widget->mId] = widget;
	widget->mWidgetManager = this;
	
	for (auto& x:widget->mChilds)
	{
		AddWidget(x);
	}
}

void sgf::WidgetManager::RemoveWidget(Widget* widget)
{
	auto pos = std::find(mWidgets.begin(), mWidgets.end(), widget);
	if (pos == mWidgets.end()) {
		SGF_ASSERT(false);
	}
	else {
		mWidgetMap[widget->mId] = nullptr;
		mWidgets.erase(pos);
	}
}

sgf::Widget* sgf::WidgetManager::GetWidgetByID(int srcId)
{
	return mWidgetMap[srcId];
}

void sgf::WidgetManager::ClearWidget()
{
	mWidgets.clear();
	for (auto& x:mWidgetMap)
	{
		x.second = nullptr;
	}
}



void sgf::WidgetManager::Update(GameAppBase* app)
{
	size_t length = mWidgets.size();
	bool isMouseUsed = false;
	bool isModelUsed = false;
	for (auto x = mWidgets.rbegin();x != mWidgets.rend();x++)
	{
		if ((*x)->GetVisible()) {
			(*x)->mTick.Update();
			
			if(!isModelUsed)
				(*x)->Update();

			if (!isMouseUsed && (*x)->GetExactRect().IsPointIn(app->mMouseX, app->mMouseY)) {
				(*x)->mIsMouseHover = true;
				if (app->mIsMouseLeftDown) {
					if (!(*x)->mIsMouseCaught) {
						(*x)->mIsMouseCaught = true;
						
					}
				}
				else {
					if ((*x)->mIsMouseCaught) {
						if ((*x)->mListener)
							(*x)->mListener->OnClick((*x)->mId);
					}
					(*x)->mIsMouseCaught = false;
				}
				if ((*x)->mListener)
					(*x)->mListener->MouseHover((*x)->mId);
				isMouseUsed = true;
			}
			else {
				(*x)->mIsMouseHover = false;
				(*x)->mIsMouseCaught = false;
			}

			if ((*x)->mEnabledModel) {
				isMouseUsed = true;
				isModelUsed = true;
			}
		}
	}
}

void sgf::WidgetManager::Draw(Graphics* graphics)
{
	graphics->ActiveTextureShader();
	for (auto& x: mWidgets)
	{
		if (!x->GetVisible())continue;
		auto pos = x->GetExactPosition();
		graphics->ModelMoveTo(pos.first, pos.second);
		graphics->MoveTo(0,0);
		if (gShowWidgetHitBoxAllowed) {
			if (x->mIsMouseHover)
				graphics->SetCubeColor({ 1,0,0,0.5f });
			else
				graphics->SetCubeColor({ 0,0,1,0.5f });
			graphics->FillRect(x->mRect.mWidth, x->mRect.mHeight);
		}
		x->Draw(graphics);
	}
	graphics->Present();
}

sgf::Widget::Widget(int id)
{
	mId = id; mTick.BindToCounter(&gGameApp->mTick);
}

sgf::Widget::~Widget()
{
	if (mParent)
	{
		SHOW_ERROR_ABORT_EXIT("Remove Widget from its Parent before deleting");
	}
	RemoveAllChild();
}

void sgf::Widget::Resize(float x, float y, float width, float height)
{
	mRect = { x,y,width,height };
}

void sgf::Widget::Resize(FloatRect& rect)
{
	mRect = rect;
}

void sgf::Widget::MoveToCenter(int screenWidth, int screenHeight)
{
	mRect.mX = (screenWidth - mRect.mWidth) / 2;
	mRect.mY = (screenHeight - mRect.mHeight) / 2;
}

void sgf::Widget::Translate(int x, int y)
{
	mRect.mX += x;
	mRect.mY += y;
}

bool sgf::Widget::GetVisible()
{
	if (!mVisible)return false;

	if (!mParent)
		return mVisible;
	else
		return mParent->GetVisible();
}

void sgf::Widget::AppendChild(Widget* child)
{
	mChilds.push_back(child);
	child->mParent = this;
	if (!child->mListener)
		child->mListener = mListener;
	if (!child->mWidgetManager && mWidgetManager)
		mWidgetManager->AddWidget(child);
}

void sgf::Widget::RemoveChild(Widget* child)
{
	auto pos = std::find(mChilds.begin(), mChilds.end(), child);
	if (pos == mChilds.end()) {
		SGF_ASSERT(false);
	}
	else {
		mChilds.erase(pos);
	}
}

void sgf::Widget::RemoveAllChild()
{
	for (auto& x : mChilds)
	{
		x->UnbindFromParentDirectly();
		mWidgetManager->RemoveWidget(x);
		delete x;
	}

	mChilds.clear();
}

void sgf::Widget::UnbindFromParentDirectly()
{
	mParent = nullptr;
}

void sgf::Widget::DumpWidgetImage(Graphics* g,const char* outPath)
{
	g->Present();

	unsigned int fbo, tex;
	g->GenFrameBuffer(&fbo, &tex, mRect.mWidth + GetExactPosition().first, mRect.mHeight + GetExactPosition().second);

	Draw(g);

	for (auto& x : mChilds)
	{
		g->ModelMoveTo(x->GetExactPosition().first, x->GetExactPosition().second);
		g->MoveTo(0, 0);
		x->Draw(g);
	}

	g->Present();
	g->CaptureScreen(GetExactPosition().first, GetExactPosition().second, mRect.mWidth, mRect.mHeight)->DumpToFiles(outPath);

	g->ResetFrameBuffer();

}

//ÀúÊ·ÒÅÁô
std::pair<int, int> sgf::Widget::GetExactPosition()
{
	if (mParent) {
		auto resultPosition = mParent->GetExactPosition();
		resultPosition.first += mRect.mX;//mParent->mRect.mX + 
		resultPosition.second += mRect.mY;//mParent->mRect.mY + 
		return resultPosition;
	}
	else {
		return{ mRect.mX,mRect.mY };
	}
}

sgf::FloatRect sgf::Widget::GetExactRect()
{
	FloatRect resultRect = mRect;
	auto exPos = GetExactPosition();
	return { (float)exPos.first,(float)exPos.second,resultRect.mWidth,resultRect.mHeight };
}

sgf::TemplateButton::TemplateButton(int buttonId) : Widget(buttonId)
{
	
}

sgf::TemplateButton::~TemplateButton()
{

}

void sgf::TemplateButton::Draw(Graphics* graphics)
{
	if (mIsMouseHover) {
		graphics->SetCubeColor({ 255,0,0,255 });
		graphics->FillRect(mRect);
		
	}else{
		graphics->SetCubeColor({ 255,255,255,255 });
		graphics->FillRect(mRect);
	}
}
