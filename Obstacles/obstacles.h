#define _USE_MATH_DEFINES
#define _USE_OBSTACLES

#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <particles.h>

#ifndef _D2D1_H_
	#include <d2d1.h>
	#include <d2d1helper.h>
#endif

int getDecimal(int iNumber){
	int i = 1;
	for(;;){
		if((iNumber % 10) || (!iNumber)){
			break;
		}else{
			iNumber = iNumber/10;
			i++;
		}
	}
	return i;
}

int RangedRand_I( int range_min, int range_max )
{
	// Generate random numbers in the half-closed interval
	// [range_min, range_max). In other words,
	// range_min <= random number < range_max
	//srand( (unsigned)time( NULL )); 
	return (double)rand() / (RAND_MAX + 1) * (range_max - range_min)
            + range_min;
}

float RangedRand_F( int range_min, int range_max )
{
	// Generate random numbers in the half-closed interval
	// [range_min, range_max). In other words,
	// range_min <= random number < range_max
	//srand( (unsigned)time( NULL )); 
	return (double)rand() / (RAND_MAX + 1) * (range_max - range_min)
            + range_min;
}

void calculateDots(D2D1_POINT_2F **dots);

typedef struct obstacle{
	D2D1_POINT_2F D2D1ptPosition;
	D2D1_POINT_2F D2D1ptInitialPosition;
	D2D1_MATRIX_3X2_F D2D1mMatrix;
	ID2D1PathGeometry *ptD2DDynamicGeometry;
	float fTranslationAngle;
	float fRotationAngle;
	float fRotationSpeed;
	float fSpeed;
	float fDistance;
	int iDots;
	int ihitPoints;
	int iLifetime;
	int iTotalLifeTime;
	D2D1_POINT_2F* ptD2D1pDots;
	//INT64 x[100000]; //Colocando um array enorme de int64 para testar o gerenciamento de memória.
	struct obstacle *ptONextObstacle;
} OBSTACLE;

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
#define _STRUCT_NAVE_CREATED

class Obstacle{
public:
	__declspec(dllexport) Obstacle();
	__declspec(dllexport) ~Obstacle();

private:
	OBSTACLE *ptObstacle;
	OBSTACLE *ptDestroyedObstacle;
	NAVE *ptNave;

public:
	__declspec(dllexport) HRESULT addObstacle(int width, int height, ID2D1Factory *ptFactory);
	__declspec(dllexport) HRESULT addDestroyedObstacle(OBSTACLE *ptObstacle, ID2D1Factory *ptFactory, D2D1_MATRIX_3X2_F matrix);
	__declspec(dllexport) HRESULT addDestroyedCollidableObstacle(OBSTACLE *ptDestObstacle, ID2D1Factory *ptFactory, D2D1_MATRIX_3X2_F matrix);
	__declspec(dllexport) void transformObstacle(ID2D1HwndRenderTarget *renderTarget, ID2D1SolidColorBrush *brush, Shot *shot);
	__declspec(dllexport) void transformDestroyedObstacle(ID2D1HwndRenderTarget *renderTarget, ID2D1SolidColorBrush *brush);
	__declspec(dllexport) void disposeObstacle(OBSTACLE *ptPrevObstacle);
	__declspec(dllexport) void disposeDestroyedObstacle(OBSTACLE *ptPrevObstacle);
	__declspec(dllexport) void getNave(NAVE *nave);
	HRESULT checkCollision(PARTICLE *ptParticle, OBSTACLE *ptObstacle, D2D1_MATRIX_3X2_F D2D1mTransformMatrixObstacle, BOOL *bCollide);
	HRESULT checkNaveCollision(OBSTACLE *ptObstacle, D2D1_MATRIX_3X2_F D2D1mTransformMatrixObstacle, BOOL *bCollide);
};

