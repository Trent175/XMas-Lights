#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include <Windows.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* mainFrame = new MainFrame("CPP GUI");
	mainFrame->SetClientSize(800, 600);
	mainFrame->Center();
	mainFrame->Show();

	return true;
}