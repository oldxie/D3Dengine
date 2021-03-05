#include "DXAPI.h"

SimpleVertex* CreateMesh()
{
	//fftSize = (int)Mathf.Pow(2, FFTPow);
	int MeshSize = 60;
	int MeshLength = 128;
	int index_size = (MeshSize - 1) * (MeshSize - 1) * 6;
	WORD *vertIndexs = new WORD[index_size];
	SimpleVertex *vertex= new SimpleVertex[MeshSize * MeshSize];
	//XMFLOAT3 *positions = new XMFLOAT3[MeshSize * MeshSize];
	XMFLOAT2 *uvs = new XMFLOAT2[MeshSize * MeshSize];
	
	int inx = 0;
	
	for (int i = 0; i < MeshSize; i++)
	{
		for (int j = 0; j < MeshSize; j++)
		{
			int index = i * MeshSize + j;
			vertex[index].Pos = XMFLOAT3((j - MeshSize / 2.0f) * MeshLength / MeshSize, 0, (i - MeshSize / 2.0f) * MeshLength / MeshSize);
			vertex[index].uv = XMFLOAT2(j / (MeshSize - 1.0f), i / (MeshSize - 1.0f));
			vertex[index].Normal = XMFLOAT3(0, 1, 0);
			uvs[index] = XMFLOAT2(j / (MeshSize - 1.0f), i / (MeshSize - 1.0f));

			if (i != MeshSize - 1 && j != MeshSize - 1)
			{
				vertIndexs[inx++] = index;
				vertIndexs[inx++] = index + MeshSize;
				vertIndexs[inx++] = index + MeshSize + 1;

				vertIndexs[inx++] = index;
				vertIndexs[inx++] = index + MeshSize + 1;
				vertIndexs[inx++] = index + 1;
			}
		}
	}

	CreateResource();
	CreateVB_IB(vertex,vertIndexs, MeshSize, index_size);
	//mesh.vertices = positions;
	//mesh.SetIndices(vertIndexs, MeshTopology.Triangles, 0);
	//mesh.uv = uvs;
	return vertex;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
/*
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}
	SimpleVertex* vertex=CreateMesh();
	// Main message loop
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
			Render(vertex);
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}*/