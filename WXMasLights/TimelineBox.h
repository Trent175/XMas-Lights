#pragma once
#include <wx/wx.h>
#include "Constants.h"

class TimelineBox {
public:
    TimelineBox(int channel, double startTime, double duration = 3.0);
    void Draw(wxDC& dc);
    bool Contains(const wxPoint& point) const;
    bool IsResizingLeft(const wxPoint& point) const;
    bool IsResizingRight(const wxPoint& point) const;
    void Move(double newStartTime, int newChannel);
    void Resize(double newStartTime, double newDuration);

    int GetChannel() const { return channel; }
    double GetStartTime() const { return startTime; }
    double GetDuration() const { return duration; }

private:
    int channel;
    double startTime;
    double duration;
    static constexpr int RESIZE_HANDLE_WIDTH = 5;
    wxRect GetBounds() const;
};