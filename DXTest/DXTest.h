// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _USE_MATH_DEFINES

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 7 or later.
#define WINVER 0x0700       // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows 7 or later.
#define _WIN32_WINNT 0x0700 // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

// C RunTime Header Files
#include <stdlib.h>
#include <time.h>
#include <list>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>
#include <math.h>
#include <strsafe.h>
#ifndef _USE_OBSTACLES
	#include <obstacles.h>
#endif
#ifndef _USE_PARTICLES
	#include <particles.h>
#endif



// TODO: reference additional headers your program requires here
#include "resource.h"

#define GAME_STATE_ONMENU 0
#define GAME_STATE_RUNNING 1
#define GAME_STATE_GAMEOVER 2

/******************************************************************
*                                                                 *
*  Macros                                                         *
*                                                                 *
******************************************************************/

template<class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#ifndef _STRUCT_NAVE_CREATED
typedef struct nave{
	D2D1_POINT_2F* dots;
	D2D1_POINT_2F center;
	float orientation;
	float speed;
	D2D1_POINT_2F torque;
	D2D1_POINT_2F position;
	int keyup;
	int keydown;
	int keyleft;
	int keyright;
	int iPontuation;
	int iHealth;
} NAVE;
#endif

class DxComponent{

public:
    DxComponent();
    ~DxComponent();

	HRESULT Initialize();
	void DxComponent::RunMessageLoop();
	Smoke *smoke;
	Smoke *destruction;
	Obstacle *obstacle;
	Shot *shot;
	int keyF1;
	int keyShot;
	int keyShotPrevState;
	int iShotFrameCount;
	int iFlagDestructionAnimation;
	int iGameOver;
	int iGameState;

private:
	HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    void DiscardDeviceResources();

    HRESULT OnRender();
	HRESULT DisplayMenuScreen();
	HRESULT DisplayGameScreen();
	void OnResize(
        UINT width,
        UINT height
        );
	void Restart();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	ID2D1Factory *pD2DFactory;
	ID2D1HwndRenderTarget *pD2DRenderTarget;
	ID2D1SolidColorBrush *pD2DColorBlackBrush;
	ID2D1SolidColorBrush *pD2DColorYellowBrush;
	ID2D1SolidColorBrush *pD2DColorRedBrush;
	ID2D1PathGeometry *pD2DDynamicGeometry;
	IDWriteFactory *pDWriteFactory;
	IDWriteTextFormat *pDWriteTextFormat;
	HWND hWnd;
	NAVE nave;
};
