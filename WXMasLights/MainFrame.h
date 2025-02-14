#pragma once
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <vector>
#include "TimelineBox.h"
#include "Constants.h"

class MainPanel : public wxScrolledCanvas {
public:
    MainPanel(wxWindow* parent);

private:
    std::vector<TimelineBox> boxes;
    TimelineBox* selectedBox = nullptr;
    bool isResizingLeft = false;
    bool isResizingRight = false;
    wxPoint dragStart;

    void OnLeftDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    double SnapToGrid(double time) const;
};

// MainFrame.h
#pragma once
#include <wx/wx.h>
#include <wx/statline.h>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
private:
    MainPanel* mainPanel;
};