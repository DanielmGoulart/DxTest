#include "obstacles.h"

Obstacle::Obstacle(){
	ptObstacle = NULL;
	ptDestroyedObstacle = NULL;
	time_t timeSeed = (unsigned)time( NULL );
	srand(timeSeed);
}

HRESULT Obstacle::addObstacle(int width, int height, ID2D1Factory *ptFactory){
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	OBSTACLE *ptInitObs = ptObstacle;
	OBSTACLE *ptObst = new OBSTACLE();
	ptObst->fSpeed = RangedRand_F(20,80)/20;
	ptObst->fRotationAngle = 0;
	ptObst->fRotationSpeed = RangedRand_F(1, 15)/10;
	ptObst->iLifetime = 0;
	ptObst->iTotalLifeTime = RangedRand_I(600,1200);
	ptObst->ihitPoints = RangedRand_I(15,26);
	ptObst->ptONextObstacle = NULL;
	ptObst->D2D1mMatrix = D2D1::Matrix3x2F::Identity();
	//Definindo posição inicial
	int initialPos = RangedRand_I(1,5);
	switch(initialPos){
	case 1:
		ptObst->D2D1ptPosition.x = 0;
		ptObst->D2D1ptPosition.y = RangedRand_I(1,height);
		ptObst->fTranslationAngle = RangedRand_I(0,90) - 45;
		break;
	case 2:
		ptObst->D2D1ptPosition.x = RangedRand_I(1,width);
		ptObst->D2D1ptPosition.y = 0;
		ptObst->fTranslationAngle = RangedRand_I(0,90) + 90 - 45;
		break;
	case 3:
		ptObst->D2D1ptPosition.x = width;
		ptObst->D2D1ptPosition.y = RangedRand_I(1,height);
		ptObst->fTranslationAngle = RangedRand_I(0,90) + 180 - 45;
		break;
	case 4:
		ptObst->D2D1ptPosition.x = RangedRand_I(1,width);
		ptObst->D2D1ptPosition.y = height;
		ptObst->fTranslationAngle = RangedRand_I(0,90) + 270 - 45;
		break;
	}
	if(SUCCEEDED(hr))
		hr = ptFactory->CreatePathGeometry(&ptObst->ptD2DDynamicGeometry);
	
	if(SUCCEEDED(hr))
		hr = ptObst->ptD2DDynamicGeometry->Open(&pSink);

	if(SUCCEEDED(hr)){

		ptObst->iDots = RangedRand_I(3,7);
		double dAngle = (M_PI * 2)/ptObst->iDots;
		double dInitialAngle = 0;
		int iScalar = RangedRand_I(20,50);
		ptObst->ptD2D1pDots = new D2D1_POINT_2F[ptObst->iDots]();
		ZeroMemory(ptObst->ptD2D1pDots, sizeof(D2D1_POINT_2F)*ptObst->iDots);
		D2D1_POINT_2F D2D1pInitialDot = D2D1::Point2F(0,0);
		ptObst->D2D1ptInitialPosition = D2D1pInitialDot;
		pSink->BeginFigure(
			D2D1::Point2F(D2D1pInitialDot.x + iScalar * cos(dInitialAngle),D2D1pInitialDot.y + iScalar * sin(dInitialAngle)),
			D2D1_FIGURE_BEGIN_FILLED);
		//Definindo os iDots pontos do polígono no vetor ptD2D1pDots[0...iDots-1]
		for(int i = 0; i < ptObst->iDots; i++){
			ptObst->ptD2D1pDots[i].x = D2D1pInitialDot.x + iScalar * cos(dInitialAngle);
			ptObst->ptD2D1pDots[i].y = D2D1pInitialDot.y + iScalar * sin(dInitialAngle);
			dInitialAngle += dAngle;
		}
		pSink->AddLines(ptObst->ptD2D1pDots, ptObst->iDots);
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        hr = pSink->Close();
		pSink->Release();
		//delete ptD2D1pDots;
	}
	//Adiciona o objeto na lista
	if(!ptInitObs){
		ptObstacle = ptObst;
	}else{
		ptObst->ptONextObstacle = ptObstacle;
		ptObstacle = ptObst;
	}

	return hr;
}

