#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <cstdlib>
#include <stdlib.h>
#include "Colors_JesusHadMercyOnHisApostles.h"
#include <ctime>
#include "Position_PikachuHatedAshButBefriendedHim.h"
#include "CL\cl.h"
#include "BoidsCudantOpenCL.h"

//CHANGE THIS FOR MORE OR LESS THREADS
int numberOfFlocks = 512;
int numberOfBoids = 8;

Boid* Boids;
Color* Colors;

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(5 / 256.0f, 154 / 256.0f, 244 / 256.0f, 1);

	for (int i = 0; i < numberOfFlocks; i++)
	{
		for (int j = 0; j < numberOfBoids; j++)
		{
			auto concernedBoid = Boids + (numberOfBoids * i) + j;
			auto color = Colors + i;
			glColor3b(color->R, color->G, color->B);
			glBegin(GL_TRIANGLES);
			glVertex2f(concernedBoid->Coordinate.X, concernedBoid->Coordinate.Y + 0.005);
			glVertex2f(concernedBoid->Coordinate.X - 0.0025, concernedBoid->Coordinate.Y - 0.0025);
			glVertex2f(concernedBoid->Coordinate.X + 0.0025, concernedBoid->Coordinate.Y - 0.0025);
			glEnd();
		}
	}
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int ptr, char** charptr)
{
	int counter = 0;

	srand((time(0)));
	Boids = (Boid*)malloc(sizeof(Boid) * numberOfFlocks * numberOfBoids);
	Colors = (Color*)malloc(sizeof(Color) * numberOfFlocks);

	// GENERATE THE FLOCKS AND THE BOIDS.
	for (int i = 0; i < numberOfFlocks * numberOfBoids; i++) {
		Boid boid;
		boid.Coordinate.X = (float)rand() / RAND_MAX;
		boid.Coordinate.Y = (float)rand() / RAND_MAX;

		*(Boids + i) = boid;
	}

	for (int i = 0; i < numberOfFlocks; i++)
	{
		Color color;

		color.R = rand() % 256;
		color.G = rand() % 256;
		color.B = rand() % 256;

		*(Colors + i) = color;
	}


	glutInit(&ptr, charptr);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(1960, 1080);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("PROJECT426_29676279");
	glutDisplayFunc(draw);

	int timeToDrawGraphics = 0;
	int timeToUpdate = 0;

	while (true)
	{
		auto currentTime = GetTickCount();

		if (currentTime >= timeToDrawGraphics) {
			glutMainLoopEvent();
			timeToDrawGraphics = currentTime + 15;
		}

		if (currentTime >= timeToUpdate) {

			doSomeGPUComputing(Boids, numberOfBoids, numberOfFlocks);

			timeToUpdate = currentTime + 33;
		}
	}
}
