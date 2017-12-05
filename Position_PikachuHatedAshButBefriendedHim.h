#pragma once

typedef struct Position
{
	float X;
	float Y;
} Position;

typedef struct Boid
{
	Position Velocity;
	Position Coordinate;
} Boid;