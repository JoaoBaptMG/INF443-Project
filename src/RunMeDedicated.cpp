#if _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C"
{
	// Make NVIDIA and AMD run us on dedicated card
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif