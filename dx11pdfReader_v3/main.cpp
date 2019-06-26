#include "Control.h"
#include "InputListener.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Control control;
	std::shared_ptr<Render> render(new Render());

	control.SetRender(render);
	control.Init();
	control.SetPages();
	control.AddInputListener(render.get());
	

	control.Run();
	control.Close();

	return 0;
}