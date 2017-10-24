#include "DXTest.h"

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    ){
	// Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL))){
			DxComponent DxApp;

			if(SUCCEEDED(DxApp.Initialize())){
				DxApp.RunMessageLoop();
			}
	}
	CoUninitialize();

    // return this part of the WM_QUIT message to Windows
    return 0;

}

DxComponent::DxComponent() :
	pD2DFactory(NULL),
	pD2DRenderTarget(NULL),
	pD2DColorBlackBrush(NULL),
	pD2DColorYellowBrush(NULL),
	pD2DColorRedBrush(NULL),
	pD2DDynamicGeometry(NULL),
	pDWriteFactory(NULL),
	pDWriteTextFormat(NULL),
	hWnd(NULL),
	smoke(NULL),
	destruction(NULL)
{
	nave.torque.x = 0;
	nave.torque.y = 0;
	nave.orientation = -90.0f;
	nave.speed = 0;
	nave.keyup = 0;
	nave.keydown = 0;
	nave.keyleft = 0;
	nave.keyright = 0;
	nave.iPontuation = 0;
	nave.iHealth = 100;
	keyF1= 0;
	keyShot = 0;
	iShotFrameCount = 0;
	iFlagDestructionAnimation = 0;
	iGameOver = 0;
	iGameState = GAME_STATE_ONMENU;
}

DxComponent::~DxComponent(){
	SafeRelease(&pD2DFactory);
	SafeRelease(&pD2DRenderTarget);
	SafeRelease(&pD2DColorBlackBrush);
	SafeRelease(&pD2DColorYellowBrush);
	SafeRelease(&pD2DColorRedBrush);
	SafeRelease(&pD2DDynamicGeometry);
	SafeRelease(&pDWriteFactory);
	SafeRelease(&pDWriteTextFormat);
	delete smoke;
}

HRESULT DxComponent::Initialize(){
		HRESULT hr = S_OK;
		

		hr = CreateDeviceIndependentResources();

		if(SUCCEEDED(hr)){

			// fill in the struct with the needed information
			WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
			wcex.style         = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc   = DxComponent::WndProc;
			wcex.cbClsExtra    = 0;
			wcex.cbWndExtra    = sizeof(LONG_PTR);
			wcex.hInstance     = HINST_THISCOMPONENT;
			wcex.hbrBackground = NULL;
			wcex.lpszMenuName  = NULL;
			wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
			wcex.lpszClassName = L"D2DDemoApp";
			wcex.hIcon = LoadIcon(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDI_ICON1));
			wcex.hIconSm = LoadIcon(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDI_ICON1));

			// Registering Windows Class
			RegisterClassEx(&wcex);

			// Because the CreateWindow function takes its size in pixels,
			// obtain the system DPI and use it to scale the window size.
			FLOAT dpiX, dpiY;

			// The factory returns the current system DPI. This is also the value it will use
			// to create its own windows.
			pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

			// Creating the Window
			hWnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(800.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(600.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );

			hr = hWnd ? S_OK : E_FAIL;
			if (SUCCEEDED(hr))
			{
				ShowWindow(hWnd, SW_SHOWNORMAL);
				UpdateWindow(hWnd);
			}
		}

		return hr;
}

