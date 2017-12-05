#include <string>

std::string updateBoidPosition()
{

	return R"(

#include "Position_PikachuHatedAshButBefriendedHim.h"


void limitPosition(__global Boid* boids, int flockIndex, int numBoidsPerFlock, int boidIndex)
{
	//LIMIT POSITION OF BOIDS.
	if ((boids + boidIndex)->Coordinate.X >= 1.0f) {
		(boids + boidIndex)->Velocity.X = -0.01f;
	}
	if ((boids + boidIndex)->Coordinate.X <= -1.0f) {
		(boids + boidIndex)->Velocity.X = 0.01f;
	}
	if ((boids + boidIndex)->Coordinate.Y >= 1.0f) {
		(boids + boidIndex)->Velocity.Y = -0.01f;
	}
	if ((boids + boidIndex)->Coordinate.Y <= -1.0f) {
		(boids + boidIndex)->Velocity.Y = 0.01f;
	}
}

//Boids fly towards the centre of mass of neighboring boids within a flock
Position centerOfMass(__global Boid* boids, int flockIndex, int numBoidsPerFlock, int boidIndex)
{
	Position total;
	

	for (int i = flockIndex * numBoidsPerFlock; i < (1 + flockIndex) * numBoidsPerFlock; i++)
	{
		if (i != boidIndex)
		{
			total.X += (boids + boidIndex)->Coordinate.X;
			total.Y += (boids + boidIndex)->Coordinate.Y;
		}
	}

	total.X = total.X / (numBoidsPerFlock - 1);
	total.Y = total.Y / (numBoidsPerFlock - 1);

	total.X = (total.X - (boids + boidIndex)->Coordinate.X) / 100;
	total.Y = (total.Y - (boids + boidIndex)->Coordinate.Y) / 100;
	
	return total;
}

//Boids stay away from each other, within a flock
Position distanceAway(__global Boid* boids, int flockIndex, int numBoidsPerFlock, int boidIndex)
{
	Position distance;
	

	for (int i = flockIndex * numBoidsPerFlock; i < (1 + flockIndex) * numBoidsPerFlock; i++)
	{
		if (i != boidIndex) {
			float someVelocityX = (boids + boidIndex)->Coordinate.X - (boids + boidIndex)->Coordinate.X;
			float someVelocityY = (boids + boidIndex)->Coordinate.Y - (boids + boidIndex)->Coordinate.Y;

			if (sqrt(someVelocityX * someVelocityX + someVelocityY * someVelocityY) < 0.03)
			{
				distance.X = distance.X - (boids + boidIndex)->Coordinate.X - (boids + boidIndex)->Coordinate.X;
				distance.Y = distance.Y - (boids + boidIndex)->Coordinate.Y - (boids + boidIndex)->Coordinate.Y;
			}
		}
	}

	return distance;
}

	//Birds try to match velocity of nearby boids, within a flock
Position matchVelocity(__global Boid* boids, int flockIndex, int numBoidsPerFlock, int boidIndex)
{
	Position velocity;
	

	for (int i = flockIndex * numBoidsPerFlock; i < (1 + flockIndex) * numBoidsPerFlock; i++)
	{
		if (i != boidIndex)
		{
			velocity.X += (boids + boidIndex)->Velocity.X;
			velocity.Y += (boids + boidIndex)->Velocity.Y;
		}

	}

	velocity.X /= (numBoidsPerFlock - 1);
	velocity.Y /= (numBoidsPerFlock - 1);

	velocity.X = (velocity.X - (boids + boidIndex)->Velocity.X) / 8;
	velocity.Y = (velocity.Y - (boids + boidIndex)->Velocity.Y) / 8;

	return velocity;
}

//Limiting the speed of the boids
Position limitVelocity(__global Boid* m)
{
	Position limitedVelocity;
	float vLimit = 0.00005;
	float norm = sqrt(m->Velocity.X*m->Velocity.X + m->Velocity.Y*m->Velocity.Y);

	if (norm > vLimit)
	{
		limitedVelocity.X = m->Velocity.X / norm * vLimit;
		limitedVelocity.Y = m->Velocity.Y / norm * vLimit;
	}
	else
	{
		limitedVelocity.X = m->Velocity.X;
		limitedVelocity.Y = m->Velocity.Y;
	}

	return limitedVelocity;
}


__kernel void updatePosition(__global Boid* boids, int numBoidsPerFlock, int numberOfFlocks)
{
	for(int flockIndex = 0; flockIndex < numberOfFlocks; flockIndex++)
	{
		for(int boidIndex = 0; boidIndex < numBoidsPerFlock; boidIndex++){

			Position total, distance, velocity;

			total = centerOfMass(boids, flockIndex, numBoidsPerFlock, (flockIndex * numBoidsPerFlock + boidIndex));
			distance = distanceAway(boids, flockIndex, numBoidsPerFlock, (flockIndex * numBoidsPerFlock + boidIndex));
			velocity = matchVelocity(boids, flockIndex, numBoidsPerFlock, (flockIndex * numBoidsPerFlock + boidIndex));

			(boids + (flockIndex * numBoidsPerFlock + boidIndex))->Velocity.X += total.X + distance.X + velocity.X;
			(boids + (flockIndex * numBoidsPerFlock + boidIndex))->Velocity.Y += total.Y + distance.Y + velocity.Y;

			limitPosition(boids, flockIndex, numBoidsPerFlock, (flockIndex * numBoidsPerFlock + boidIndex));

			(boids + (flockIndex * numBoidsPerFlock + boidIndex))->Velocity.X = limitVelocity(boids + (flockIndex * numBoidsPerFlock + boidIndex)).X;
			(boids + (flockIndex * numBoidsPerFlock + boidIndex))->Velocity.Y = limitVelocity(boids + (flockIndex * numBoidsPerFlock + boidIndex)).Y;
		
			(boids + (flockIndex * numBoidsPerFlock + boidIndex))->Coordinate.X += (boids + (flockIndex * numBoidsPerFlock + boidIndex))->Velocity.X;
			(boids + (flockIndex * numBoidsPerFlock + boidIndex))->Coordinate.Y += (boids + (flockIndex * numBoidsPerFlock + boidIndex))->Velocity.Y;
		}
	}
}
)";
}