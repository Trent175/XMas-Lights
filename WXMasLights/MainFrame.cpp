#include "MainFrame.h"
#include <wx/wx.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	wxPanel* mainPanel = new wxPanel(this);
	
	wxScrollBar* horizScroll = new wxScrollBar(this, wxID_ANY, wxPoint(0, 580), wxSize(800, 20), wxTB_HORIZONTAL);
	horizScroll->SetScrollbar(0, 1, 100, 100);
}