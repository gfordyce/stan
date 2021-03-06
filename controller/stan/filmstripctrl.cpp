/**
 * @file filmstripctrl.cpp - Displays a horizontally scrolling filmstrip
 * @author Gerald Fordyce
 * @acknowlegements Julian Smart
 */

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "filmstripctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx_render.h"

#if wxCHECK_VERSION(2,5,5)
#define USE_BUFFERED_PAINT 1
#else
#define USE_BUFFERED_PAINT 0
#endif

// #include "utils.h"
#include "wx/settings.h"
#include "wx/arrimpl.cpp"
#include "wx/dcbuffer.h"

#include "filmstripctrl.h"

WX_DEFINE_OBJARRAY(wxFilmstripItemArray);

DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_ITEM_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_ITEM_DESELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_LEFT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_MIDDLE_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_LEFT_DCLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_RETURN)

IMPLEMENT_CLASS( wxFilmstripCtrl, wxScrolledWindow )
IMPLEMENT_CLASS( wxFilmstripItem, wxObject )
IMPLEMENT_CLASS( wxFilmstripEvent, wxNotifyEvent )

BEGIN_EVENT_TABLE( wxFilmstripCtrl, wxScrolledWindow )
    EVT_PAINT(wxFilmstripCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxFilmstripCtrl::OnEraseBackground)
    EVT_LEFT_DOWN(wxFilmstripCtrl::OnLeftClick)
    EVT_RIGHT_DOWN(wxFilmstripCtrl::OnRightClick)
    EVT_MIDDLE_DOWN(wxFilmstripCtrl::OnMiddleClick)
    EVT_LEFT_DCLICK(wxFilmstripCtrl::OnLeftDClick)
    EVT_CHAR(wxFilmstripCtrl::OnChar)
    EVT_SIZE(wxFilmstripCtrl::OnSize)
    EVT_SET_FOCUS(wxFilmstripCtrl::OnSetFocus)
    EVT_KILL_FOCUS(wxFilmstripCtrl::OnKillFocus)

    EVT_MENU(wxID_SELECTALL, wxFilmstripCtrl::OnSelectAll)
    EVT_UPDATE_UI(wxID_SELECTALL, wxFilmstripCtrl::OnUpdateSelectAll)
END_EVENT_TABLE()

/*!
 * wxFilmstripCtrl
 */

wxFilmstripCtrl::wxFilmstripCtrl( )
{
    Init();
}

wxFilmstripCtrl::wxFilmstripCtrl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}

/// Creation
bool wxFilmstripCtrl::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if (!wxScrolledWindow::Create(parent, id, pos, size, style
#if wxCHECK_VERSION(2,5,1)
        |wxFULL_REPAINT_ON_RESIZE
#endif
        ))
        return false;

    CalculateOverallFilmstripSize();

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    
#if wxCHECK_VERSION(2,5,1)
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
#endif

    // Tell the sizers to use the given or best size    
    // SetBestFittingSize(size);
    SetInitialSize(size);
    
#if USE_BUFFERED_PAINT
    // Create a buffer
    RecreateBuffer(size);
#endif
    
    return true;
}

/// Member initialisation
void wxFilmstripCtrl::Init()
{
    m_filmstripOverallSize = wxFILMSTRIP_DEFAULT_OVERALL_SIZE;
    m_freezeCount = 0;
    m_spacing = wxFILMSTRIP_DEFAULT_SPACING;
    m_filmstripMargin = wxFILMSTRIP_DEFAULT_MARGIN;
    m_firstSelection = -1;
    m_lastSelection = -1;
    m_focussedFilmstripBackgroundColour = wxFILMSTRIP_DEFAULT_FOCUSSED_BACKGROUND;
    m_unfocussedFilmstripBackgroundColour  = wxFILMSTRIP_DEFAULT_UNFOCUSSED_BACKGROUND;
    m_unselectedFilmstripBackgroundColour = wxFILMSTRIP_DEFAULT_UNSELECTED_BACKGROUND;
    m_tagColour = wxFILMSTRIP_DEFAULT_TAG_COLOUR;
    m_focusRectColour = wxFILMSTRIP_DEFAULT_FOCUS_RECT_COLOUR;
    m_focusItem = -1;
}

/// Call Freeze to prevent refresh
void wxFilmstripCtrl::Freeze()
{
    m_freezeCount ++;
}

/// Call Thaw to refresh
void wxFilmstripCtrl::Thaw()
{
    m_freezeCount --;

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh();
    }
}

