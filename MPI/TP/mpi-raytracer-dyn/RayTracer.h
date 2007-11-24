/**
Universidade Federal do Rio Grande do Sul
CMP255 - Programacao paralela para processamento de alto desempenho
Nicolas Maillard
Aluno: Carlos Eduardo Benevides Bezerra
Semestre: 2007/1
Porto Alegre, Rio Grande do Sul, Brasil
31 de maio de 2007

Descricao:
	Esse algoritmo executa ray-tracing, que eh uma tecnica de renderizacao grafica
baseada em raios que "partem" do ponto de visao e sao refletidos aos colidirem 
com as formas presentes na cena 3D. O trabalho consistiu em paralelizar um codigo
de ray-tracing, utilizando MPI. O codigo original, que eh sequencial, pode ser
encontrado em:
	
	http://sourceforge.net/projects/simpleraytracer/

	Eh gerado um arquivo de saida, sendo o padrao "default.ppm". Pode ser
especificado outro nome como primeiro argumento da linha de comando.
Ha outras opcoes de parametros de linha de comando, que podem
ser encontrados no arquivo /simpleraytracer/readme.txt.

	No que se refere a corretude, pode-se comparar os arquivos de saida gerados
com diferentes numeros de threads, para verificar se os resultados sao identicos.
Alem disso, pode-se ainda comparar com o resultado encontrado com o codigo-fonte
do algoritmo sequencial original, que estah disponivel no sourceforge.
**/

/** RayTraceCanvas.h
 */

#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "unit_limiter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <string>

//#include <SDL/SDL.h>
//#include <SDL.h>
		
#include <mpi.h>

#include<string>
#include "Point.h"
#include "Shape.h"
#include "Color.h"
#include "Light.h"
		
#define TASK_OPEN 0
#define TASK_SENT 1
#define TASK_DONE 2
		
#define PROCESS_READY 0
#define PROCESS_RECEIVING 1
#define PROCESS_WORKING 2
		

typedef struct {
	unsigned char R, G, B;
} myPixel;

class RayTracer
{

public:
	RayTracer(int rank, int nprocs , int bl_size);
	~RayTracer();

	void RayTrace();
	void RayTraceBlock(int bl_start, int bl_end);

	void AddShape(Shape* pShape);
	void AddLight(Light* pLight);

	void BackgroundColor(const Color& rBackgroundColor);
	void EyePoint(const Point& rEyePoint);
	void AmbientColor(const Color& rAmbient);
	void AmbientIntensity(double vAmbientIntensity);

	void OutputFilename(const string& rOutputFilename);
	
private:
	
	Color Render(const Ray& rRay, Color& tmpSpecular, int tmpRecursions = 0, bool vIsReflecting = false, const Shape* pReflectingFrom = 0 ); // vEyeRay should be true if the ray we are tracing is a ray from the eye, otherwise it should be false
	Shape* QueryScene(const Ray& rRay, Point& rIntersectionPoint, bool vIsReflecting = false, const Shape* pReflectingFrom = 0);
	double Shade(const Shape* pShapeToShade, const Point& rPointOnShapeToShade, Color& tmpSpecular);
	double Specular(const Shape* pShapeToShade, const Point& rPointOnShapeToShade, const Light* pLight);

#ifdef _SDL_H	
	void drawOnScreen(SDL_Surface* screen, int x, int y, unsigned char R, unsigned char G, unsigned char B);
	void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
#endif
	void handleCalculatedPixel(myPixel** screenBuffer, unsigned char* lineBuffer,
														 int x, int y, unsigned char R, unsigned char G, unsigned char B);
	void handleReceivedLine(myPixel** screenBuffer, unsigned char* lineBuffer);
	void bufferizeLine (int y, myPixel** screenBuffer, unsigned char* lineBuffer);
	
	void sendTask (int p_id, int task_number);
	int checkDistributedTasks();
	int nextTask();
	int isWaitingForResults();
	void waitDistributedTasks ();
	void saveToFile (myPixel** screenFileBuffer);
	void finishRemoteProcesses();
		
	Color mBackgroundColor;
	Color mAmbientColor;
	Point mEyePoint;
	Color mSpecularColor;
	double mAmbientIntensity;
	
	string mOutputFilename;

	ShapeList* mpShapes;
	LightList* mpLights;

	int mRecursions;
	int mRecursionLimit;
	int mAntiAliasDetail;
	int mWidth;
	int mHeight;
	
#ifdef _SDL_H
	SDL_Surface* screen;
#endif
	
	int mRank;
	int mNProcs;
	int mBlkSize;
	
	myPixel** screenFileBuffer;
	unsigned char** m_vImgLineBuffer;			

	int okLines[400];

	int mWholeWorkSent;
	int mNTasks;
	int* m_vTasks;
	int* m_vRecebeu;
	int* m_vSentTask;
	int* m_vProcessBlockCounter;
	MPI_Status* m_vRecv_status;
	MPI_Status* m_vSend_status;
	MPI_Request* m_vReq_recv;
	MPI_Request* m_vReq_send;
	
	int* m_vProcesses;

};

#endif
