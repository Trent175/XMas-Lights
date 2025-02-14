#include "MainFrame.h"

// MainPanel implementation
MainPanel::MainPanel(wxWindow* parent) : wxScrolledCanvas(parent) {
    SetBackgroundColour(wxColour(255, 255, 255));
    SetScrollRate(SCROLL_RATE, SCROLL_RATE);

    Bind(wxEVT_LEFT_DOWN, &MainPanel::OnLeftDown, this);
    Bind(wxEVT_MOTION, &MainPanel::OnMouseMove, this);
    Bind(wxEVT_LEFT_UP, &MainPanel::OnLeftUp, this);
    Bind(wxEVT_PAINT, &MainPanel::OnPaint, this);
}

double MainPanel::SnapToGrid(double time) const {
    const double gridSize = 0.125;  // 1/8th second
    return std::round(time / gridSize) * gridSize;
}

void MainPanel::OnLeftDown(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);

    selectedBox = nullptr;
    for (auto& box : boxes) {
        if (box.Contains(pos)) {
            selectedBox = &box;
            isResizingLeft = box.IsResizingLeft(pos);
            isResizingRight = box.IsResizingRight(pos);
            dragStart = pos;
            break;
        }
    }

    if (!selectedBox) {
        double time = SnapToGrid(pos.x / 100.0);  // Convert pixels to seconds
        int channel = pos.y / CHANNEL_HEIGHT;
        if (channel >= 0 && channel < CHANNELS) {
            boxes.emplace_back(channel, time, 3.0);
            selectedBox = &boxes.back();  // Select the newly created box
            dragStart = pos;
            Refresh();
        }
    }

    event.Skip();
}

void MainPanel::OnMouseMove(wxMouseEvent& event) {
    if (!selectedBox || !event.LeftIsDown()) return;

    wxPoint pos = event.GetPosition();
    CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);

    // Calculate time difference in seconds
    double deltaTime = (pos.x - dragStart.x) / 100.0;  // 100 pixels per second

    if (isResizingLeft) {
        double newStart = SnapToGrid(selectedBox->GetStartTime() + deltaTime);
        double newDuration = selectedBox->GetDuration() - (newStart - selectedBox->GetStartTime());

        // Prevent negative duration
        if (newDuration >= 0.125) {  // Minimum duration of 1/8th second
            selectedBox->Resize(newStart, newDuration);
        }
    }
    else if (isResizingRight) {
        double newDuration = SnapToGrid(selectedBox->GetDuration() + deltaTime);

        // Prevent negative duration
        if (newDuration >= 0.125) {  // Minimum duration of 1/8th second
            selectedBox->Resize(selectedBox->GetStartTime(), newDuration);
        }
    }
    else {
        // Moving the entire box
        double newTime = SnapToGrid(selectedBox->GetStartTime() + deltaTime);
        int newChannel = std::max(0, std::min(CHANNELS - 1, pos.y / CHANNEL_HEIGHT));

        // Prevent negative start time
        if (newTime >= 0) {
            selectedBox->Move(newTime, newChannel);
        }
    }

    dragStart = pos;  // Update drag start position for next movement
    Refresh();
    event.Skip();
}

void MainPanel::OnLeftUp(wxMouseEvent& event) {
    selectedBox = nullptr;
    isResizingLeft = false;
    isResizingRight = false;
    event.Skip();
}

void MainPanel::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    DoPrepareDC(dc);

    // Draw grid lines
    dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
    for (int i = 0; i <= CHANNELS; ++i) {
        dc.DrawLine(0, i * CHANNEL_HEIGHT, 1500, i * CHANNEL_HEIGHT);
    }
    for (int i = 0; i < 150; i += 10) {
        dc.DrawLine(i * 10, 0, i * 10, CHANNELS * CHANNEL_HEIGHT);
    }

    // Draw boxes
    for (auto& box : boxes) {
        box.Draw(dc);
    }
}

