#include "App.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
//#include "Graphics.h"

App::App( const std::string& commmandline)
	:
	commandline(commmandline),
	wnd(1080,720)
{

	 TerrainList.push_back(std::make_unique<Terrain>(128, 128, wnd.Gfx()));
	 //Camera Projection
	 //wnd.Gfx().SetCamera();
	 wnd.Gfx().SetProjection(XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 200.0f));
	 //a small quard
	 //wnd.Gfx().SetCullMode();
	 //sWINDOW = std::move(std::make_unique<subWin>(wnd.Gfx(), 1080*0.3, 720*0.3));
	 
}

int App::Go()
{
	MSG msg = { 0 };
	
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DoFrame();
		}
	}

	return (int)msg.wParam;
}

App::~App()
{}

void App::DoFrame()
{
	// imgui stuff
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	CameraControl();
	const auto dt = timer.Mark();
	
	for (auto& b : TerrainList)
	{
		b->Update(dt, wnd.Gfx());
		b->Submit(wnd.Gfx());
	}
	if (ImGui::Begin("info"))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();

	// imgui stuff
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	wnd.Gfx().EndFrame();
}

void App::CameraControl(){

	if (wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		wnd.Gfx().GetCamera()->Reset();
	}
	auto _char = wnd.kbd.ReadChar();
	if (_char.has_value()) {
		wnd.Gfx().GetCamera()->CameraUpdate(_char.value());
	}
	

}