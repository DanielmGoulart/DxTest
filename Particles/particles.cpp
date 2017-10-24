#include "particles.h"

void Smoke::transformParticle(ID2D1HwndRenderTarget *renderTarget, D2D1_MATRIX_3X2_F matrix, ID2D1SolidColorBrush *brush){
	PARTICLE *ptPrevParticle = NULL;
	PARTICLE *ptPartcl = ptParticle;
	while(ptPartcl){
		ptPartcl->currDistance += ptPartcl->speed;
		if(ptPartcl->currDistance >= ptPartcl->maxDistance){
			ptPartcl = ptPartcl->nextParticle;
			disposeParticle(ptPrevParticle);
			continue;
		}
		ptPartcl->position.x = ptPartcl->currDistance * cos(ptPartcl->angle * M_PI/ 180) *-1;
		ptPartcl->position.y = ptPartcl->currDistance * sin(ptPartcl->angle * M_PI/ 180) *-1;
		ptPartcl->dots = D2D1::RectF(ptPartcl->position.x -10 , ptPartcl->position.y, ptPartcl->position.x - 8, ptPartcl->position.y + 2);
		renderTarget->SetTransform(ptPartcl->matrix);
		renderTarget->FillRectangle(ptPartcl->dots,brush);
		ptPrevParticle = ptPartcl;
		ptPartcl = ptPartcl->nextParticle;
	}

	
}

void Smoke::disposeAllParticles(){
	PARTICLE *ptPrevParticle = NULL;
	PARTICLE *ptPartcl = ptParticle;
	while(ptPartcl){
		ptPrevParticle = ptPartcl;
		ptPartcl = ptPartcl->nextParticle;
		delete ptPrevParticle;
	}
	ptParticle = NULL;
}

void Smoke::disposeParticle(PARTICLE *ptPrevParticleDisposable){
	PARTICLE *dispose;
	PARTICLE *ptPartcl = ptParticle;
	if(!ptPrevParticleDisposable){ //Condição para excluir o primeiro elemento
		dispose = ptPartcl;
		ptParticle = ptPartcl->nextParticle;
		delete dispose;
		return;
	}else{
		dispose = ptPrevParticleDisposable->nextParticle;
		ptPrevParticleDisposable->nextParticle = dispose->nextParticle;
		delete dispose;
	}
	//Otimizado para não usar loop
	/************************************
	while(ptPartcl->nextParticle != ptParticleDisposable && ptPartcl->nextParticle != NULL){
		ptPartcl = ptPartcl->nextParticle;
		
	}
	dispose = ptPartcl->nextParticle;
	if(ptPartcl->nextParticle != NULL)ptPartcl->nextParticle = ptPartcl->nextParticle->nextParticle;
	delete dispose;
	*************************************/
}

void Smoke::disposeNextParticle(PARTICLE *ptPrevParticle){
	PARTICLE *partDisposable = ptPrevParticle->nextParticle;
	ptPrevParticle->nextParticle = partDisposable->nextParticle;
	free(partDisposable);
}

void Smoke::addParticle(D2D1_MATRIX_3X2_F matrix){
	PARTICLE *ptPartcl = ptParticle;
	PARTICLE *particle = new PARTICLE();
	particle->position = initialPosition;
	particle->nextParticle = NULL;
	particle->angle = RangedRand(-45,45);
	particle->maxDistance =  RangedRand(10,100);
	particle->speed = RangedRand(1,5);
	particle->currDistance = 0;
	particle->matrix = matrix;
	particle->dots = D2D1::RectF(initialPosition.x -1, initialPosition.y -1, initialPosition.x + 2, initialPosition.y + 2);
	particle->nextParticle = NULL;
	if(!ptPartcl){ //Condição para adicionar o primeiro elemento
		ptParticle = particle;
		return;
	}else{
		particle->nextParticle = ptParticle;
		ptParticle = particle;
		return;
	}
	//Não mais necessário (otimizado para não usar loop)
	/**********************************
	while(ptPartcl->nextParticle){
		ptPartcl = ptPartcl->nextParticle;
	}
	//ptPartcl->nextParticle = new PARTICLE();
	ptPartcl->nextParticle = particle;
	***********************************/
}

void Smoke::addParticle(D2D1_MATRIX_3X2_F matrix, int iMaxAngle){
	PARTICLE *ptPartcl = ptParticle;
	PARTICLE *particle = new PARTICLE();
	particle->position = initialPosition;
	particle->nextParticle = NULL;
	particle->angle = RangedRand(0,iMaxAngle);
	particle->maxDistance =  RangedRand(100,400);
	particle->speed = (float) RangedRand(100,501)/100;
	particle->currDistance = 0;
	particle->matrix = matrix;
	particle->dots = D2D1::RectF(initialPosition.x -1, initialPosition.y -1, initialPosition.x + 2, initialPosition.y + 2);
	particle->nextParticle = NULL;
	if(!ptPartcl){ //Condição para adicionar o primeiro elemento
		ptParticle = particle;
		return;
	}else{
		particle->nextParticle = ptParticle;
		ptParticle = particle;
		return;
	}
}

