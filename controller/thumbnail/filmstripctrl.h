/**
 * @file filmstripctrl.h - Displays a horizontally scrolling filmstrip
 * @author Gerald Fordyce
 * @acknowlegements Julian Smart
 */

#ifndef _WX_FILMSTRIPCTRL_H_
#define _WX_FILMSTRIPCTRL_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filmstripctrl.cpp"
#endif

#include "wx/dynarray.h"
#include "frame.h"

/*!
 * Includes
 */

/*!
 * Styles and flags
 */

/* Styles
 */

#define wxFS_MULTIPLE_SELECT    0x0010
#define wxFS_SINGLE_SELECT      0x0000

/* Flags
 */

#define wxFILMSTRIP_SHIFT_DOWN  0x01
#define wxFILMSTRIP_CTRL_DOWN   0x02
#define wxFILMSTRIP_ALT_DOWN    0x04

/* Defaults
 */

#define wxFILMSTRIP_DEFAULT_OVERALL_SIZE wxSize(-1, -1)
#define wxFILMSTRIP_DEFAULT_SPACING 3
#define wxFILMSTRIP_DEFAULT_MARGIN 3
#define wxFILMSTRIP_DEFAULT_UNFOCUSSED_BACKGROUND wxColour(175, 175, 175)
#define wxFILMSTRIP_DEFAULT_FOCUSSED_BACKGROUND wxColour(140, 140, 140)
// #define wxFILMSTRIP_DEFAULT_UNSELECTED_BACKGROUND wxColour(205, 205, 205)
#define wxFILMSTRIP_DEFAULT_UNSELECTED_BACKGROUND wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)
#define wxFILMSTRIP_DEFAULT_TYPE_COLOUR wxColour(0, 0, 200)
#define wxFILMSTRIP_DEFAULT_TAG_COLOUR wxColour(0, 0, 255)
#define wxFILMSTRIP_DEFAULT_FOCUS_RECT_COLOUR wxColour(100, 80, 80)

/*!
 * Forward declarations
 */

class wxFilmstripCtrl;

/*!
 * wxFilmstripItem class declaration
 */

// Drawing styles/states
#define wxFILMSTRIP_SELECTED    0x01
#define wxFILMSTRIP_TAGGED      0x02
// The control is focussed
#define wxFILMSTRIP_FOCUSSED    0x04
// The item itself has the focus
#define wxFILMSTRIP_IS_FOCUS    0x08

class wxFilmstripItem: public wxObject
{
    DECLARE_CLASS(wxFilmstripItem)
public:
    wxFilmstripItem() {}

    /// Draw the item
    virtual bool Draw(wxDC& dc, wxFilmstripCtrl* ctrl, const wxRect& rect, int style) ;

    /// Draw the background
    virtual bool DrawBackground(wxDC& dc, wxFilmstripCtrl* ctrl, const wxRect& rect, int style, int index) ;
};

/*!
 * wxStanFilmstripItem class declaration
 * This item renders a scaled stan frame.
 */

class wxStanFilmstripItem: public wxFilmstripItem
{
    DECLARE_CLASS(wxStanFilmstripItem)
public:

	wxStanFilmstripItem(stan::frame* fr) :
		frame_(fr)
		{}

    /// Draw the item
    virtual bool Draw(wxDC& dc, wxFilmstripCtrl* ctrl, const wxRect& rect, int style) ;

    stan::frame* get_frame() { return frame_; }

protected:
	stan::frame* frame_;	// STAN frame
};

WX_DECLARE_OBJARRAY(wxFilmstripItem, wxFilmstripItemArray);

/*!
 * wxFilmstripCtrl class declaration
 */

class wxFilmstripCtrl: public wxScrolledWindow
{    
    DECLARE_CLASS( wxFilmstripCtrl )
    DECLARE_EVENT_TABLE()

public:
// Constructors

    wxFilmstripCtrl( );
    wxFilmstripCtrl( wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = 0 );

// Operations

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = 0 );

    /// Member initialisation
    void Init();

    /// Call Freeze to prevent refresh
    void Freeze();

    /// Call Thaw to refresh
    void Thaw();

    /// Scrolls the item into view if necessary
    void EnsureVisible(int n);

    /// Draws the item. Normally you override function in wxFilmstripItem.
    virtual bool DrawItem(int n, wxDC& dc, const wxRect& rect, int style) ;

    /// Draws the background for the item, including bevel
    virtual bool DrawItemBackground(int n, wxDC& dc, const wxRect& rect, int style) ;