// MainFrame implementation
MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title,
    wxDefaultPosition, wxSize(800, 600))
{
    wxPanel* containerPanel = new wxPanel(this);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* contentSizer = new wxBoxSizer(wxHORIZONTAL);

    mainPanel = new MainPanel(containerPanel);
    wxScrolledCanvas* leftPanel = new wxScrolledCanvas(containerPanel);
    wxScrolledCanvas* topPanel = new wxScrolledCanvas(containerPanel);

    int virtualWidth = 1500;
    int virtualHeight = CHANNELS * CHANNEL_HEIGHT;

    mainPanel->SetMinSize(wxSize(500, 400));
    mainPanel->SetVirtualSize(virtualWidth, virtualHeight);

    topPanel->SetMinSize(wxSize(500 + SCROLLBAR_WIDTH, TOP_PANEL_HEIGHT));
    topPanel->SetVirtualSize(virtualWidth + SCROLLBAR_WIDTH, TOP_PANEL_HEIGHT);

    leftPanel->SetMinSize(wxSize(LEFT_PANEL_WIDTH, 400 + SCROLL_RATE));
    leftPanel->SetVirtualSize(LEFT_PANEL_WIDTH, virtualHeight + SCROLL_RATE);

    leftPanel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);
    topPanel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

    // Set up scroll synchronization
    mainPanel->Bind(wxEVT_SCROLLWIN_THUMBTRACK, [=](wxScrollWinEvent& event) {
        if (event.GetOrientation() == wxHORIZONTAL) {
            topPanel->Scroll(event.GetPosition(), topPanel->GetViewStart().y);
        }
        else {
            leftPanel->Scroll(leftPanel->GetViewStart().x, event.GetPosition());
        }
        event.Skip();
        });

    mainPanel->Bind(wxEVT_SCROLLWIN_THUMBRELEASE, [=](wxScrollWinEvent& event) {
        if (event.GetOrientation() == wxHORIZONTAL) {
            topPanel->Scroll(event.GetPosition(), topPanel->GetViewStart().y);
        }
        else {
            leftPanel->Scroll(leftPanel->GetViewStart().x, event.GetPosition());
        }
        event.Skip();
        });

    // Set up other panels' paint events
    leftPanel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
        wxPaintDC dc(static_cast<wxWindow*>(evt.GetEventObject()));
        static_cast<wxScrolledCanvas*>(evt.GetEventObject())->DoPrepareDC(dc);

        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        for (int i = 0; i < CHANNELS; ++i) {
            wxString text = wxString::Format("Channel %d", i + 1);
            dc.DrawText(text, 5, i * CHANNEL_HEIGHT + 15);
        }
        });

    topPanel->Bind(wxEVT_PAINT, [](wxPaintEvent& evt) {
        wxPaintDC dc(static_cast<wxWindow*>(evt.GetEventObject()));
        static_cast<wxScrolledCanvas*>(evt.GetEventObject())->DoPrepareDC(dc);

        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        for (int i = 0; i < 150; ++i) {
            wxString text = wxString::Format("%d", i);
            dc.DrawText(text, i * 100, 40);
        }
        });

    // Layout
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(LEFT_PANEL_WIDTH, TOP_PANEL_HEIGHT, 0);
    topSizer->Add(topPanel, 1, wxEXPAND);
    topSizer->Add(SCROLLBAR_WIDTH, 0, 0);

    contentSizer->Add(leftPanel, 0, wxEXPAND | wxBOTTOM, SCROLLBAR_WIDTH);
    contentSizer->Add(mainPanel, 1, wxEXPAND);

    mainSizer->Add(topSizer, 0, wxEXPAND);
    mainSizer->Add(contentSizer, 1, wxEXPAND);

    containerPanel->SetSizer(mainSizer);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(containerPanel, 1, wxEXPAND);
    SetSizer(frameSizer);
}