void DxComponent::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT DxComponent::CreateDeviceIndependentResources(){
	smoke = new Smoke();
	destruction = new Smoke();
	obstacle = new Obstacle();
	shot = new Shot();
	obstacle->getNave(&nave);
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;

	// Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	// Create a DirectWrite factory
	if(SUCCEEDED(hr)){
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
	}

	if(SUCCEEDED(hr)){
		hr = pDWriteFactory->CreateTextFormat(L"Times New Roman",// Font family name.
											NULL,// Font collection (NULL sets it to use the system font collection).
											DWRITE_FONT_WEIGHT_REGULAR,
											DWRITE_FONT_STYLE_NORMAL,
											DWRITE_FONT_STRETCH_NORMAL,
											20.0f,
											L"en-us",
											&pDWriteTextFormat
											);
	}

	if(SUCCEEDED(hr)){
		hr = pDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}

	if(SUCCEEDED(hr)){
		hr = pDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	if(SUCCEEDED(hr)){
		hr = pD2DFactory->CreatePathGeometry(&pD2DDynamicGeometry);
	}

	if(SUCCEEDED(hr)){
		hr = pD2DDynamicGeometry->Open(&pSink);
	}

	if(SUCCEEDED(hr)){
		pSink->BeginFigure(
			D2D1::Point2F(0.0f,0.0f),
			D2D1_FIGURE_BEGIN_FILLED);

		D2D1_POINT_2F triangle[] = {{-10,-10},{-10,10},{0,0}};

		nave.center.x = (triangle[0].x + triangle[1].x + triangle[2].x) /3;
		nave.center.y = (triangle[0].y + triangle[1].y + triangle[2].y) /3;
		//nave.dots = new D2D1_POINT_2F[3];
		nave.dots = triangle;
		pSink->AddLines(triangle, 3);

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

        hr = pSink->Close();
	}

	SafeRelease(&pSink);
    return hr;
}

HRESULT DxComponent::CreateDeviceResources(){
	HRESULT hr = S_OK;

	if(!pD2DRenderTarget){
		RECT rc;
		GetClientRect(hWnd,&rc);

		D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
				);
	
		// Create a D2D render target
		hr = pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
												 D2D1::HwndRenderTargetProperties(hWnd, size),
												 &pD2DRenderTarget);

		if(SUCCEEDED(hr)){
			hr = pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
														&pD2DColorBlackBrush);
		}
		if(SUCCEEDED(hr)){
			hr = pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow),
														&pD2DColorYellowBrush);
		}
		if(SUCCEEDED(hr)){
			hr = pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red),
														&pD2DColorRedBrush);
		}

		nave.position.x = (rc.right - rc.left)/2;
		nave.position.y = (rc.bottom - rc.top)/2;
	}
	return hr;
}

void DxComponent::DiscardDeviceResources()
{
    SafeRelease(&pD2DRenderTarget);
    SafeRelease(&pD2DColorBlackBrush);
	SafeRelease(&pD2DColorYellowBrush);
	SafeRelease(&pD2DColorRedBrush);
	delete smoke;
}

HRESULT DxComponent::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();

	switch(iGameState)
	{
		case GAME_STATE_ONMENU:
		{
			DisplayMenuScreen();
		}
		break;
		case GAME_STATE_RUNNING:
		{
			if(SUCCEEDED(hr)){
				DisplayGameScreen();

			}
		}
		break;
		case GAME_STATE_GAMEOVER:
		{

		}
	}
	return hr;
}

HRESULT DxComponent::DisplayMenuScreen(){
	HRESULT hr = S_OK;
	D2D1_SIZE_F rtSize = pD2DRenderTarget->GetSize();

	wchar_t sGameOver[] = L"Asteroids Demo.\nPress Space to Start\n\n\n\n\n\nCommands: \nArrows: Ship Movement\nA: Shot\0";
	int iLen = lstrlen(sGameOver);
	D2D1_RECT_F rectGameOver = D2D1::RectF(
    rtSize.width/2 - 100.0f,
    rtSize.height/2 - 50.0f,
    rtSize.width/2 + 200.0f,
    rtSize.height/2 + 100.0f
    );
	pD2DRenderTarget->BeginDraw();
	pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Gray));
	pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	pD2DColorBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	pD2DRenderTarget->DrawTextW(sGameOver,iLen,pDWriteTextFormat,rectGameOver,pD2DColorBlackBrush);
	pD2DColorBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	pD2DRenderTarget->EndDraw();
	return hr;
}

