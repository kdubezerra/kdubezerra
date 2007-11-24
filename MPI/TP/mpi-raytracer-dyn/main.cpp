#include "RayTracer.h"
#include "Sphere.h"
#include "Triangle.h"

#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define EXECUTION_REPETITIONS 1
#define BLOCK_SIZE 1

int main( int argc, char ** argv )
{
	
	MPI_Init(&argc, &argv);

	int rank, nprocs;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	RayTracer* p_ray_tracer = new RayTracer(rank, nprocs, BLOCK_SIZE);

	// BEGIN HANDLE COMMAND LINE ARGS
	if ( argc == 1 )
		p_ray_tracer->OutputFilename("default.ppm");
	if ( argc >= 2 )
	{
		p_ray_tracer->OutputFilename(argv[1]);
		p_ray_tracer->EyePoint( Point(0,0,0) );
	}
	if (argc >= 5)
		p_ray_tracer->EyePoint( Point(strtod(argv[2],NULL), strtod(argv[3],NULL), strtod(argv[4],NULL)) );
	if (argc >= 9) // specified eye point and light sources
	{
		Light* dynamic_light;
		// might want to check that (argc - 1) is a multiple of 3
		for(int i=5; i < argc; i+=4)
		{
			// make light sources dynamically
			dynamic_light = new Light(Point(strtod(argv[i],NULL),strtod(argv[i+1],NULL),strtod(argv[i+2],NULL)));
			dynamic_light->Intensity(strtod(argv[i+3],NULL));
			p_ray_tracer->AddLight(dynamic_light);
		}
	}
	// END HANDLE COMMAND LINE ARGS

	
	Light* light = NULL;
	Sphere* sphere = NULL;
	Triangle* triangle = NULL;


	//////////////////////////////////////////////////////
	// begin setup model
	p_ray_tracer->BackgroundColor(Color(0,0,0));

	light = new Light(Point(0,0,0));
	light->Intensity(0.5);
	p_ray_tracer->AddLight(light);


	sphere = new Sphere(Point(-3,0,10),2);
	sphere->ShapeColor(Color(1,1,0.2));
	p_ray_tracer->AddShape(sphere);

	sphere = new Sphere(Point(2.5,0,15),2);
	sphere->ShapeColor(Color(1,0.5,0.2));
	p_ray_tracer->AddShape(sphere);

	triangle = new Triangle(Point(-20,6,20),  Point(20,6,20), Point(0,10,10));	
	triangle->ShapeColor(Color(1,0,0));
	triangle->DiffuseReflectivity(0.9);
	p_ray_tracer->AddShape(triangle);

	triangle = new Triangle(Point(-20,-6,20),  Point(20,-6,20), Point(0,-10,10));	
	triangle->ShapeColor(Color(0,1,0));
	triangle->DiffuseReflectivity(0.9);
	p_ray_tracer->AddShape(triangle);

	// end setup model
	//////////////////////////////////////////////////////

	/**if (rank == 0)**/ {
	
		/*** Variable for time measurement ***/
		struct timeval start[EXECUTION_REPETITIONS], stop[EXECUTION_REPETITIONS];
		long execution_time_usec[EXECUTION_REPETITIONS];
		double average_time = 0.0f;
	
//		cout << "Process " << rank << " executing RayTrace..." /*<< EXECUTION_REPETITIONS << " time(s).\nThe number of times to run this program can be set\nby changing a #define in \"main.cpp\"." << endl*/ << endl;
		for (int repetition = 0; repetition < EXECUTION_REPETITIONS; repetition++) {
		
			/*** Get initial time ***/
			gettimeofday(&start[repetition], NULL);
	
			/*** Execute the calculation ***/
			p_ray_tracer->RayTrace();
	
	
			/*** Get final time ***/
			gettimeofday(&stop[repetition], NULL);
	
			/*** Print out the execution time ***/
			execution_time_usec[repetition] =
					(stop[repetition].tv_sec - start[repetition].tv_sec)*1e6 +
					(stop[repetition].tv_usec - start[repetition].tv_usec);
	
			if (rank == 0)
				cout /* << "Process " << rank << " finished " << repetition + 1 << "in (usec): "*/ << execution_time_usec[repetition] << endl;		
			average_time += (double) execution_time_usec[repetition];
	
		}
	
		average_time /= (double) EXECUTION_REPETITIONS;	
	
		cout.precision(2);
// 		cout << "\nAverage execution time in " << EXECUTION_REPETITIONS << " execution(s): " << fixed << average_time << endl;
	
	}

// 	cout << "Processo " << rank << " poderia terminar já..." << endl;

	delete p_ray_tracer;

// 	cout << "Processo " << rank << " deletou o objeto p_ray_tracer..." << endl;

	MPI_Finalize();

// 	cout << "Processo " << rank << " terminou..." << endl;

	return 0;
}