void Obstacle::transformObstacle(ID2D1HwndRenderTarget *renderTarget, ID2D1SolidColorBrush *brush, Shot *shot){
	OBSTACLE* ptPrevObstacle = NULL;
	OBSTACLE* ptObst = ptObstacle;
	float fGeometryLength = 0;
	while(ptObst){
		ptObst->iLifetime++;
		D2D1_SIZE_F size = renderTarget->GetSize();
		D2D1_POINT_2F D2D1p2fAbsolutePosition;
		D2D1p2fAbsolutePosition.x = (ptObst->D2D1ptPosition.x * ptObst->D2D1mMatrix._11) + (ptObst->D2D1ptPosition.y * ptObst->D2D1mMatrix._21) + ptObst->D2D1mMatrix._31;
		D2D1p2fAbsolutePosition.y = (ptObst->D2D1ptPosition.x * ptObst->D2D1mMatrix._12) + (ptObst->D2D1ptPosition.y * ptObst->D2D1mMatrix._22) + ptObst->D2D1mMatrix._32;
		if(ptObst->iLifetime >= ptObst->iTotalLifeTime ||
		D2D1p2fAbsolutePosition.x < 0 - 50||
		D2D1p2fAbsolutePosition.x > size.width + 50 ||
		D2D1p2fAbsolutePosition.y < 0 - 50 ||
		D2D1p2fAbsolutePosition.y > size.height +50
		){
			ptObst = ptObst->ptONextObstacle;
			disposeObstacle(ptPrevObstacle);
			continue;
		}

		ptObst->D2D1ptPosition.x += ptObst->fSpeed * cos(ptObst->fTranslationAngle * M_PI/180);
		ptObst->D2D1ptPosition.y += ptObst->fSpeed * sin(ptObst->fTranslationAngle * M_PI/180);
		ptObst->fRotationAngle += ptObst->fRotationSpeed;
		D2D1_MATRIX_3X2_F D2D2mRotation =  D2D1::Matrix3x2F::Rotation(ptObst->fRotationAngle,ptObst->D2D1ptInitialPosition);
		D2D1_MATRIX_3X2_F D2D2mtranslation = D2D1::Matrix3x2F::Translation(ptObst->D2D1ptPosition.x - ptObst->D2D1ptInitialPosition.x,
																		   ptObst->D2D1ptPosition.y - ptObst->D2D1ptInitialPosition.y);
		D2D1_MATRIX_3X2_F D2D2mTransform = D2D2mRotation * D2D2mtranslation * ptObst->D2D1mMatrix;
		//ptObst->D2D1mMatrix = D2D2mTransform;
		HRESULT hr = S_OK;
		D2D1_POINT_2F D2D1pCollision = {0,0};

		/******************************************************************************
		*	Rotina para o tratamento de colisões entre os tiros e os obstáculos
		*	Ao detectar a colisão verifica o HP do objeto e subtrai da potencia
		*	do tiro.
		*******************************************************************************/
		if(SUCCEEDED(hr)){
			PARTICLE *ptTransfParticle = shot->ptParticle, *ptPrevTransfParticle = NULL;
			BOOL bCollide = false, bDestroy = false, bCollideNave = false;
			hr = checkNaveCollision(ptObst, D2D2mTransform, &bCollideNave);
			//Determinando a posição da nave após a colisão
			if(SUCCEEDED(hr) && bCollideNave && ptNave->iHealth > 0){
				brush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
				float fAngle = atan2f(ptObst->D2D1ptPosition.y - ptNave->position.y, ptObst->D2D1ptPosition.x - ptNave->position.x);
				
				while(bCollideNave && SUCCEEDED(hr)){
					ptNave->position.x = ptNave->position.x - 1*cos(fAngle);
					ptNave->position.y = ptNave->position.y - 1*sin(fAngle);
					hr = ptObst->ptD2DDynamicGeometry->FillContainsPoint(ptNave->position,D2D2mTransform,&bCollideNave);
				}

				ptNave->iHealth -= 1;
				if(ptNave->iHealth < 0) ptNave->iHealth = 0;
				//ptNave->position.x = ptObst->D2D1ptPosition.x + (ptObst->D2D1ptPosition.x - ptNave->position.x);
				//ptNave->position.y = ptObst->D2D1ptPosition.y + (ptObst->D2D1ptPosition.y - ptNave->position.y);
				
			}

			//D2D1_MATRIX_3X2_F mCollisionMatrix;
			//Determinando a colisão das partículas de tiro
			while(ptTransfParticle){
				//Pegando a posição absoluta
				//D2D1pCollision.x = (ptTransfParticle->position.x*ptTransfParticle->matrix._11) + (ptTransfParticle->matrix._21*ptTransfParticle->position.y) + (ptTransfParticle->matrix._31);
				//D2D1pCollision.y = (ptTransfParticle->position.x*ptTransfParticle->matrix._12) + (ptTransfParticle->matrix._22*ptTransfParticle->position.y) + (ptTransfParticle->matrix._32);
				//Calculando a posição através da posição relativa
				hr = checkCollision(ptTransfParticle,ptObst,D2D2mTransform, &bCollide);
				//Colidiu!!
				if(SUCCEEDED(hr) && bCollide){
					ptObst->ihitPoints -= shot->power;
					if(ptObst->ihitPoints <= 0){
						bDestroy = true;
						break;
					}else{
						ptNave->iPontuation += 1;
						brush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
						ptTransfParticle = ptTransfParticle->nextParticle;
						shot->disposeParticle(ptPrevTransfParticle);
						continue;
					}
				}
				ptPrevTransfParticle = ptTransfParticle;
				ptTransfParticle = ptTransfParticle->nextParticle;
			}
			if(bDestroy){
				ptNave->iPontuation += 10;
				ID2D1Factory *ptD2D1Factory;
				renderTarget->GetFactory(&ptD2D1Factory);
				ptObst->ptD2DDynamicGeometry->ComputeLength(D2D2mTransform,&fGeometryLength);
				if(fGeometryLength < 180)
					addDestroyedObstacle(ptObst,ptD2D1Factory,D2D2mTransform);
				else
					addDestroyedCollidableObstacle(ptObst,ptD2D1Factory,D2D2mTransform);
				brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
				if(bCollideNave) brush->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
				ptObst = ptObst->ptONextObstacle;
				disposeObstacle(ptPrevObstacle);
				continue;
			}
		}

		renderTarget->SetTransform(D2D2mTransform);	
		renderTarget->FillGeometry(ptObst->ptD2DDynamicGeometry,brush);
		brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		//renderTarget->DrawRectangle(&ptRectCollision,brush); //DEBUG
		ptPrevObstacle = ptObst;
		ptObst = ptObst->ptONextObstacle;
	}
}

