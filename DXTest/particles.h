#include <stdlib.h>
#include <windows.h>

#ifndef _D2D1_H_
	#include <d2d1.h>
#include <d2d1helper.h>
#endif

int RangedRand( int range_min, int range_max )
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
	Smoke();
	~Smoke();

private:
	//PARTICLE particles;
	PARTICLE *ptParticle;
	D2D1_POINT_2F initialPosition;

public:
	void initialize(D2D1_MATRIX_3X2_F matrix);
	void addParticle(D2D1_MATRIX_3X2_F matrix);
	void disposeNextParticle(PARTICLE *ptPrevParticle);
	void disposeParticle(PARTICLE *ptPrevParticleDisposable);
	void transformParticle(ID2D1HwndRenderTarget *renderTarget, D2D1_MATRIX_3X2_F matrix, ID2D1SolidColorBrush *brush);

};

