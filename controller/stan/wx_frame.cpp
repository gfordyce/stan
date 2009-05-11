#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <wx/spinctrl.h>

#include "wx_frame.h"
#include "wx_canvas.h"
#include "animation.h"
#include "line.xpm"
#include "circle.xpm"
#include "thumbnailctrl.h"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string path) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    path_(path),
    timer_(this, TIMER_ID)
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_New, _T("&New...") );
    menuFile->Append( ID_Open, _T("&Open...") );
    menuFile->Append( ID_Load, _T("&Load figure...") );
    menuFile->Append( ID_Save, _T("Save &As...") );
    menuFile->Append( ID_NextFrame, _T("&Next frame...\tCtrl+N") );
    menuFile->Append( ID_PrevFrame, _T("&Previous frame...\tCtrl+P") );
    menuFile->Append( ID_CopyFrame, _T("&Dup frame...\tCtrl+D") );
    menuFile->Append( ID_About, _T("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _T("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );

    SetMenuBar( menuBar );

#if 0
    // Attempt a toolbar
    m_toolbar = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, ID_Toolbar );
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));
    wxBitmap lineBitmap(line_xpm);
    wxBitmap circleBitmap(circle_xpm);
    // wxBitmap lineBitmap(wxT("bitmaps/line.png"), wxBITMAP_TYPE_PNG);
    // wxBitmap lineBitmap(GetBitmapResource(wxT("bitmaps/line.png")));
    m_toolbar->AddTool(ID_Line, _T(""), lineBitmap, _("Line tool"), wxITEM_NORMAL);
    m_toolbar->AddTool(ID_Circle, _T(""), circleBitmap, _("Circle tool"), wxITEM_NORMAL);
    m_toolbar->Realize();
    //SetToolBar(m_toolbar);
#endif

    // Frame has a panel which is fitted to it
    // wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    // SetSizer(topSizer);

  	// wxPanel *mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    // topSizer->Add(mainPanel, 1, wxEXPAND | wxALL, 0);

    // Top-level sizer formats vertically: 1) thumbs, and 2) the rest (controls and frame)
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

   	// the thumbnail control
	wxBoxSizer* thumbSizer = new wxBoxSizer(wxHORIZONTAL);
	frameBrowser_ = new wxThumbnailCtrl(this, ID_FRAME_THUMB, wxDefaultPosition, wxSize(150, 125),
				wxSUNKEN_BORDER | wxHSCROLL | wxTH_MULTIPLE_SELECT);
	frameBrowser_->SetThumbnailImageSize(wxSize(150, 100));
	// thumbSizer->Add(frameBrowser_, 1, wxEXPAND | wxALL, 0);
	// mainSizer->Add(thumbSizer, 0, wxALL, 0);
    mainSizer->Add(frameBrowser_, 0, wxEXPAND|wxALL, 0);

    // This dude manages the control panel and animation frame (2nd row)
    wxBoxSizer* frameSizer = new wxBoxSizer(wxHORIZONTAL);

	// Control panel
	wxPanel *ctrlPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxButton* newButton = new wxButton(ctrlPanel, ID_NewFrame, wxT("New Frame"), wxDefaultPosition, wxDefaultSize);
   	wxButton* delButton = new wxButton(ctrlPanel, ID_DelFrame, wxT("Delete Frame"), wxDefaultPosition, wxDefaultSize);
	wxButton* playButton = new wxButton(ctrlPanel, ID_Play, wxT("Play / Stop"), wxDefaultPosition, wxDefaultSize);
    wxSpinCtrl* frameRate = new wxSpinCtrl(ctrlPanel, ID_FrameRate, wxT("10"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 60, 1);
	wxSizer *ctrlSizer = new wxBoxSizer(wxVERTICAL);
    ctrlPanel->SetSizer(ctrlSizer);
    ctrlSizer->AddSpacer(20);
	ctrlSizer->Add(newButton);
    ctrlSizer->Add(delButton);
	ctrlSizer->Add(playButton);
    ctrlSizer->Add(frameRate);
    // ctrlSizer->Add(m_toolbar, 1, wxEXPAND|wxALL, 0);

    // Animation frames rendered on fixed size canvas (640 x 480)
    m_canvas = new MyCanvas( this, wxID_ANY, wxDefaultPosition, wxSize(640, 480) );
    m_canvas->SetScrollbars( 0, 0, 0, 0 );
    m_canvas->SetBackgroundColour(wxColour(255, 255, 255));

	frameSizer->Add(ctrlPanel, 1, wxEXPAND, 0, 0);
    frameSizer->Add(m_canvas, 0, wxALL, 0);
    mainSizer->Add(frameSizer, 1, wxEXPAND|wxALL, 0);

    CreateStatusBar();
    SetStatusText( _T("Welcome to Stick'm Up!") );

    if (path_ != "NA") {
        LoadAnimation(const_cast<char *>(path_.c_str()));
    }
}

