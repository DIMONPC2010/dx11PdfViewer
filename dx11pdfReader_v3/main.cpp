#include "Control.h"
#include "InputListener.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	/*BookmarksIO *marks = new BookmarksIO("bookmarks.bin");
	marks->ReadBookmarksFromFile();
	marks->AddBookmark("somepdffile.pdf", 9);
	marks->WriteBookmarksToFile();*/


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