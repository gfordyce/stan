#ifndef _COLORSTATICTEXT
#define _COLORSTATICTEXT        1

#include <wx/wx.h>

class colorStaticText : public wxControl
{
public: 
    DECLARE_DYNAMIC_CLASS(colorStaticText);
 
    colorStaticText () {}
    colorStaticText (wxWindow* parent, wxWindowID id, const wxString& txt) :
        wxControl (parent, id) { SetLabel (txt); }
 
    wxSize DoGetBestSize() const;
    void OnPaint(wxPaintEvent& event);
    void SetLabel( const wxString& label );
 
    DECLARE_EVENT_TABLE();
};
 
#endif // _COLORSTATICTEXT