// Adding items

    /// Append a single item
    virtual int Append(wxFilmstripItem* item);

    /// Insert a single item
    virtual int Insert(wxFilmstripItem* item, int pos = 0);

// Deleting items

    /// Clear all items
    virtual void Clear() ;

    /// Delete this item
    virtual void Delete(int n) ;

// Accessing items

    /// Get the number of items in the control
    virtual int GetCount() const { return m_items.GetCount(); }

    /// Is the control empty?
    bool IsEmpty() const { return GetCount() == 0; }

    /// Get the nth item
    wxFilmstripItem* GetItem(int n);

    /// Get the overall rect of the given item
    /// If transform is true, rect is relative to the scroll viewport
    /// (i.e. may be negative)
    bool GetItemRect(int item, wxRect& rect, bool transform = true);

    /// Get the image rect of the given item
    bool GetItemRectImage(int item, wxRect& rect, bool transform = true);

    /// Return the column given the client
    /// size and a left-to-right layout assumption
    bool GetCol(int item, const wxSize& clientSize, int& col);

    /// Get the focus item, or -1 if there is none
    int GetFocusItem() const { return m_focusItem; }

    /// Set the focus item
    void SetFocusItem(int item) ;

// Selection

    /// Select or deselect an item
    void Select(int n, bool select = true) ;

    /// Select or deselect a range
    void SelectRange(int from, int to, bool select = true) ;

    /// Tag or untag an item
    void Tag(int n, bool tag = true) ;

    /// Select all
    void SelectAll() ;

    /// Select none
    void SelectNone() ;

	/// Get the index of the single selection, if not multi-select.
    /// Returns -1 if there is no selection.
    int GetSelection() const ;

    /// Get indexes of all selections, if multi-select
    const wxArrayInt& GetSelections() const { return m_selections; }

    /// Get indexes of all tags
    const wxArrayInt& GetTags() const { return m_tags; }

    /// Returns true if the item is selected
    bool IsSelected(int n) const ;

    /// Returns true if the item is tagged
    bool IsTagged(int n) const ;

    /// Clears all selections
    void ClearSelections();

    /// Clears all tags
    void ClearTags();

// Visual properties

    /// The overall size of the filmstrip, including decorations.
    /// DON'T USE THIS from the application, since it will
    /// normally be calculated by SetFilmstripImageSize.
    void SetFilmstripOverallSize(const wxSize& sz) { m_filmstripOverallSize = sz; }
    const wxSize& GetFilmstripOverallSize() const { return m_filmstripOverallSize; }

    /// The size of the image part
    void SetFilmstripImageSize(const wxSize& sz);
    const wxSize& GetFilmstripImageSize() const { return m_filmstripImageSize; }

    /// The inter-item spacing
    void SetSpacing(int spacing) { m_spacing = spacing; }
    int GetSpacing() const { return m_spacing; }

    /// The margin between elements within the filmstrip
    void SetFilmstripMargin(int margin) { m_filmstripMargin = margin; }
    int GetFilmstripMargin() const { return m_filmstripMargin; }

    /// The focussed and unfocussed background colour for a
    /// selected filmstrip
    void SetSelectedFilmstripBackgroundColour(const wxColour& focussedColour, const wxColour& unfocussedColour)
    {
        m_focussedFilmstripBackgroundColour = focussedColour; m_unfocussedFilmstripBackgroundColour = unfocussedColour;
    }
    const wxColour& GetSelectedFilmstripFocussedBackgroundColour() const { return m_focussedFilmstripBackgroundColour; }
    const wxColour& GetSelectedFilmstripUnfocussedBackgroundColour() const { return m_unfocussedFilmstripBackgroundColour; }

    /// The unselected background colour for a filmstrip
    void SetUnselectedFilmstripBackgroundColour(const wxColour& colour) { m_unselectedFilmstripBackgroundColour = colour; }
    const wxColour& GetUnselectedFilmstripBackgroundColour() const { return m_unselectedFilmstripBackgroundColour; }

    /// The colour for the tag outline
    void SetTagColour(const wxColour& colour) { m_tagColour = colour; }
    const wxColour& GetTagColour() const { return m_tagColour; }

    /// The focus rectangle pen colour
    void SetFocusRectColour(const wxColour& colour) { m_focusRectColour = colour; }
    const wxColour& GetFocusRectColour() const { return m_focusRectColour; }

// Command handlers

    void OnSelectAll(wxCommandEvent& event);
    void OnUpdateSelectAll(wxUpdateUIEvent& event);
    
