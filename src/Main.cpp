#include "Lumiere.h"

using namespace Lumiere::Utils;

int main()
{
	Logger::Log(LogLevel::Error, "An error occured in ", __FUNCTION__, " function");
	std::cin.get();
}