void Obstacle::disposeObstacle(OBSTACLE *ptPrevObstacle){
	OBSTACLE *ptDisposableObst;
	OBSTACLE *ptFirstObst = ptObstacle;
	if(!ptPrevObstacle){
		ptDisposableObst = ptFirstObst;
		ptObstacle = ptFirstObst->ptONextObstacle;
		ptDisposableObst->ptD2DDynamicGeometry->Release();
		delete[ptDisposableObst->iDots] ptDisposableObst->ptD2D1pDots;
		delete ptDisposableObst;
	}else{
		ptDisposableObst = ptPrevObstacle->ptONextObstacle;
		ptPrevObstacle->ptONextObstacle = ptDisposableObst->ptONextObstacle;
		ptDisposableObst->ptD2DDynamicGeometry->Release();
		delete[ptDisposableObst->iDots] ptDisposableObst->ptD2D1pDots;
		delete ptDisposableObst;
	}
}

HRESULT Obstacle::addDestroyedObstacle(OBSTACLE *ptObstacle, ID2D1Factory *ptFactory, D2D1_MATRIX_3X2_F matrix){
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;

	int inumDots = 0, i = 0, j = 0;
	D2D1_POINT_2F D2D1p2fDots = {0,0};
	D2D1_POINT_2F *pD2D1p2fDots = &ptObstacle->ptD2D1pDots[i];
	//Pegando o ponto central da figura
	while(inumDots < ptObstacle->iDots){
		D2D1p2fDots.x += pD2D1p2fDots->x; 
		D2D1p2fDots.y += pD2D1p2fDots->y;
		inumDots++;
		pD2D1p2fDots = &ptObstacle->ptD2D1pDots[inumDots];
	}
	D2D1p2fDots.x = D2D1p2fDots.x/inumDots;
	D2D1p2fDots.y = D2D1p2fDots.y/inumDots;
	OBSTACLE *ptInitDestObs = ptDestroyedObstacle;
	OBSTACLE *ptDestObst;
	//Criando os triangulos de forma iterativa
	while(i < inumDots){
		ptDestObst = new OBSTACLE();
		ptDestObst->D2D1mMatrix = matrix;
		D2D1_POINT_2F D2D1ptBarycenter;
		j = i+1;
		if(j >= inumDots) j = 0;
		D2D1_POINT_2F D2D1p2fDotsGeometry[] = {D2D1p2fDots,ptObstacle->ptD2D1pDots[i],ptObstacle->ptD2D1pDots[j]};
		ptDestObst->fTranslationAngle = atan2((ptObstacle->ptD2D1pDots[i].y + ptObstacle->ptD2D1pDots[j].y)/2,
											  (ptObstacle->ptD2D1pDots[i].x + ptObstacle->ptD2D1pDots[j].x)/2) * 180 / M_PI;

		D2D1ptBarycenter.x = (D2D1p2fDots.x + ptObstacle->ptD2D1pDots[i].x + ptObstacle->ptD2D1pDots[j].x) /3;
		D2D1ptBarycenter.y = (D2D1p2fDots.y + ptObstacle->ptD2D1pDots[i].y + ptObstacle->ptD2D1pDots[j].y) /3;
		ptDestObst->fRotationSpeed = RangedRand_F(1, 15)/10;
		ptDestObst->fRotationAngle = 0;
		ptDestObst->fSpeed = RangedRand_F(10,20)/20;
		ptDestObst->D2D1ptPosition = D2D1p2fDots;
		ptDestObst->iLifetime = 0;
		ptDestObst->ptONextObstacle = NULL;
		ptDestObst->iTotalLifeTime = RangedRand_I(180,300);
		ptDestObst->D2D1ptInitialPosition = D2D1ptBarycenter;
		ptDestObst->D2D1ptPosition.x = D2D1ptBarycenter.x;
		ptDestObst->D2D1ptPosition.y = D2D1ptBarycenter.y;
		//ptDestObst->D2D1mMatrix = D2D1::Matrix3x2F::Identity();

		hr = ptFactory->CreatePathGeometry(&ptDestObst->ptD2DDynamicGeometry);
		if(SUCCEEDED(hr)){
			ptDestObst->ptD2DDynamicGeometry->Open(&pSink);
		}
		if(SUCCEEDED(hr)){
			pSink->BeginFigure(D2D1p2fDots,D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddLines(D2D1p2fDotsGeometry,3);
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			hr = pSink->Close();
			pSink->Release();
		}

		//Adiciona o objeto na lista
		if(!ptInitDestObs){
			ptDestroyedObstacle = ptDestObst;
			ptInitDestObs = ptDestObst;
		}else{
			ptDestObst->ptONextObstacle = ptDestroyedObstacle;
			ptDestroyedObstacle = ptDestObst;
		}

		i++;
	}
	return hr;
}

HRESULT Obstacle::addDestroyedCollidableObstacle(OBSTACLE *ptDestObstacle, ID2D1Factory *ptFactory, D2D1_MATRIX_3X2_F matrix){
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	int iCount = 0, i = 0, j = 0;
	D2D1_POINT_2F D2D1p2fCenterPoint = {0,0};
	D2D1_POINT_2F *pD2D1p2fDots = &ptDestObstacle->ptD2D1pDots[i];
	//Pegando os pontos da geometria
	while(iCount < ptDestObstacle->iDots){
		D2D1p2fCenterPoint.x += pD2D1p2fDots->x; 
		D2D1p2fCenterPoint.y += pD2D1p2fDots->y;
		iCount++;
		pD2D1p2fDots = &ptDestObstacle->ptD2D1pDots[iCount];
	}
	D2D1p2fCenterPoint.x = D2D1p2fCenterPoint.x/iCount;
	D2D1p2fCenterPoint.y = D2D1p2fCenterPoint.y/iCount;

	OBSTACLE *ptInitialObstacle = NULL;
	OBSTACLE *ptNewObstacle;
	while(i < iCount){
		ptNewObstacle = new OBSTACLE();
		ptNewObstacle->iDots = 3;
		j = i+1;
		if(j >= iCount) j = 0;
		ptNewObstacle->ptD2D1pDots = new D2D1_POINT_2F[3]();
		ptNewObstacle->D2D1mMatrix = matrix;
		//Definindo atributos relativos aos triângulos
		ptNewObstacle->ptD2D1pDots[0] = D2D1p2fCenterPoint;
		ptNewObstacle->ptD2D1pDots[1] = ptDestObstacle->ptD2D1pDots[i];
		ptNewObstacle->ptD2D1pDots[2] = ptDestObstacle->ptD2D1pDots[j];
		ptNewObstacle->fTranslationAngle = atan2((ptDestObstacle->ptD2D1pDots[i].y + ptDestObstacle->ptD2D1pDots[j].y)/2,
												(ptDestObstacle->ptD2D1pDots[i].x + ptDestObstacle->ptD2D1pDots[j].x)/2) * 180 / M_PI;
		D2D1_POINT_2F D2D1p2fBarycenter;
		D2D1p2fBarycenter.x = (ptNewObstacle->ptD2D1pDots[0].x + ptNewObstacle->ptD2D1pDots[1].x + ptNewObstacle->ptD2D1pDots[2].x) /3;
		D2D1p2fBarycenter.y = (ptNewObstacle->ptD2D1pDots[0].y + ptNewObstacle->ptD2D1pDots[1].y + ptNewObstacle->ptD2D1pDots[2].y) /3;
		ptNewObstacle->D2D1ptPosition.x = D2D1p2fBarycenter.x;
		ptNewObstacle->D2D1ptPosition.y = D2D1p2fBarycenter.y;

		//Definindo atributos diversos
		ptNewObstacle->fSpeed = RangedRand_F(20,40)/20;
		ptNewObstacle->fRotationAngle = 0;
		ptNewObstacle->fRotationSpeed = RangedRand_F(1, 15)/10;
		ptNewObstacle->iLifetime = 0;
		ptNewObstacle->iTotalLifeTime = RangedRand_I(600,1200);
		ptNewObstacle->ihitPoints = RangedRand_I(10,21);
		ptNewObstacle->ptONextObstacle = NULL;
		ptNewObstacle->D2D1ptInitialPosition = D2D1p2fBarycenter;

		//Criando a geometria
		hr = ptFactory->CreatePathGeometry(&ptNewObstacle->ptD2DDynamicGeometry);
		if(SUCCEEDED(hr)){
			ptNewObstacle->ptD2DDynamicGeometry->Open(&pSink);
		}
		if(SUCCEEDED(hr)){
			pSink->BeginFigure(ptNewObstacle->ptD2D1pDots[0],D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddLines(ptNewObstacle->ptD2D1pDots,3);
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			hr = pSink->Close();
			pSink->Release();
		}

		//Adicionando na lista
		ptInitialObstacle = ptDestObstacle->ptONextObstacle;
		ptDestObstacle->ptONextObstacle = ptNewObstacle;
		ptNewObstacle->ptONextObstacle = ptInitialObstacle;

		i++;
	}

	return hr;
}


void Obstacle::transformDestroyedObstacle(ID2D1HwndRenderTarget *renderTarget, ID2D1SolidColorBrush *brush){
	OBSTACLE* ptPrevObstacle = NULL;
	OBSTACLE* ptObst = ptDestroyedObstacle;
	while(ptObst){
		ptObst->iLifetime++;
		D2D1_SIZE_F size = renderTarget->GetSize();
		if(ptObst->iLifetime >= ptObst->iTotalLifeTime){
			ptObst = ptObst->ptONextObstacle;
			disposeDestroyedObstacle(ptPrevObstacle);
			continue;
		}
		brush->SetOpacity(1.0f - (float)ptObst->iLifetime / ptObst->iTotalLifeTime);
		ptObst->D2D1ptPosition.x += ptObst->fSpeed * cos(ptObst->fTranslationAngle * M_PI/180);
		ptObst->D2D1ptPosition.y += ptObst->fSpeed * sin(ptObst->fTranslationAngle * M_PI/180);
		ptObst->fRotationAngle += ptObst->fRotationSpeed;
		D2D1_MATRIX_3X2_F D2D2mTransform = D2D1::Matrix3x2F::Rotation(ptObst->fRotationAngle,ptObst->D2D1ptInitialPosition) *
										   D2D1::Matrix3x2F::Translation(ptObst->D2D1ptPosition.x - ptObst->D2D1ptInitialPosition.x,
																		 ptObst->D2D1ptPosition.y - ptObst->D2D1ptInitialPosition.y) *
										   ptObst->D2D1mMatrix;
		//ptObst->D2D1mMatrix = D2D2mTransform * ptObst->D2D1mMatrix;
		
		renderTarget->SetTransform(D2D2mTransform);	
		renderTarget->FillGeometry(ptObst->ptD2DDynamicGeometry,brush);
		//brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		brush->SetOpacity(1);
		ptPrevObstacle = ptObst;
		ptObst = ptObst->ptONextObstacle;
	}
}

void Obstacle::disposeDestroyedObstacle(OBSTACLE *ptPrevObstacle){
	OBSTACLE *ptDisposableObst = NULL;
	OBSTACLE *ptFirstObst = ptDestroyedObstacle;
	if(!ptPrevObstacle){
		ptDisposableObst = ptFirstObst;
		ptDestroyedObstacle = ptFirstObst->ptONextObstacle;
		ptDisposableObst->ptD2DDynamicGeometry->Release();
		delete ptDisposableObst;
	}else{
		ptDisposableObst = ptPrevObstacle->ptONextObstacle;
		ptPrevObstacle->ptONextObstacle = ptDisposableObst->ptONextObstacle;
		ptDisposableObst->ptD2DDynamicGeometry->Release();
		delete ptDisposableObst;
	}
}

void Obstacle::getNave(NAVE *nave){
	ptNave = nave;
}

HRESULT Obstacle::checkCollision(PARTICLE *ptParticle, OBSTACLE *ptObstacle, D2D1_MATRIX_3X2_F D2D1mTransformMatrixObstacle, BOOL *bCollide){
	HRESULT hr = S_FALSE;
	D2D1_MATRIX_3X2_F mCollisionMatrix = ptParticle->matrix;
	if(D2D1InvertMatrix(&mCollisionMatrix))
		hr = ptObstacle->ptD2DDynamicGeometry->FillContainsPoint(ptParticle->position,D2D1mTransformMatrixObstacle * mCollisionMatrix,bCollide);

	return hr;
}

HRESULT Obstacle::checkNaveCollision(OBSTACLE *ptObstacle, D2D1_MATRIX_3X2_F D2D1mTransformMatrixObstacle, BOOL *bCollide){
	HRESULT hr = S_OK;

	hr = ptObstacle->ptD2DDynamicGeometry->FillContainsPoint(ptNave->position,D2D1mTransformMatrixObstacle,bCollide);

	return hr;
}