// Event handlers

    /// Painting
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    /// Left-click
    void OnLeftClick(wxMouseEvent& event);

    /// Left-double-click
    void OnLeftDClick(wxMouseEvent& event);

    /// Middle-click
    void OnMiddleClick(wxMouseEvent& event);

    /// Right-click
    void OnRightClick(wxMouseEvent& event);

    /// Key press
    void OnChar(wxKeyEvent& event);

    /// Sizing
    void OnSize(wxSizeEvent& event);

    /// Setting/losing focus
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

// Implementation

    /// Set up scrollbars, e.g. after a resize
    void SetupScrollbars();
	
    /// Calculate the outer filmstrip size based
    /// on font used for text and inner size
    void CalculateOverallFilmstripSize();

    /// Do (de)selection
    void DoSelection(int n, int flags);

    /// Find the item under the given point
    bool HitTest(const wxPoint& pt, int& n);

    /// Keyboard navigation
    virtual bool Navigate(int keyCode, int flags);

    /// Scroll to see the image
    void ScrollIntoView(int n, int keyCode);

    /// Paint the background
    void PaintBackground(wxDC& dc);
    
    /// Recreate buffer bitmap if necessary
    bool RecreateBuffer(const wxSize& size = wxDefaultSize);
    
// Overrides
    wxSize DoGetBestSize() const ;

// Data members
private:

    /// The items
    wxFilmstripItemArray    m_items;

    /// The selections
    wxArrayInt              m_selections;

    /// The tags
    wxArrayInt              m_tags;

    /// Outer size of the filmstrip item
    wxSize                  m_filmstripOverallSize;

    /// Image size of the filmstrip item
    wxSize                  m_filmstripImageSize;

    /// The inter-item spacing
    int                     m_spacing;

    /// The margin between the image/text and the edge of the filmstrip
    int                     m_filmstripMargin;

	/// Allows nested Freeze/Thaw
    int                     m_freezeCount;

    /// First selection in a range
    int                     m_firstSelection;

    /// Last selection
    int                     m_lastSelection;

    /// Focus item
    int                     m_focusItem;

    /// Focussed/unfocussed selected filmstrip background colours
    wxColour                m_focussedFilmstripBackgroundColour;
    wxColour                m_unfocussedFilmstripBackgroundColour;
    wxColour                m_unselectedFilmstripBackgroundColour;
    wxColour                m_focusRectColour;

    /// Tag colour
    wxColour                m_tagColour;
    
    /// Buffer bitmap
    wxBitmap                m_bufferBitmap;
};

/*!
 * wxFilmstripEvent - the event class for wxFilmstripCtrl notifications
 */

class wxFilmstripEvent : public wxNotifyEvent
{
public:
    wxFilmstripEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxNotifyEvent(commandType, winid),
        m_itemIndex(-1), m_flags(0)
        { }

    wxFilmstripEvent(const wxFilmstripEvent& event)
        : wxNotifyEvent(event),
        m_itemIndex(event.m_itemIndex), m_flags(event.m_flags)
        { }

    int GetIndex() const { return m_itemIndex; }
    void SetIndex(int n) { m_itemIndex = n; }

    int GetFlags() const { return m_flags; }
    void SetFlags(int flags) { m_flags = flags; }

    virtual wxEvent *Clone() const { return new wxFilmstripEvent(*this); }

protected:
    int           m_itemIndex;
    int           m_flags;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxFilmstripEvent)
};

/*!
 * wxFilmstripCtrl event macros
 */

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_ITEM_SELECTED, 2600)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_ITEM_DESELECTED, 2601)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_LEFT_CLICK, 2602)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_RIGHT_CLICK, 2603)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_MIDDLE_CLICK, 2604)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_LEFT_DCLICK, 2605)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_FILMSTRIP_RETURN, 2606)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxFilmstripEventFunction)(wxFilmstripEvent&);

#define EVT_FILMSTRIP_ITEM_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),
#define EVT_FILMSTRIP_ITEM_DESELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),
#define EVT_FILMSTRIP_LEFT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_LEFT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),
#define EVT_FILMSTRIP_RIGHT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),
#define EVT_FILMSTRIP_MIDDLE_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),
#define EVT_FILMSTRIP_LEFT_DCLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_LEFT_DCLICK, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),
#define EVT_FILMSTRIP_RETURN(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_FILMSTRIP_RETURN, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxFilmstripEventFunction, & fn ), NULL ),

#endif    // _WX_FILMSTRIPCTRL_H_
