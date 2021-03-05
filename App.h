#pragma once
#include <Windows.h>
#include <xstring>
#include <vector>
#include "Window.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "IndexBuffer.h"
#include "FreeObject.h"
#include "Topology.h"
#include "TransformCbuf.h"
#include "Terrain.h"
#include "subWin.h"
#include "Timer.h"
#include "ImguiManager.h"
class App
{
public:
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
	~App();
	
private:
	void DoFrame();
private:
	ImguiManager imgui;
	std::string commandline;
	Window wnd;
	std::vector<std::unique_ptr<FreeObject>> FreeObjectList;
	std::vector<std::unique_ptr<Terrain>> TerrainList;
	ChiliTimer timer;
	void CameraControl();
};