HRESULT DxComponent::DisplayGameScreen(){
	HRESULT hr = S_OK;
	D2D1_SIZE_F rtSize = pD2DRenderTarget->GetSize();

	//Não desenha se o tamanho da janela for igual a zero
	if(rtSize.width == 0 || rtSize.height == 0) return hr;

	pD2DRenderTarget->BeginDraw();

	pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //Mesma coisa que multiplicar por 1 (Não entendi a finalidade [Já entendi a finalidade :D])
	pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Gray)); //Seta a cor como cinza do pano de fundo

	//Renderização da Nave
	D2D1_POINT_2F rotationposition = D2D1::Point2F(nave.center.x,nave.center.y);
	D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Rotation(nave.orientation,rotationposition) * D2D1::Matrix3x2F::Translation(nave.position.x - nave.center.x,nave.position.y - nave.center.y);
		
	//Transformação da nave
	if(nave.iHealth > 0){
		pD2DRenderTarget->SetTransform(matrix);
		pD2DRenderTarget->FillGeometry(pD2DDynamicGeometry,pD2DColorYellowBrush);

		if(nave.keyleft)
			nave.orientation -= 3;
		if(nave.keyright)
			nave.orientation += 3;

		if(nave.keydown){
			nave.speed -= 0.02f;
			if(nave.speed <= -2) nave.speed = -2;
		}else if(nave.speed < 0){
			nave.speed += 0.02f;
			if(nave.speed > 0) nave.speed = 0;
		}

		if(nave.keyup){
			nave.speed += 0.04f;
			if(nave.speed > 4) nave.speed = 4;
		}else if(nave.speed > 0){
			nave.speed -= 0.02f;
			if(nave.speed <= 0) nave.speed = 0;
		}

		nave.torque.x = (nave.speed * cos(nave.orientation * M_PI/ 180));
		nave.torque.y = (nave.speed * sin(nave.orientation * M_PI/ 180));

		nave.position.x += nave.torque.x;
		nave.position.y += nave.torque.y;

		int minX = 0, minY = 0, maxX = rtSize.width, maxY = rtSize.height;
		if(nave.position.x < minX)
			nave.position.x = minX;
		if(nave.position.y < minY) nave.position.y = minY;
		if(nave.position.x > maxX)
			nave.position.x = maxX;
		if(nave.position.y > maxY) nave.position.y = maxY;
		
		//Particulas
		
		smoke->transformParticle(pD2DRenderTarget,matrix,pD2DColorYellowBrush);
		if(nave.keyup){
			smoke->addParticle(matrix);
		}

		//Tiros
		shot->transformParticle(pD2DRenderTarget,matrix,pD2DColorYellowBrush, nave.speed);
		if((keyShot && !(iShotFrameCount % shot->getShotLatency())) || (!keyShotPrevState && keyShot)){
			shot->addParticle(matrix);
			iShotFrameCount = 1;
		}
		iShotFrameCount ++;
		iShotFrameCount = iShotFrameCount > 60 ? 1 : iShotFrameCount;
		keyShotPrevState = keyShot;

	}else{
		/*************************************************
		*	Qualquer rotina de destruição da nave aqui
		*************************************************/
		if(!iFlagDestructionAnimation){
			float fDots = 0;
			if(SUCCEEDED(hr)){
				hr = pD2DDynamicGeometry->ComputeArea(D2D1::Matrix3x2F::Identity(),&fDots);
			}

			if(SUCCEEDED(hr)){
				int i = 0;
				while(i < fDots){
					destruction->addParticle(matrix,360);
					i++;
				}
			}
			iFlagDestructionAnimation = 1;
			iGameOver = 1;
			shot->disposeAllParticles();
			smoke->disposeAllParticles();
		}

	}
		
	//Animação da destruição da nave
	destruction->transformParticle(pD2DRenderTarget,matrix,pD2DColorYellowBrush);

	//Obstáculo
	obstacle->transformObstacle(pD2DRenderTarget,pD2DColorBlackBrush, shot);
	if(RangedRand(0,100) < 1 && !keyF1){
		obstacle->addObstacle(rtSize.width,rtSize.height,pD2DFactory);
	}
	obstacle->transformDestroyedObstacle(pD2DRenderTarget,pD2DColorBlackBrush);

		
	//Texto
	wchar_t *pSText = new wchar_t[50]();
	size_t cbDest = 50 * sizeof(wchar_t);
	StringCbPrintf(pSText,cbDest,L"Score: %d \nShield: %d%%",nave.iPontuation,nave.iHealth);
		
	D2D1_RECT_F layoutRect = D2D1::RectF(0,0,400,120);
	pD2DColorBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	pD2DRenderTarget->DrawTextW(pSText,50,pDWriteTextFormat,layoutRect,pD2DColorBlackBrush);
	delete pSText;
	pD2DColorBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

	if(iGameOver){
		wchar_t sGameOver[] = L"Game Over!\nPress F2 to restart.\0";
		int iLen = lstrlen(sGameOver);
		D2D1_RECT_F rectGameOver = D2D1::RectF(
        rtSize.width/2 - 100.0f,
        rtSize.height/2 - 50.0f,
        rtSize.width/2 + 200.0f,
        rtSize.height/2 + 100.0f
        );

		pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		pD2DColorBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		pD2DRenderTarget->DrawTextW(sGameOver,iLen,pDWriteTextFormat,rectGameOver,pD2DColorBlackBrush);
		pD2DColorBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	}

	pD2DRenderTarget->EndDraw();

	if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
	return hr;
}

