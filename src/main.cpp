

#include <Windows.h>
#include <./Device/WindowDevice.h>

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,
					 int nCmdShow) {
	
	WindowDevice &window = WindowDevice::getInstance();
	
	window.Init(hInstance, nCmdShow, TEXT("test window"), 1280, 720, true);

	int ret = window.MessageLoop([]() { });

	return ret;
}