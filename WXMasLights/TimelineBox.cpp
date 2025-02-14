#include "TimelineBox.h"

TimelineBox::TimelineBox(int channel, double startTime, double duration)
    : channel(channel), startTime(startTime), duration(duration) {
}

void TimelineBox::Draw(wxDC& dc) {
    wxRect bounds = GetBounds();

    dc.SetBrush(wxBrush(wxColour(100, 150, 255, 180)));
    dc.SetPen(wxPen(wxColour(50, 100, 200), 1));
    dc.DrawRectangle(bounds);

    dc.SetBrush(wxBrush(wxColour(50, 100, 200)));
    dc.DrawRectangle(bounds.GetLeft(), bounds.GetTop(), RESIZE_HANDLE_WIDTH, bounds.GetHeight());
    dc.DrawRectangle(bounds.GetRight() - RESIZE_HANDLE_WIDTH, bounds.GetTop(), RESIZE_HANDLE_WIDTH, bounds.GetHeight());
}

wxRect TimelineBox::GetBounds() const {
    const int pixelsPerSecond = 100;  // Must match the grid scale
    int x = static_cast<int>(startTime * pixelsPerSecond);
    int y = channel * CHANNEL_HEIGHT;
    int width = static_cast<int>(duration * pixelsPerSecond);

    // Ensure minimum width
    width = std::max(width, RESIZE_HANDLE_WIDTH * 2 + 1);

    return wxRect(x, y, width, CHANNEL_HEIGHT);
}

bool TimelineBox::Contains(const wxPoint& point) const {
    wxRect bounds = GetBounds();
    wxRect innerBounds = bounds;
    innerBounds.Deflate(RESIZE_HANDLE_WIDTH, 0);

    // Check if point is in the main body (excluding resize handles)
    if (innerBounds.Contains(point)) return true;

    // Check if point is in either resize handle
    if (IsResizingLeft(point) || IsResizingRight(point)) return true;

    return false;
}

bool TimelineBox::IsResizingLeft(const wxPoint& point) const {
    wxRect bounds = GetBounds();
    wxRect leftHandle(bounds.GetLeft(), bounds.GetTop(), RESIZE_HANDLE_WIDTH, bounds.GetHeight());
    return leftHandle.Contains(point);
}

bool TimelineBox::IsResizingRight(const wxPoint& point) const {
    wxRect bounds = GetBounds();
    wxRect rightHandle(bounds.GetRight() - RESIZE_HANDLE_WIDTH, bounds.GetTop(), RESIZE_HANDLE_WIDTH, bounds.GetHeight());
    return rightHandle.Contains(point);
}

void TimelineBox::Move(double newStartTime, int newChannel) {
    startTime = newStartTime;
    channel = newChannel;
}

void TimelineBox::Resize(double newStartTime, double newDuration) {
    startTime = newStartTime;
    duration = newDuration;
}
