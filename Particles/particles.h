#define _USE_MATH_DEFINES
#define _USE_PARTICLES

#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>

#ifndef _D2D1_H_
	#include <d2d1.h>
	#include <d2d1helper.h>
#endif

__declspec(dllexport) int RangedRand( int range_min, int range_max )
{
	// Generate random numbers in the half-closed interval
	// [range_min, range_max). In other words,
	// range_min <= random number < range_max
	//srand( (unsigned)time( NULL )); 
	return (double)rand() / (RAND_MAX + 1) * (range_max - range_min)
            + range_min;
}

typedef struct particle{
	D2D1_RECT_F dots;
	D2D1_POINT_2F position;
	D2D1_MATRIX_3X2_F matrix;
	float angle;
	float maxDistance;
	float speed;
	float currDistance;
	//INT64 x[100000]; //Colocando um array enorme de int64 para testar o gerenciamento de memória.
	struct particle *nextParticle;
} PARTICLE;

class Smoke{
public:
	__declspec(dllexport) Smoke();
	__declspec(dllexport) ~Smoke();

private:
	//PARTICLE particles;
	PARTICLE *ptParticle;
	D2D1_POINT_2F initialPosition;

public:
	__declspec(dllexport) void initialize(D2D1_MATRIX_3X2_F matrix);
	__declspec(dllexport) void addParticle(D2D1_MATRIX_3X2_F matrix);
	__declspec(dllexport) void addParticle(D2D1_MATRIX_3X2_F matrix, int iMaxAngle);
	__declspec(dllexport) void disposeNextParticle(PARTICLE *ptPrevParticle);
	__declspec(dllexport) void disposeParticle(PARTICLE *ptPrevParticleDisposable);
	__declspec(dllexport) void transformParticle(ID2D1HwndRenderTarget *renderTarget, D2D1_MATRIX_3X2_F matrix, ID2D1SolidColorBrush *brush);
	__declspec(dllexport) void disposeAllParticles();
};

class Shot{
public:
	__declspec(dllexport) Shot();
	__declspec(dllexport) ~Shot();
	PARTICLE *ptParticle;
	int power;

private:
	int iShotLatency;

public:
	__declspec(dllexport) int getShotLatency();
	__declspec(dllexport) void addParticle(D2D1_MATRIX_3X2_F matrix);
	__declspec(dllexport) void disposeParticle(PARTICLE *ptPrevParticleDisposable);
	__declspec(dllexport) void transformParticle(ID2D1HwndRenderTarget *renderTarget, D2D1_MATRIX_3X2_F matrix, ID2D1SolidColorBrush *brush, int iCurrSpeed);
	__declspec(dllexport) void disposeAllParticles();
};