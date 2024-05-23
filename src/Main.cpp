#include "Lumiere.h"

using namespace Lumiere::Core;
using namespace Lumiere::Utils;

int main()
{
	std::unique_ptr<Window> win = std::make_unique<Window>();

	if (!win->Init("Lumiere", 640, 480))
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Failed to initialize a window\n");
		return -1;
	}

	win->Loop();
	win->Cleanup();

	return 0;
}
