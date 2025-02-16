#include "ReanimViewer.h"
#include "Constant.h"
#include "IconFonts.h"
#include <nfd.h>

ViewerApp::ViewerApp()
	:GameApp(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, "Client",true,true)
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.Fonts->AddFontDefault();

	float baseFontSize = 13.0f; // 13.0f is the size of the default font. Change to the font size you use.
	float iconFontSize = baseFontSize * 1.5f; // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	io.Fonts->AddFontFromFileTTF((sgf::String("fonts/") + FONT_ICON_FILE_NAME_FAS).c_str(), iconFontSize, &icons_config, icons_ranges);

}

ViewerApp::~ViewerApp()
{

}

void ViewerApp::DrawImgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoMove |       // 禁止移动
		ImGuiWindowFlags_NoResize |     // 禁止调整大小
		ImGuiWindowFlags_NoCollapse |   // 禁止折叠
		ImGuiWindowFlags_NoTitleBar |   // 移除标题栏
		ImGuiWindowFlags_MenuBar;       // 允许菜单栏

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin("Main DockSpace", nullptr, window_flags);

	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	DisplayMenuBar();
	
	ImGui::End();

	DisplayBenchLayer();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ViewerApp::Draw()
{
	mGraphics->Clear();
	DrawImgui();

	mGraphics->ActiveTextureShader();
	DrawTopLayer();
	mGraphics->Present();
	
	SDL_GL_SwapWindow(mGameWindow);
}

void ViewerApp::Update()
{
	if (mAnimator)
		mAnimator->Update();

	if (mFocused)
	{
		static ImGuiIO& io = ImGui::GetIO();

		float wheel = GetMouseWheelY();

		mScale += wheel * 0.1f;

		if (mScale < 0.5f)
			mScale = 0.5f;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			mDragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			//std::cout << "Mouse dragging: (" << mDragDelta.x << ", " << mDragDelta.y << ")" << std::endl;
		}
		else {
			mMapPos.x += mDragDelta.x;
			mMapPos.y += mDragDelta.y;
			mDragDelta = { 0,0 };
		}
	}

	GameApp::Update();
}

void ViewerApp::CopeEvent(SDL_Event& theEvent)
{
	GameApp::CopeEvent(theEvent);

	switch (theEvent.type) {
	case SDL_EventType::SDL_WINDOWEVENT:
		switch (theEvent.window.event)
		{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = theEvent.window.data1;
			mHeight = theEvent.window.data2;
		default:
			break;
		}
		//theEvent.window.event
		break;
	case SDL_EventType::SDL_MOUSEWHEEL: {
		break;
	}
	}
}

static void ImGuiDrawGrid(
	ImDrawList* draw_list, ImVec2 window_pos, ImVec2 window_size, ImVec2 pos_offset, float grid_spacing, ImU32 grid_color, ImU32 grid_color_shallow)
{
	pos_offset.x -= int(pos_offset.x) / int(grid_spacing * 10) * grid_spacing * 10;
	pos_offset.y -= int(pos_offset.y) / int(grid_spacing * 10) * grid_spacing * 10;


	draw_list->AddRectFilled(window_pos, { window_pos.x + window_size.x ,window_pos.y + window_size.y }, IM_COL32(0x69, 0x62, 0x5d, 0xff));

	int num_lines_x = (int)(window_size.x / grid_spacing) + 1;
	int num_lines_y = (int)(window_size.y / grid_spacing) + 1;

	for (int i = 0; i < num_lines_x; ++i)
	{
		float x = window_pos.x + i * grid_spacing + pos_offset.x;
		if (i % 10)
			draw_list->AddLine(ImVec2(x, window_pos.y), ImVec2(x, window_pos.y + window_size.y), grid_color_shallow);
		else
			draw_list->AddLine(ImVec2(x, window_pos.y), ImVec2(x, window_pos.y + window_size.y), grid_color);

	}


	for (int i = 0; i < num_lines_y; ++i)
	{
		float y = window_pos.y + i * grid_spacing + pos_offset.y;
		if (i % 10)
			draw_list->AddLine(ImVec2(window_pos.x, y), ImVec2(window_pos.x + window_size.x, y), grid_color_shallow);
		else
			draw_list->AddLine(ImVec2(window_pos.x, y), ImVec2(window_pos.x + window_size.x, y), grid_color);
	}
}