bool MyFrame::LoadAnimation(char *path)
{
    std::cout << "Loading animation from: " << path << std::endl;

    bool ret = false;
    animation* anim = NULL;
	std::ifstream ifs(path);
	if (ifs.good())
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("animation", anim);
        if (anim == NULL) {
            std::cerr << "Error loading " << path_ << std::endl;
            return false;
        }
        m_canvas->set_animation(anim);

		frameBrowser_->Clear();
		frameBrowser_->Freeze();

		// Set some bright colors
		frameBrowser_->SetUnselectedThumbnailBackgroundColour(*wxWHITE);
		frameBrowser_->SetSelectedThumbnailBackgroundColour(*wxWHITE, *wxWHITE);

		std::list<frame*> frames = anim->get_frames();
		BOOST_FOREACH(frame* fr, frames) {
			frameBrowser_->Append(new wxStanThumbnailItem(fr));
        }

		// Tag and select the first thumbnail
		// frameBrowser_->Tag(0);
		frameBrowser_->Select(0);

		frameBrowser_->Thaw();
        ret = true;
	}
    ifs.close();

    return ret;
}

bool MyFrame::SaveAnimation(char* path)
{
    std::cout << "Saving animation to: " << path << std::endl;

    animation* anim = m_canvas->get_animation();
    if (anim != NULL) {
        std::ofstream ofs(path);
        assert(ofs.good());
        {
            boost::archive::xml_oarchive oa(ofs);
            oa << boost::serialization::make_nvp("animation", anim);

        }
        ofs.close();
    }
    return true;
}

bool MyFrame::LoadFigure(char *path)
{
    std::cout << "Loading figure from: " << path << std::endl;

    bool ret = false;
    figure* fig;
	std::ifstream ifs(path);
	if (ifs.good())
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("figure", fig);
        if (fig == NULL) {
            std::cerr << "Error loading " << path << std::endl;
            return false;
        }
        m_canvas->add_figure(fig);
        ret = true;
	}
    ifs.close();

    return ret;
}

void MyFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
	animation* an = new animation();

	frame* fr = new frame(0, 0, 640, 480);
	an->add_frame(fr);

	m_canvas->set_animation(an);
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("ANI files (*.ani)|*.ani|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        LoadAnimation(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        LoadFigure(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Save as ?");
    wxString wildcard = wxT("ANI files (*.ani)|*.ani|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        SaveAnimation(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnNextFrame(wxCommandEvent& WXUNUSED(event))
{
    m_canvas->next_frame();
    m_canvas->Refresh();
}

void MyFrame::OnPrevFrame(wxCommandEvent& WXUNUSED(event))
{
    m_canvas->prev_frame();
    m_canvas->Refresh();
}

void MyFrame::OnCopyFrame(wxCommandEvent& WXUNUSED(event))
{
    m_canvas->copy_frame();
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

void MyFrame::OnLine(wxCommandEvent& event)
{
    std::cout << "Line tool." << std::endl;
}

void MyFrame::OnCircle(wxCommandEvent& event)
{
    std::cout << "Circle tool." << std::endl;
}

void MyFrame::OnPlay(wxCommandEvent& event)
{
    std::cout << "Play animation." << std::endl;
    if (timer_.IsRunning()) {
        timer_.Stop();
    }
    else {
        int frame_rate = 10;
        m_canvas->first_frame();
        timer_.Start(1000 / frame_rate);
    }
}

void MyFrame::OnThumbNailSelected(wxThumbnailEvent& event)
{
    std::cout << "Thumbnail selected with item index: " << (int)event.GetIndex() << std::endl;
    wxStanThumbnailItem* item = (wxStanThumbnailItem*)frameBrowser_->GetItem(event.GetIndex());
    m_canvas->set_frame(item->get_frame());
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    // Do whatever you want to do every second here
    std::cout << "Timer event." << std::endl;

    // Draw the current frame
    m_canvas->Refresh();

    // Sequence to next frame
    if (m_canvas->next_frame() == NULL) {
        timer_.Stop();
    }
}

// END of this file -----------------------------------------------------------
