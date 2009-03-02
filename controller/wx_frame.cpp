#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "wx_frame.h"
#include "wx_canvas.h"
#include "animation.h"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string path) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    path_(path)
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_Open, _T("&Open...") );
    menuFile->Append( ID_Load, _T("&Load figure...") );
    menuFile->Append( ID_About, _T("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _T("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );

    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText( _T("Welcome to Stick'm Up!") );

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );

    if (path_ != "")
    {
        LoadShapes();
    }
}

bool MyFrame::LoadShapes()
{
    bool ret = false;
    animation* anim;
	std::ifstream ifs(path_.c_str());
	if (ifs.good())
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("animation", anim);
        if (anim == NULL) {
            std::cerr << "Error loading " << path_ << std::endl;
            return false;
        }
        m_canvas->set_animation(anim);
        ret = true;
	}
    ifs.close();

    return ret;
}

bool MyFrame::LoadFigure(char *path)
{
    std::cout << "Loading figure from: " << path << std::endl;
    return true;
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("XML files (*.xml)|*.xml|TXT files (*.txt)|*.txt");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();
        int filterIndex = dialog.GetFilterIndex();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        SetShapeFilePath(std::string(path));

        LoadShapes();
        m_canvas->Refresh();
    }
}

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("XML files (*.xml)|*.xml|TXT files (*.txt)|*.txt");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();
        int filterIndex = dialog.GetFilterIndex();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        LoadFigure(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is Stick'em Up, the stick figure animator."),
                 _T("About Stick'em Up"), wxOK | wxICON_INFORMATION, this);
}

// END of this file -----------------------------------------------------------