Smoke::Smoke()
{
	initialPosition.x = 9;
	initialPosition.y = 0;
	time_t timeSeed = (unsigned)time( NULL );
	srand(timeSeed); //Comando para aumentar a aleatoriedade
	//Iniciando a primeira partícula
	/**************************************************
	PARTICLE *particles = new PARTICLE();
	particles->nextParticle = NULL;
	particles->position = initialPosition;
	particles->angle = RangedRand(-45,45);
	particles->speed = RangedRand(1,5);
	particles->maxDistance = RangedRand(10,100);
	particles->currDistance = 0;
	particles->dots = D2D1::RectF(initialPosition.x -1, initialPosition.y -1, initialPosition.x + 2, initialPosition.y + 2);
	***************************************************/
	ptParticle = NULL;
}

Smoke::~Smoke(){

}
//Não mais necessário.
void Smoke::initialize(D2D1_MATRIX_3X2_F matrix){
	PARTICLE particles;
	particles.nextParticle = NULL;
	particles.position = initialPosition;
	particles.angle = RangedRand(-45,45);
	particles.speed = RangedRand(1,5);
	particles.maxDistance = RangedRand(10,100);
	particles.currDistance = 0;
	particles.dots = D2D1::RectF(initialPosition.x -1, initialPosition.y -1, initialPosition.x + 2, initialPosition.y + 2);
	ptParticle = &particles;
}


Shot::Shot(){
	time_t timeSeed = (unsigned)time( NULL );
	srand(timeSeed);
	ptParticle = NULL;
	iShotLatency = 5;
	power = 2;
}

void Shot::addParticle(D2D1_MATRIX_3X2_F matrix){
	PARTICLE *ptPartcl = ptParticle;
	PARTICLE *particle = new PARTICLE();
	particle->position = D2D1::Point2F();
	particle->nextParticle = NULL;
	particle->angle = 0;
	particle->maxDistance = 1000;
	particle->speed = 15;
	particle->currDistance = 0;
	particle->matrix = matrix;
	particle->dots = D2D1::RectF(0, 0, 3, 3);
	particle->nextParticle = NULL;
	if(!ptPartcl){ //Condição para adicionar o primeiro elemento
		ptParticle = particle;
		return;
	}else{
		particle->nextParticle = ptParticle;
		ptParticle = particle;
		return;
	}
}

void Shot::transformParticle(ID2D1HwndRenderTarget *renderTarget, D2D1_MATRIX_3X2_F matrix, ID2D1SolidColorBrush *brush, int iCurrSpeed){
	PARTICLE *ptPrevParticle = NULL;
	PARTICLE *ptPartcl = ptParticle;
	while(ptPartcl){
		ptPartcl->currDistance += ptPartcl->speed + iCurrSpeed;
		if(ptPartcl->currDistance >= ptPartcl->maxDistance){
			ptPartcl = ptPartcl->nextParticle;
			disposeParticle(ptPrevParticle);
			continue;
		}
		ptPartcl->position.x = ptPartcl->currDistance * cos(ptPartcl->angle * M_PI/ 180);
		ptPartcl->position.y = ptPartcl->currDistance * sin(ptPartcl->angle * M_PI/ 180);
		ptPartcl->dots = D2D1::RectF(ptPartcl->position.x, ptPartcl->position.y, ptPartcl->position.x -10 , ptPartcl->position.y + 2);
		renderTarget->SetTransform(ptPartcl->matrix);
		renderTarget->FillRectangle(ptPartcl->dots,brush);
		ptPrevParticle = ptPartcl;
		ptPartcl = ptPartcl->nextParticle;
	}

}

void Shot::disposeParticle(PARTICLE *ptPrevParticleDisposable){
	PARTICLE *dispose;
	PARTICLE *ptPartcl = ptParticle;
	if(!ptPrevParticleDisposable){ //Condição para excluir o primeiro elemento
		dispose = ptPartcl;
		ptParticle = ptPartcl->nextParticle;
		delete dispose;
		return;
	}else{
		dispose = ptPrevParticleDisposable->nextParticle;
		ptPrevParticleDisposable->nextParticle = dispose->nextParticle;
		delete dispose;
	}
}

int Shot::getShotLatency(){
	return iShotLatency;
}

void Shot::disposeAllParticles(){
	PARTICLE *ptPrevParticle = NULL;
	PARTICLE *ptPartcl = ptParticle;
	while(ptPartcl){
		ptPrevParticle = ptPartcl;
		ptPartcl = ptPartcl->nextParticle;
		delete ptPrevParticle;
	}
	ptParticle = NULL;
}