void ViewerApp::DisplayBenchLayer()
{
	ImGui::Begin("Layer List");

	if (mReanimPtr)
	{
		for (size_t i = 0; i < mReanimPtr->mTracks->size(); i++)
		{
			sgf::String& trackName = mReanimPtr->mTracks->at(i).mTrackName;
			if (ImGui::Checkbox((trackName + "##" + sgf::SString::StrParse(i)).c_str(), &mTrackShowList[i])) {
				UpdateAnimatorState();
			}

			if (trackName.substr(0, 5) == "anim_") {
				ImGui::SameLine();
				if (ImGui::Button((sgf::String("As Work Range##") + sgf::SString::StrParse(i)).c_str())) {
					mAnimator->SetFrameRangeByTrackName(trackName);
				};
			}
		}
	}
	

	ImGui::End();

	ImGui::Begin("Image Resource List");
	if (mReanimPtr) {
		int ctr = 0;
		for (auto& x : *mReanimPtr->mImagesSet)
		{
			ImGui::Text(x.c_str(),&mImageShowList[ctr]);
			ctr++;
		}
	}
	ImGui::End();

	ImGui::Begin("Viewer");

	mWindowSize = ImGui::GetWindowSize();
	mWindowPos = ImGui::GetWindowPos();
	mFocused = ImGui::IsWindowFocused();

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGuiDrawGrid(draw_list, mWindowPos, mWindowSize,
		{ mMapPos.x + mDragDelta.x, mMapPos.y + mDragDelta.y },
		10.0f * mScale, IM_COL32(0xff, 0xff, 0xff, 0xA0), IM_COL32(0xff, 0xff, 0xff, 0x20));


	ImGui::End();

	ImGui::Begin("Control Panel");

	if (mAnimator) {
		ImGui::PushStyleColor(ImGuiCol_Text, { 0,1,0,1 });
		if (ImGui::Button(ICON_FA_PLAY " Play", { 0,30 })) {
			mAnimator->Play();
		}
		ImGui::PopStyleColor();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Text, { 1,0,0,1 });
		if (ImGui::Button(ICON_FA_PAUSE " Pause", { 0,30 })) {
			mAnimator->Pause();
		}
		ImGui::PopStyleColor();


		if (ImGui::SliderFloat("Speed", &mReanimSpeed, 0.5f, 5.0f, "%.1f")) {
			mAnimator->mSpeed = mReanimSpeed;
		};

		ImGui::SameLine();
		if (ImGui::Button("Reset")) { mReanimSpeed = 1.0f; mAnimator->mSpeed = 1.0f; };

		ImGui::SliderFloat("Process", &mAnimator->mFrameIndexNow, mAnimator->mFrameIndexBegin, mAnimator->mFrameIndexEnd, "%.1f");
		ImGui::Text("RangeBegin: %f", mAnimator->mFrameIndexBegin);
		ImGui::SameLine();
		ImGui::Text("RangeEnd: %f", mAnimator->mFrameIndexEnd);
		ImGui::SameLine();
		if (ImGui::Button("Reset to default")) {
			mAnimator->SetFrameRangeToDefault();
		};
	}
	ImGui::End();
}

void ViewerApp::DisplayMenuBar()
{
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Files")) {
			if (ImGui::MenuItem("Open")) {
				char* targetPath = nullptr;
				char* targetDirPath = nullptr;
				nfdu8filteritem_t item = { "ReanimFile(xml)","xml,reanim" };
				NFD_OpenDialog(&targetPath, &item, 1, nullptr);
				if (!targetPath)
					std::cout << "Faild to open" << std::endl;
				else
				{
					NFD_PickFolder(&targetDirPath,"");
					if (!targetDirPath)
						std::cout << "Faild to open" << std::endl;
					else {
						LoadReanim(targetPath, targetDirPath);
					}
				}
			}
			ImGui::EndMenu();
		}

		if (mReanimPtr) {
			if (ImGui::MenuItem("Reload")) {
				LoadReanim(mTargetPath.c_str(), mImagesDir.c_str());
			}
		}

		ImGui::Text("FPS: %d", mFPS);
		ImGui::EndMenuBar();
	}
}

void ViewerApp::DrawTopLayer()
{
	if (mAnimator) {
		PresentAnimator();
	}
}

void ViewerApp::LoadReanim(const char* reanimPath, const char* resDirPath)
{
	if (mReanimPtr)
	{
		for (auto& x : *mReanimPtr->mImagesSet)
		{
			//此处由于设计缺陷需要这样实现资源释放
			auto* img = mResourceManager.GetResourceFast<sgf::SimpleImage>(x);
			delete img;
			mResourceManager.mResourcePool.clear();
		}

		delete mReanimPtr;
	}


	mTargetPath = reanimPath;
	mImagesDir = resDirPath;

	mReanimSpeed = 1.0f;
	mReanimPtr = new sgf::Reanimation();
	mReanimPtr->LoadFromFile(reanimPath);
	mReanimPtr->mResourceManager = &mResourceManager;

	int ctr = 0;
	for (auto& x : *mReanimPtr->mImagesSet)
	{
		mImageShowList[ctr] = true;
		std::cout << x.substr(13) << std::endl;

		mReanimPtr->mResourceManager->LoadImageWithID(sgf::String(resDirPath) + "\\" + x.substr(13) + ".png", x);
		ctr++;
	}

	for (size_t i = 0; i < mReanimPtr->mTracks->size(); i++)
	{
		mTrackShowList[i] = true;
	}

	if (mAnimator)
	{
		delete mAnimator;
	}

	mAnimator = new sgf::Animator(mReanimPtr);
}

void ViewerApp::UpdateAnimatorState()
{
	for (size_t i = 0; i < mAnimator->mExtraInfos.size(); i++)
	{
		mAnimator->mExtraInfos[i].mVisible = mTrackShowList[i];
	}
}

void ViewerApp::PresentAnimator()
{
	mGraphics->SetCubeColor({ 1,1,1,1 });
	mGraphics->MoveTo(0, 0);
	mGraphics->SetClipRect({ mWindowPos.x ,mWindowPos.y + 20,mWindowSize.x ,mWindowSize.y - 20 });

	mGraphics->MoveTo(mMapPos.x + mDragDelta.x, mMapPos.y + mDragDelta.y - 20);
	mAnimator->PresentMatrix(mGraphics, glm::scale(glm::mat4x4(1.0f), glm::vec3(mScale, mScale, 1.0f)));

	mGraphics->ClearClipRect();
}
