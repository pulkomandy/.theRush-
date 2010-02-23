#include "BoundingSphere.h"
#include "Point.h"
#include "Timer.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

float rand(float min, float max)
{
    return min + (max - min) * rand() / (float)RAND_MAX;
}

int main()
{
	using namespace MiniBall;

	const float epsilon = radiusEpsilon + 1e-5f;   // Used for testing correctness of algorithms
	BoundingSphere BS;
	unsigned int p = 0;

	do
	{
		printf("Enter number of points: ");

		if(!scanf("%d", &p))
		{
			printf("Invalid input.\n\n");
			
			break;
		}

		printf("\nAllocating memory.\n");

		Point *P = new Point[p];

		if(!P)
		{
			printf("Memory could not be allocated.\n\n");
			
			break;
		}

		printf("\nConstructing random points: ");

		srand((unsigned)time(0));

		for(unsigned int i = 0; i < 10; i++)
		{
			for(unsigned int j = i * p / 10; j < (i + 1) * p / 10; j++)
			{
				P[j].x = rand(-1, 1);
				P[j].y = rand(-1, 1);
				P[j].z = rand(-1, 1);
			}

			printf(".");
		}

		printf("\n\nBounding sphere approximation:\n\n");

		double time0 = Timer::seconds();

		BS = Sphere::smallBall(P, p);

		double time1 = Timer::seconds();

		printf("   Construction time: %f\n", time1 - time0);
		printf("   Center = (%f, %f, %f)\n", BS.center.x, BS.center.y, BS.center.z);
		printf("   Radius = %f\n\n", BS.radius);

		for(i = 0; i < p; i++)
		{
			if(Sphere::d(BS, P[i]) >= -epsilon)
			{
				printf("   Boundary point distance check: %.3f\n", Sphere::d(BS, P[i]));
			}
		}

		printf("\nSmallest enclosing sphere:\n\n");

		time0 = Timer::seconds();

		BS = Sphere::miniBall(P, p);

		time1 = Timer::seconds();

		printf("   Construction time: %f\n", time1 - time0);
		printf("   Center = (%f, %f, %f)\n", BS.center.x, BS.center.y, BS.center.z);
		printf("   Radius = %f\n\n", BS.radius);

		for(i = 0; i < p; i++)
		{
			if(Sphere::d(BS, P[i]) >= -epsilon)
			{
				printf("   Boundary point distance check: %.3f\n", Sphere::d(BS, P[i]));
			}
		}

		printf("\n");

		if(P)
		{
			delete[] P;
			P = 0;
		}
	}
	while(true);

	return 0;
}