/// Append a single item
int wxFilmstripCtrl::Append(wxFilmstripItem* item)
{
    int sz = (int) GetCount();
    m_items.Add(item);
    m_firstSelection = -1;
    m_lastSelection = -1;
    m_focusItem = -1;

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh();
    }
    return sz;
}

/// Insert a single item
int wxFilmstripCtrl::Insert(wxFilmstripItem* item, int pos)
{
    m_items.Insert(item, pos);
    m_firstSelection = -1;
    m_lastSelection = -1;
    m_focusItem = -1;

    // Must now change selection indices because
    // items above it have moved up
    size_t i;
    for (i = 0; i < m_selections.GetCount(); i++)
    {
        if (m_selections[i] >= pos)
            m_selections[i] = m_selections[i] + 1;
    }
    // Ditto for tags
    for (i = 0; i < m_tags.GetCount(); i++)
    {
        if (m_tags[i] >= pos)
            m_tags[i] = m_tags[i] + 1;
    }

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh();
    }
    return pos;
}

/// Clear all items
void wxFilmstripCtrl::Clear()
{
    m_firstSelection = -1;
    m_lastSelection = -1;
    m_focusItem = -1;
    m_items.Clear();
    m_selections.Clear();
    m_tags.Clear();

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh();
    }
}

/// Delete this item
void wxFilmstripCtrl::Delete(int n)
{
    if (m_firstSelection == n)
        m_firstSelection = -1;
    if (m_lastSelection == n)
        m_lastSelection = -1;
    if (m_focusItem == n)
        m_focusItem = -1;

    if (m_selections.Index(n) != wxNOT_FOUND)
        m_selections.Remove(n);

    if (m_tags.Index(n) != wxNOT_FOUND)
        m_tags.Remove(n);

    m_items.RemoveAt(n);

    // Must now change selection indices because
    // items have moved down
    size_t i;
    for (i = 0; i < m_selections.GetCount(); i++)
    {
        if (m_selections[i] > n)
            m_selections[i] = m_selections[i] - 1;
    }

    if (m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh();
    }
}

/// Get the nth item
wxFilmstripItem* wxFilmstripCtrl::GetItem(int n)
{
    wxASSERT(n < GetCount());

    if (n < GetCount())
    {
        return & m_items[(size_t) n];
    }
    else
        return NULL;
}

/// Get the overall rect of the given item
bool wxFilmstripCtrl::GetItemRect(int n, wxRect& rect, bool transform)
{
    wxASSERT(n < GetCount());
    if (n < GetCount())
    {
        int row = 0, col;
        if (!GetCol(n, GetClientSize(), col))
            return false;

        int x = col * (m_filmstripOverallSize.x + m_spacing) + m_spacing;
        int y = row * (m_filmstripOverallSize.y + m_spacing) + m_spacing;

        if (transform)
        {
            int startX, startY;
            int xppu, yppu;
            GetScrollPixelsPerUnit(& xppu, & yppu);
            GetViewStart(& startX, & startY);
            x = x - startX*xppu;
            y = y - startY*yppu;
        }
        
        rect.x = x;
        rect.y = y;
        rect.width = m_filmstripOverallSize.x;
        rect.height = m_filmstripOverallSize.y;
        
        return true;
    }

    return false;
}

/// The size of the image part
void wxFilmstripCtrl::SetFilmstripImageSize(const wxSize& sz)
{
    m_filmstripImageSize = sz;
    CalculateOverallFilmstripSize();

    if (GetCount() > 0 && m_freezeCount == 0)
    {
        SetupScrollbars();
        Refresh();
    }
}

/// Calculate the outer filmstrip size based
/// on font used for text and inner size
void wxFilmstripCtrl::CalculateOverallFilmstripSize()
{
    //  wxCoord w;
    wxClientDC dc(this);

	m_filmstripOverallSize.x = m_filmstripImageSize.x;
    m_filmstripOverallSize.y = m_filmstripImageSize.y;
}

/// Return the column given the client
/// size and a left-to-right layout assumption
bool wxFilmstripCtrl::GetCol(int item, const wxSize& clientSize, int& col)
{
    wxASSERT(item < GetCount());
    if (item >= GetCount())
        return false;

    col = item;
    return true;
}


