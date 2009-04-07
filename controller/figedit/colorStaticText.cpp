#include "colorStaticText.h"

IMPLEMENT_DYNAMIC_CLASS(colorStaticText,wxControl);

BEGIN_EVENT_TABLE(colorStaticText, wxControl)
    EVT_PAINT(colorStaticText::OnPaint)
END_EVENT_TABLE()
 
wxSize colorStaticText::DoGetBestSize() const
{
    // Do not return any arbitrary default value...
    //wxASSERT_MSG( m_widget, wxT("colorStaticText::DoGetBestSize called before creation") );

    // There is probably a better way than this, but I don't know it.
    // So I use the const_cast to grab a dc that is only used for metrics.
    wxClientDC dc (const_cast<colorStaticText*> (this));

    wxCoord w = 0;
    wxCoord h = 0;
    dc.GetTextExtent( GetLabel (), &w, &h );

    return wxSize( w, h );
}
 
void colorStaticText::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc (this);
    wxCoord width = 0, height = 0;
    // wxCoord x = 0, y = 0, tw = 0, th = 0;
    
    GetClientSize( &width, &height );

    dc.SetTextForeground (GetForegroundColour ());
    dc.SetTextBackground (GetBackgroundColour ());

    dc.DrawText( GetLabel (), 0, 0);
}
 
void colorStaticText::SetLabel( const wxString& label )
{
    wxControl::SetLabel( label );
    Refresh ();
}

// END of this file -----------------------------------------------------------

