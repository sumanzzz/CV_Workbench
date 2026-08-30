#include "Application.h"
#include "ImageLoader.h"

#include <filesystem>
#include <iostream>

int main(int argc , char* argv[])
{
	Application app(argc, argv);

	return app.run();
	
}