/// Select or deselect an item
void wxFilmstripCtrl::Select(int n, bool select)
{
    wxASSERT (n < GetCount());

    if (select)
    {
        if (m_selections.Index(n) == wxNOT_FOUND)
            m_selections.Add(n);
    }
    else
    {
        if (m_selections.Index(n) != wxNOT_FOUND)
            m_selections.Remove(n);
    }

    m_firstSelection = n;
    m_lastSelection = n;
    int oldFocusItem = m_focusItem;
    m_focusItem = n;

    if (m_freezeCount == 0)
    {
        wxRect rect;
        GetItemRect(n, rect);
        RefreshRect(rect);

        if (oldFocusItem != -1 && oldFocusItem != n)
        {
            GetItemRect(oldFocusItem, rect);
            RefreshRect(rect);
        }
    }
}

/// Select or deselect a range
void wxFilmstripCtrl::SelectRange(int from, int to, bool select)
{
    int first = from;
    int last = to;
    if (first < last)
    {
        first = to;
        last = from;
    }
    wxASSERT (first >= 0 && first < GetCount());
    wxASSERT (last >= 0 && last < GetCount());

    Freeze();
    int i;
    for (i = first; i < last; i++)
    {
        Select(i, select);
    }
    m_focusItem = to;
    Thaw();
}

/// Select all
void wxFilmstripCtrl::SelectAll()
{
    Freeze();
    int i;
    for (i = 0; i < GetCount(); i++)
	{
        Select(i, true);
	}
    if (GetCount() > 0)
	{
        m_focusItem = GetCount()-1;
	}
    else
	{
        m_focusItem = -1;
	}
    Thaw();
}

/// Select none
void wxFilmstripCtrl::SelectNone()
{
    Freeze();
    int i;
    for (i = 0; i < GetCount(); i++)
	{
        Select(i, false);
	}
    Thaw();
}

/// Get the index of the single selection, if not multi-select.
/// Returns -1 if there is no selection.
int wxFilmstripCtrl::GetSelection() const
{
    if (m_selections.GetCount() > 0)
        return m_selections[0u];
    else
        return -1;
}

/// Returns true if the item is selected
bool wxFilmstripCtrl::IsSelected(int n) const
{
    return (m_selections.Index(n) != wxNOT_FOUND) ;
}

/// Clears all selections
void wxFilmstripCtrl::ClearSelections()
{
    int count = GetCount();

    m_selections.Clear();
    m_firstSelection = -1;
    m_lastSelection = -1;
    m_focusItem = -1;

    if (count > 0 && m_freezeCount == 0)
    {
        Refresh();
    }
}

/// Set the focus item
void wxFilmstripCtrl::SetFocusItem(int item)
{
    wxASSERT( item < GetCount() );
    if (item < GetCount())
    {
        int oldFocusItem = m_focusItem;
        m_focusItem = item;
        
        if (m_freezeCount == 0)
        {
            wxRect rect;
            if (oldFocusItem != -1)
            {
                GetItemRect(oldFocusItem, rect);
                RefreshRect(rect);
            }
            if (m_focusItem != -1)
            {
                GetItemRect(m_focusItem, rect);
                RefreshRect(rect);
            }
        }
    }
}

/// Tag or untag an item
void wxFilmstripCtrl::Tag(int n, bool tag)
{
    wxASSERT (n < GetCount());

    if (tag)
    {
        if (m_tags.Index(n) == wxNOT_FOUND)
            m_tags.Add(n);
    }
    else
    {
        if (m_tags.Index(n) != wxNOT_FOUND)
            m_tags.Remove(n);
    }

    if (m_freezeCount == 0)
    {
        wxRect rect;
        GetItemRect(n, rect);
        RefreshRect(rect);
    }
}

/// Returns true if the item is tagged
bool wxFilmstripCtrl::IsTagged(int n) const
{
    return (m_tags.Index(n) != wxNOT_FOUND) ;
}

/// Clears all tags
void wxFilmstripCtrl::ClearTags()
{
    int count = GetCount();

    m_tags.Clear();

    if (count > 0 && m_freezeCount == 0)
    {
        Refresh();
    }
}

/// Painting
void wxFilmstripCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // Set this to 0 to compare it with the
    // unbuffered implementation
#if USE_BUFFERED_PAINT
    wxBufferedPaintDC dc(this, m_bufferBitmap);
#else
    wxPaintDC dc(this);