void DxComponent::OnResize(UINT width, UINT height)
{
    if (pD2DRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        pD2DRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}

void DxComponent::Restart(){
	if(iGameOver){
		nave.iHealth = 100;
		iGameOver = 0;
		iFlagDestructionAnimation = 0;
		RECT rc;
		GetClientRect(hWnd,&rc);
		nave.position.x = (rc.right - rc.left)/2;
		nave.position.y = (rc.bottom - rc.top)/2;
		nave.iPontuation = 0;
		nave.speed = 0;
		nave.orientation = -90;
	}
}

LRESULT CALLBACK DxComponent::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DxComponent *pDxComponent = (DxComponent *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            PtrToUlong(pDxComponent)
            );

        result = 1;
    }
    else
    {
        DxComponent *pDxApp = reinterpret_cast<DxComponent *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hWnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pDxApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
					/***************************************
					int const arraysize = 50;
					TCHAR* pszDest = new TCHAR[arraysize](); 
					size_t cbDest = arraysize * sizeof(TCHAR);
					StringCbPrintf(pszDest,cbDest,L"Width: %d \nHeight: %d",width,height);
					int msgboxID = MessageBox(NULL, pszDest, L"Debug", MB_ICONEXCLAMATION | MB_OK);
					//****************************************/
                    pDxApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hWnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
                    pDxApp->OnRender();
                    //ValidateRect(hWnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
			case WM_KEYDOWN: 
				switch (wParam){
				case VK_UP:
					pDxApp->nave.keyup = 1;
					//int msgboxID = MessageBox(NULL, L"Testando keyup", L"Debug", MB_ICONEXCLAMATION | MB_OK);
					break;
				case VK_LEFT:
					pDxApp->nave.keyleft = 1;
					break;
				case VK_RIGHT:
					pDxApp->nave.keyright = 1;
					break;
				case VK_DOWN:
					pDxApp->nave.keydown = 1;
					break;
				case VK_F1:
					{
						if(pDxApp->keyF1 == 0)
							pDxApp->keyF1 = 1;
						else
							pDxApp->keyF1 = 0;
					}
					break;
				case VK_F2:
					pDxApp->Restart();
					break;
				case 0x41:
					pDxApp->keyShot = 1;
					break;
				case VK_SPACE:
					if(pDxApp->iGameState == GAME_STATE_ONMENU)
						pDxApp->iGameState = GAME_STATE_RUNNING;
					break;
				}
				break;
			case WM_KEYUP:
				switch (wParam){
				case VK_UP:
					pDxApp->nave.keyup = 0;
					//int msgboxID = MessageBox(NULL, L"Testando keyup", L"Debug", MB_ICONEXCLAMATION | MB_OK);
					break;
				case VK_LEFT:
					pDxApp->nave.keyleft = 0;
					break;
				case VK_RIGHT:
					pDxApp->nave.keyright = 0;
					break;
				case VK_DOWN:
					pDxApp->nave.keydown = 0;
					break;
				case 0x41:
					pDxApp->keyShot = 0;
					break;
				}
				break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    return result;
}
