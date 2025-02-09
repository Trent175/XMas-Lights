#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/scrolwin.h>

const int CHANNELS = 16;
const int CHANNEL_HEIGHT = 50;
const int LEFT_PANEL_WIDTH = 150;
const int TOP_PANEL_HEIGHT = 100;
const int SCROLL_RATE = 10;
const int SCROLLBAR_WIDTH = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

class SyncedScrolledCanvas : public wxScrolledCanvas {
public:
    SyncedScrolledCanvas(wxWindow* parent, wxWindowID id = wxID_ANY)
        : wxScrolledCanvas(parent, id) {
        SetBackgroundColour(wxColour(255, 255, 255));
        SetScrollRate(SCROLL_RATE, SCROLL_RATE);
    }
};

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, 
    wxDefaultPosition, wxSize(800, 600)) 
{
    // Create main panel to hold all components
    wxPanel* containerPanel = new wxPanel(this);
    
    // Create sizers for layout
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* contentSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Create the scrolled panels
    SyncedScrolledCanvas* mainPanel = new SyncedScrolledCanvas(containerPanel);
    SyncedScrolledCanvas* leftPanel = new SyncedScrolledCanvas(containerPanel);
    SyncedScrolledCanvas* topPanel = new SyncedScrolledCanvas(containerPanel);
    
    // Create virtual size for scrollable content
    int virtualWidth = 1500;  // Adjust as needed
    int virtualHeight = CHANNELS * CHANNEL_HEIGHT;

    // Set minimum sizes for visible content
    mainPanel->SetMinSize(wxSize(500, 400));
    mainPanel->SetVirtualSize(virtualWidth, virtualHeight);
    
    topPanel->SetMinSize(wxSize(500 + SCROLLBAR_WIDTH, TOP_PANEL_HEIGHT));
    topPanel->SetVirtualSize(virtualWidth + SCROLLBAR_WIDTH, TOP_PANEL_HEIGHT);

    // Adjust left panel height to account for horizontal scrollbar in main panel
    leftPanel->SetMinSize(wxSize(LEFT_PANEL_WIDTH, 400 + SCROLL_RATE));
    leftPanel->SetVirtualSize(LEFT_PANEL_WIDTH, virtualHeight + SCROLL_RATE);
    
    // Disable scrollbars as needed for each panel
    leftPanel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);  // Left panel - no scrollbars
    topPanel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);   // Top panel - no scrollbars
    
    // Bind scroll events
    mainPanel->Bind(wxEVT_SCROLLWIN_THUMBTRACK, [=](wxScrollWinEvent& event) {
        if (event.GetOrientation() == wxHORIZONTAL) {
            topPanel->Scroll(event.GetPosition(), topPanel->GetViewStart().y);
        } else {
            leftPanel->Scroll(leftPanel->GetViewStart().x, event.GetPosition());
        }
        event.Skip();
    });
    
    mainPanel->Bind(wxEVT_SCROLLWIN_THUMBRELEASE, [=](wxScrollWinEvent& event) {
        if (event.GetOrientation() == wxHORIZONTAL) {
            topPanel->Scroll(event.GetPosition(), topPanel->GetViewStart().y);
        } else {
            leftPanel->Scroll(leftPanel->GetViewStart().x, event.GetPosition());
        }
        event.Skip();
    });
    
    // Add some example content
    mainPanel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
        wxPaintDC dc(static_cast<wxWindow*>(evt.GetEventObject()));
        static_cast<wxScrolledCanvas*>(evt.GetEventObject())->DoPrepareDC(dc);
        
        // Draw grid lines
        dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
        for (int i = 0; i <= CHANNELS; ++i) {
            dc.DrawLine(0, i * CHANNEL_HEIGHT, 1500, i * CHANNEL_HEIGHT);
        }
        for (int i = 0; i < 150; i += 10) {
            dc.DrawLine(i * 10, 0, i * 10, CHANNELS * CHANNEL_HEIGHT);
        }
    });
    
    leftPanel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
        wxPaintDC dc(static_cast<wxWindow*>(evt.GetEventObject()));
        static_cast<wxScrolledCanvas*>(evt.GetEventObject())->DoPrepareDC(dc);
        
        // Draw channel numbers
        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        for (int i = 0; i < CHANNELS; ++i) {
            wxString text = wxString::Format("Channel %d", i + 1);
            dc.DrawText(text, 5, i * CHANNEL_HEIGHT + 15);
        }
    });
    
    topPanel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
        wxPaintDC dc(static_cast<wxWindow*>(evt.GetEventObject()));
        static_cast<wxScrolledCanvas*>(evt.GetEventObject())->DoPrepareDC(dc);
        
        // Draw time markers
        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        for (int i = 0; i < 150; ++i) {
            wxString text = wxString::Format("%d", i);
            dc.DrawText(text, i * 100, 40);
        }
    });
    
    // Layout the panels
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(LEFT_PANEL_WIDTH, TOP_PANEL_HEIGHT, 0);  // Spacer on the left
    topSizer->Add(topPanel, 1, wxEXPAND);
    topSizer->Add(SCROLLBAR_WIDTH, 0, 0);  // Add right spacer to match scrollbar
    
    contentSizer->Add(leftPanel, 0, wxEXPAND | wxBOTTOM, SCROLLBAR_WIDTH);
    contentSizer->Add(mainPanel, 1, wxEXPAND);
    
    mainSizer->Add(topSizer, 0, wxEXPAND);
    mainSizer->Add(contentSizer, 1, wxEXPAND);
    
    containerPanel->SetSizer(mainSizer);
    
    // Set up the main frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(containerPanel, 1, wxEXPAND);
    SetSizer(frameSizer);
}