#endif

    PrepareDC(dc);
 
    if (m_freezeCount > 0)
        return;

    // Paint the background
    PaintBackground(dc);

    if (GetCount() == 0)
        return;

    wxRegion dirtyRegion = GetUpdateRegion();
    bool isFocussed = (FindFocus() == this);

    int i;
    int count = GetCount();
    int style = 0;
    wxRect rect, untransformedRect;
    for (i = 0; i < count; i++)
    {
        GetItemRect(i, rect);

        wxRegionContain c = dirtyRegion.Contains(rect);
        if (c != wxOutRegion)
        {
            style = 0;
            if (IsSelected(i))
                style |= wxFILMSTRIP_SELECTED;
            if (IsTagged(i))
                style |= wxFILMSTRIP_TAGGED;
            if (isFocussed)
                style |= wxFILMSTRIP_FOCUSSED;
            //if (isFocussed && i == m_focusItem)
            //    style |= wxFILMSTRIP_IS_FOCUS;

            GetItemRect(i, untransformedRect, false);
            
            DrawItemBackground(i, dc, untransformedRect, style);
            DrawItem(i, dc, untransformedRect, style);
        }
    }
}

// Empty implementation, to prevent flicker
void wxFilmstripCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxFilmstripCtrl::OnSetFocus(wxFocusEvent& WXUNUSED(event))
{
    if (GetCount() > 0)
        Refresh();
}

void wxFilmstripCtrl::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    if (GetCount() > 0)
        Refresh();
}

/// Left-click
void wxFilmstripCtrl::OnLeftClick(wxMouseEvent& event)
{
    SetFocus();
    int n;
    if (HitTest(event.GetPosition(), n))
    {
        int flags = 0;
        if (event.ControlDown())
            flags |= wxFILMSTRIP_CTRL_DOWN;
        if (event.ShiftDown())
            flags |= wxFILMSTRIP_SHIFT_DOWN;
        if (event.AltDown())
            flags |= wxFILMSTRIP_ALT_DOWN;

		EnsureVisible(n);
        DoSelection(n, flags);

        wxFilmstripEvent cmdEvent(
            wxEVT_COMMAND_FILMSTRIP_LEFT_CLICK,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetIndex(n);
        cmdEvent.SetFlags(flags);
        GetEventHandler()->ProcessEvent(cmdEvent);
    }
}

/// Right-click
void wxFilmstripCtrl::OnRightClick(wxMouseEvent& event)
{
    SetFocus();
    int n;
    if (HitTest(event.GetPosition(), n))
    {
        int flags = 0;
        if (event.ControlDown())
            flags |= wxFILMSTRIP_CTRL_DOWN;
        if (event.ShiftDown())
            flags |= wxFILMSTRIP_SHIFT_DOWN;
        if (event.AltDown())
            flags |= wxFILMSTRIP_ALT_DOWN;

        if (m_focusItem != n)
            SetFocusItem(n);

		wxFilmstripEvent cmdEvent(
            wxEVT_COMMAND_FILMSTRIP_RIGHT_CLICK,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetIndex(n);
        cmdEvent.SetFlags(flags);
        GetEventHandler()->ProcessEvent(cmdEvent);
    }
}

/// Left-double-click
void wxFilmstripCtrl::OnLeftDClick(wxMouseEvent& event)
{
    int n;
    if (HitTest(event.GetPosition(), n))
    {
        int flags = 0;
        if (event.ControlDown())
            flags |= wxFILMSTRIP_CTRL_DOWN;
        if (event.ShiftDown())
            flags |= wxFILMSTRIP_SHIFT_DOWN;
        if (event.AltDown())
            flags |= wxFILMSTRIP_ALT_DOWN;

        wxFilmstripEvent cmdEvent(
            wxEVT_COMMAND_FILMSTRIP_LEFT_DCLICK,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetIndex(n);
        cmdEvent.SetFlags(flags);
        GetEventHandler()->ProcessEvent(cmdEvent);
    }
}

/// Middle-click
void wxFilmstripCtrl::OnMiddleClick(wxMouseEvent& event)
{
    int n;
    if (HitTest(event.GetPosition(), n))
    {
        int flags = 0;
        if (event.ControlDown())
            flags |= wxFILMSTRIP_CTRL_DOWN;
        if (event.ShiftDown())
            flags |= wxFILMSTRIP_SHIFT_DOWN;
        if (event.AltDown())
            flags |= wxFILMSTRIP_ALT_DOWN;

        wxFilmstripEvent cmdEvent(
            wxEVT_COMMAND_FILMSTRIP_MIDDLE_CLICK,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetIndex(n);
        cmdEvent.SetFlags(flags);
        GetEventHandler()->ProcessEvent(cmdEvent);
    }
}

/// Key press
void wxFilmstripCtrl::OnChar(wxKeyEvent& event)
{
    int flags = 0;
    if (event.ControlDown())
        flags |= wxFILMSTRIP_CTRL_DOWN;
    if (event.ShiftDown())
        flags |= wxFILMSTRIP_SHIFT_DOWN;
    if (event.AltDown())
        flags |= wxFILMSTRIP_ALT_DOWN;

    if (event.GetKeyCode() == WXK_LEFT ||
        event.GetKeyCode() == WXK_RIGHT ||
        event.GetKeyCode() == WXK_UP ||
        event.GetKeyCode() == WXK_DOWN ||
        event.GetKeyCode() == WXK_HOME ||
        event.GetKeyCode() == WXK_PAGEUP ||
        event.GetKeyCode() == WXK_PAGEDOWN ||
        event.GetKeyCode() == WXK_PRIOR ||
        event.GetKeyCode() == WXK_NEXT ||
        event.GetKeyCode() == WXK_END)
    {
        Navigate(event.GetKeyCode(), flags);
    }
    else if (event.GetKeyCode() == WXK_RETURN)
    {
        wxFilmstripEvent cmdEvent(
            wxEVT_COMMAND_FILMSTRIP_RETURN,
            GetId());
        cmdEvent.SetEventObject(this);
        cmdEvent.SetFlags(flags);
        GetEventHandler()->ProcessEvent(cmdEvent);
    }
    else
        event.Skip();
}

/// Keyboard navigation
bool wxFilmstripCtrl::Navigate(int keyCode, int flags)
{
    if (GetCount() == 0)
        return false;

    wxSize clientSize = GetClientSize();
    int colsInView = clientSize.x/(m_filmstripOverallSize.x + m_spacing);
    if (colsInView < 1)
        colsInView = 1;

    int rowsInView = 1;

    int focus = m_focusItem;
    if (focus == -1)
        focus = m_lastSelection;

    if (focus == -1 || focus >= GetCount())
    {
        m_lastSelection = 0;
        DoSelection(m_lastSelection, flags);
        ScrollIntoView(m_lastSelection, keyCode);
        return true;
    }

    if (keyCode == WXK_RIGHT)
    {
        int next = focus + 1;
        if (next < GetCount())
        {
            DoSelection(next, flags);
            ScrollIntoView(next, keyCode);
        }
    }
    else if (keyCode == WXK_LEFT)
    {
        int next = focus - 1;
        if (next >= 0)
        {
            DoSelection(next, flags);
            ScrollIntoView(next, keyCode);
        }
    }
    else if (keyCode == WXK_PAGEUP || keyCode == WXK_PRIOR)
    {
        int next = focus - colsInView;
        if (next < 0)
            next = 0;
        if (next >= 0)
        {
            DoSelection(next, flags);
            ScrollIntoView(next, keyCode);
        }
    }
    else if (keyCode == WXK_PAGEDOWN || keyCode == WXK_NEXT)
    {
        int next = focus + colsInView;
        if (next >= GetCount())
            next = GetCount() - 1;
        if (next < GetCount())
        {
            DoSelection(next, flags);
            ScrollIntoView(next, keyCode);
        }
    }
    else if (keyCode == WXK_HOME)
    {
        DoSelection(0, flags);
        ScrollIntoView(0, keyCode);
    }
    else if (keyCode == WXK_END)
    {
        DoSelection(GetCount()-1, flags);
        ScrollIntoView(GetCount()-1, keyCode);
    }
    return true;
}

/// Scroll to see the image
void wxFilmstripCtrl::ScrollIntoView(int n, int keyCode)
{
    wxRect rect;
    GetItemRect(n, rect, false); // _Not_ relative to scroll start

    int ppuX, ppuY;
    GetScrollPixelsPerUnit(& ppuX, & ppuY);

    int startX, startY;
    GetViewStart(& startX, & startY);
    startX = startX * ppuX;
    startY = startY * ppuY;

    int sx, sy;
    GetVirtualSize(& sx, & sy);
    sy = 0;
    if (ppuX != 0)
        sx = sx / ppuX;

    wxSize clientSize = GetClientSize();

    // Going right
    if (keyCode == WXK_RIGHT || keyCode == WXK_END || keyCode == WXK_NEXT || keyCode == WXK_PAGEDOWN)
    {
        if ((rect.x + rect.width) > (clientSize.x + startX))
        {
            // Make it scroll so this item is at the right
            // of the window
            int x = rect.x - (clientSize.x - m_filmstripOverallSize.x - m_spacing) ;
            SetScrollbars(ppuX, ppuY, sx, sy, (int)(0.5 + x) / ppuX, 0);
        }
        else if (rect.x < startX)
        {
            // Make it scroll so this item is at the left
            // of the window
            int x = rect.x;
            SetScrollbars(ppuX, ppuY, sx, sy, (int) (0.5 + x / ppuX), 0);
        }
    }
    // Going left
    else if (keyCode == WXK_LEFT || keyCode == WXK_HOME || keyCode == WXK_PRIOR || keyCode == WXK_PAGEUP)
    {
        if (rect.x < startX)
        {
            // Make it scroll so this item is at the left
            // of the window
            int x = rect.x ;
            SetScrollbars(ppuX, ppuY, sx, sy, (int) (0.5 +  x / ppuX), 0);
        }
        else if ((rect.y + rect.height) > (clientSize.y + startY))
        {
            // Make it scroll so this item is at the right
            // of the window
            int x = rect.x - (clientSize.x - m_filmstripOverallSize.x - m_spacing) ;
            SetScrollbars(ppuX, ppuY, sx, sy, (int) (0.5 + x / ppuX), 0);
        }
    }
}

/// Scrolls the item into view if necessary
void wxFilmstripCtrl::EnsureVisible(int n)
{
    wxRect rect;
    GetItemRect(n, rect, false); // _Not_ relative to scroll start

    int ppuX, ppuY;
    GetScrollPixelsPerUnit(& ppuX, & ppuY);

    if (ppuX == 0)
        return;

    int startX, startY;
    GetViewStart(& startX, & startY);
    startX = startX * ppuX;
    startY = 0;

    int sx, sy;
    GetVirtualSize(& sx, & sy);
    sy = 0;
    if (ppuX != 0)
        sx = sx / ppuX;

    wxSize clientSize = GetClientSize();

    if ((rect.x + rect.width) > (clientSize.x + startX))
    {
        // Make it scroll so this item is at the right
        // of the window
        int x = rect.x - (clientSize.x - m_filmstripOverallSize.x - m_spacing) ;
        SetScrollbars(ppuX, ppuY, sx, sy, (int)(0.5 + x / ppuX), 0);
    }
    else if (rect.x < startX)
    {
        // Make it scroll so this item is at the left
        // of the window
        int x = rect.x ;
        SetScrollbars(ppuX, ppuY, sx, sy, (int)(0.5 + x / ppuX), 0);
    }
}

/// Sizing
void wxFilmstripCtrl::OnSize(wxSizeEvent& event)
{
    SetupScrollbars();
    RecreateBuffer();
    event.Skip();
}

/// Set up scrollbars, e.g. after a resize
void wxFilmstripCtrl::SetupScrollbars()
{
    if (m_freezeCount)
        return;

    if (GetCount() == 0)
    {
        SetScrollbars(0, 0, 0, 0, 0, 0);
        return;
    }

    int lastItem = wxMax(0, GetCount() - 1);
    int pixelsPerUnit = 10;
    wxSize clientSize = GetClientSize();

    int maxWidth = GetCount() * (m_filmstripOverallSize.x + m_spacing) + m_spacing;

    int unitsX = maxWidth / pixelsPerUnit;

    int startX, startY;
    GetViewStart(& startX, & startY);
    
    int maxPositionX = (wxMax(maxWidth - clientSize.x, 0)) / pixelsPerUnit;
	int maxPositionY = 0;
    
    // Move to previous scroll position if
    // possible
    SetScrollbars(pixelsPerUnit, 0,
        unitsX, 0,
        wxMin(maxPositionX, startX), wxMin(maxPositionY, startY));
}

/// Draws the item. Normally you override function in wxFilmstripItem.
bool wxFilmstripCtrl::DrawItem(int n, wxDC& dc, const wxRect& rect, int style)
{
    wxFilmstripItem* item = GetItem(n);
    if (item)
    {
        return item->Draw(dc, this, rect, style);
    }
    else
        return false;
}

/// Draws the background for the item, including bevel
bool wxFilmstripCtrl::DrawItemBackground(int n, wxDC& dc, const wxRect& rect, int style)
{
    wxFilmstripItem* item = GetItem(n);
    if (item)
    {
        return item->DrawBackground(dc, this, rect, style, n);
    }
    else
	{
        return false;
	}
}

/// Do (de)selection
void wxFilmstripCtrl::DoSelection(int n, int flags)
{
    bool isSelected = IsSelected(n);

    wxArrayInt stateChanged;

    bool multiSelect = (GetWindowStyle() & wxFS_MULTIPLE_SELECT) != 0;

    if (multiSelect && (flags & wxFILMSTRIP_CTRL_DOWN) == wxFILMSTRIP_CTRL_DOWN)
    {
        Select(n, !isSelected);
        stateChanged.Add(n);
    }
    else if (multiSelect && (flags & wxFILMSTRIP_SHIFT_DOWN) == wxFILMSTRIP_SHIFT_DOWN)
    {
        // We need to find the last item selected,
        // and select all in between.

        int first = m_firstSelection ;

        // Want to keep the 'first' selection
        // if we're extending the selection
        bool keepFirstSelection = false;
        wxArrayInt oldSelections = m_selections;

        m_selections.Clear(); // TODO: need to refresh those that become unselected. Store old selections, compare with new

        if (m_firstSelection != -1 && m_firstSelection < GetCount() && m_firstSelection != n)
        {
            int step = (n < m_firstSelection) ? -1 : 1;
            int i;
            for (i = m_firstSelection; i != n; i += step)
            {
                if (!IsSelected(i))
                {
                    m_selections.Add(i);
                    stateChanged.Add(i);

                    wxRect rect;
                    GetItemRect(i, rect);
                    RefreshRect(rect);
                }
            }
            keepFirstSelection = true;
        }

        // Refresh all the previously selected items that became unselected
        size_t i;
        for (i = 0; i < oldSelections.GetCount(); i++)
        {
            if (!IsSelected(oldSelections[i]))
            {
                wxRect rect;
                GetItemRect(oldSelections[i], rect);
                RefreshRect(rect);
            }
        }

        Select(n, true);
        if (stateChanged.Index(n) == wxNOT_FOUND)
            stateChanged.Add(n);

        if (keepFirstSelection)
            m_firstSelection = first;
    }
    else
    {
        size_t i = 0;
        for (i = 0; i < m_selections.GetCount(); i++)
        {
            wxRect rect;
            GetItemRect(m_selections[i], rect);
            RefreshRect(rect);

            stateChanged.Add(i);
        }

        m_selections.Clear();
        Select(n, true);
        if (stateChanged.Index(n) == wxNOT_FOUND)
            stateChanged.Add(n);
    }

    // Now notify the app of any selection changes
    size_t i = 0;
    for (i = 0; i < stateChanged.GetCount(); i++)
    {
        wxFilmstripEvent event(
            m_selections.Index(stateChanged[i]) != wxNOT_FOUND ? wxEVT_COMMAND_FILMSTRIP_ITEM_SELECTED : wxEVT_COMMAND_FILMSTRIP_ITEM_DESELECTED,
            GetId());
        event.SetEventObject(this);
        event.SetIndex(stateChanged[i]);
        GetEventHandler()->ProcessEvent(event);
    }
}

/// Find the item under the given point
bool wxFilmstripCtrl::HitTest(const wxPoint& pt, int& n)
{
    wxSize clientSize = GetClientSize();
    int startX, startY;
    int ppuX, ppuY;
    GetViewStart(& startX, & startY);
    GetScrollPixelsPerUnit(& ppuX, & ppuY);

	int colPos = (int) ((pt.x + startX * ppuX) / (m_filmstripOverallSize.x + m_spacing));
    int rowPos = 0;

    int itemN = colPos;
    if (itemN >= GetCount())
        return false;

    wxRect rect;
    GetItemRect(itemN, rect);
    // if (rect.Inside(pt))
    if (rect.Contains(pt))
    {
        n = itemN;
        return true;
    }

    return false;
}

void wxFilmstripCtrl::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    SelectAll();
}

void wxFilmstripCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable( GetCount() > 0 );
}    

/// Paint the background
void wxFilmstripCtrl::PaintBackground(wxDC& dc)
{
    wxColour backgroundColour = GetBackgroundColour();
    if (!backgroundColour.Ok())
        backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    // Clear the background
    dc.SetBrush(wxBrush(backgroundColour));
    dc.SetPen(*wxTRANSPARENT_PEN);
    wxRect windowRect(wxPoint(0, 0), GetClientSize());    
    windowRect.x -= 2; windowRect.y -= 2;
    windowRect.width += 4; windowRect.height += 4;
    
    // We need to shift the rectangle to take into account
    // scrolling. Converting device to logical coordinates.
    CalcUnscrolledPosition(windowRect.x, windowRect.y, & windowRect.x, & windowRect.y);
    dc.DrawRectangle(windowRect);
}

/// Recreate buffer bitmap if necessary
bool wxFilmstripCtrl::RecreateBuffer(const wxSize& size)
{
    wxSize sz = size;
    if (sz == wxDefaultSize)
        sz = GetClientSize();
        
    if (sz.x < 1 || sz.y < 1)
        return false;
        
    if (!m_bufferBitmap.Ok() || m_bufferBitmap.GetWidth() < sz.x || m_bufferBitmap.GetHeight() < sz.y)
        m_bufferBitmap = wxBitmap(sz.x, sz.y);
    return m_bufferBitmap.Ok();
}

/*!
 * wxFilmstripItem
 */

/// Draw the item
bool wxFilmstripItem::Draw(wxDC& WXUNUSED(dc), wxFilmstripCtrl* WXUNUSED(ctrl), const wxRect& WXUNUSED(rect), int WXUNUSED(style))
{
    return false;
}

/// Draw the item background
bool wxFilmstripItem::DrawBackground(wxDC& dc, wxFilmstripCtrl* ctrl, const wxRect& rect, int style, int WXUNUSED(index))
{
    wxColour mediumGrey = ctrl->GetUnselectedFilmstripBackgroundColour();
    wxColour unfocussedDarkGrey = ctrl->GetSelectedFilmstripUnfocussedBackgroundColour();
    wxColour focussedDarkGrey = ctrl->GetSelectedFilmstripFocussedBackgroundColour();
    wxColour darkGrey ;
    if (style & wxFILMSTRIP_FOCUSSED)
        darkGrey = focussedDarkGrey;
    else
        darkGrey = unfocussedDarkGrey;

    if (style & wxFILMSTRIP_SELECTED)
    {
        wxBrush brush(darkGrey);
        wxPen pen(darkGrey);
        dc.SetBrush(brush);
        dc.SetPen(pen);
    }
    else
    {
        wxBrush brush(mediumGrey);
        wxPen pen(mediumGrey);
        dc.SetBrush(brush);
        dc.SetPen(pen);
    }

    dc.DrawRectangle(rect);

    if (style & wxFILMSTRIP_TAGGED)
    {
        wxPen bluePen = ctrl->GetTagColour();
        dc.SetPen(bluePen);

        dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());

        dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetTop());
        dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
    }
    else if (style & wxFILMSTRIP_SELECTED)
    {
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());

        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetTop());
        dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
    }
    else
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        dc.DrawLine(rect.GetLeft(), rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetTop());
        dc.DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom());
    }

    // If the item itself is the focus, draw a dotted
    // rectangle around it
    if (style & wxFILMSTRIP_IS_FOCUS)
    {
        wxPen dottedPen(ctrl->GetFocusRectColour(), 1, wxDOT);
        dc.SetPen(dottedPen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        wxRect focusRect = rect;
        focusRect.x --;
        focusRect.y --;
        focusRect.width += 2;
        focusRect.height += 2;
        dc.DrawRectangle(focusRect);
    }
    
    return true;
}

wxSize wxFilmstripCtrl::DoGetBestSize() const
{
    wxSize sz = wxWindow::DoGetBestSize();
    
    // GDF: Try snapping the height to match the thumb
    sz.SetHeight(m_filmstripOverallSize.y);

    return sz;
}

/*!
 * wxStanFilmstripItem
 */

IMPLEMENT_CLASS(wxStanFilmstripItem, wxFilmstripItem)

/// Draw the item
bool wxStanFilmstripItem::Draw(wxDC& dc, wxFilmstripCtrl* WXUNUSED(ctrl), const wxRect& rect, int WXUNUSED(style))
{
	// dc.DrawCircle(rect.x + (rect.width / 2), rect.y + (rect.height / 2), rect.width / 2);
	// dc.DrawRectangle(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
	// std::cout << "Drawing a wxStanFilmstripItem at " << rect.x << ", " << rect.y << ", " <<
	//	rect.width << ", " << rect.height << std::endl;

	if (frame_ != NULL) {
		wxRect rc = rect;
		stan::WxRender::render_frame(frame_, dc, rc);
	}
    return true;
}
