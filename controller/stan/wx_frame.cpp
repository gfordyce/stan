#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <wx/spinctrl.h>

#include "wx_frame.h"
#include "wx_canvas.h"
#include "wx_render.h"
#include "animation.h"
#include "cut.xpm"
#include "copy.xpm"
#include "play.xpm"
#include "stop.xpm"
#include "forward.xpm"
#include "reverse.xpm"
#include "thumbnaildlg.h"
// #include "thumbnailctrl.h"
#include "filmstripctrl.h"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string data_path, std::string path) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    data_path_(data_path),
    path_(path),
    anim_(NULL),
    timer_(this, TIMER_ID),
    image_()
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

    // Top-level sizer formats vertically: 1) thumbs, and 2) the rest (controls and frame)
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

   	// the thumbnail control
	wxBoxSizer* thumbSizer = new wxBoxSizer(wxHORIZONTAL);
    frameBrowser_ = new wxFilmstripCtrl(this, ID_FRAME_THUMB, wxDefaultPosition, wxSize(150, 125),
				wxSUNKEN_BORDER | wxHSCROLL | wxFS_MULTIPLE_SELECT);
	frameBrowser_->SetFilmstripImageSize(wxSize(150, 100));
    mainSizer->Add(frameBrowser_, 0, wxEXPAND|wxALL, 0);

    // This dude manages the control panel and animation frame (2nd row)
    wxBoxSizer* frameSizer = new wxBoxSizer(wxHORIZONTAL);

	// Control panel
	wxPanel *ctrlPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    wxBitmap copyBitmap(copy_xpm);
    wxBitmapButton* newButton = new wxBitmapButton(ctrlPanel, ID_NewFrame, copyBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

    wxBitmap cutBitmap(cut_xpm);
    wxBitmapButton* delButton = new wxBitmapButton(ctrlPanel, ID_DelFrame, cutBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

    wxBitmap forwardBitmap(forward_xpm);
    wxBitmapButton* forwardButton = new wxBitmapButton(ctrlPanel, ID_NextFrame, forwardBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

    wxBitmap reverseBitmap(reverse_xpm);
    wxBitmapButton* reverseButton = new wxBitmapButton(ctrlPanel, ID_PrevFrame, reverseBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    
    wxBitmap playBitmap(play_xpm);
    wxBitmapButton* playButton = new wxBitmapButton(ctrlPanel, ID_Play, playBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

    wxBitmap stopBitmap(stop_xpm);
    wxBitmapButton* stopButton = new wxBitmapButton(ctrlPanel, ID_Stop, stopBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

    // wxBitmap imageBitmap(wxT("ben.bmp"), wxBITMAP_TYPE_BMP);
    wxImage image;
    image.LoadFile(wxT("c:\\dev\\stan\\build\\msvc8\\debug\\images\\ben2.bmp"));
    image.Rescale(64, 100);
    wxBitmap imageBitmap(image);
    wxBitmapButton* imageButton = new wxBitmapButton(ctrlPanel, ID_Image, imageBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);

    wxStaticText* rateText = new wxStaticText(ctrlPanel, wxID_ANY, wxT("Frame rate:"));
    frameRate_ = new wxSpinCtrl(ctrlPanel, ID_FrameRate, wxT("4"), wxDefaultPosition, wxSize(60, 24), wxSP_ARROW_KEYS, 1, 60, 1);
    repeat_ = new wxCheckBox(ctrlPanel, ID_Repeat, wxT("&Repeat"), wxDefaultPosition, wxDefaultSize);
    repeat_->SetValue(true);
	wxSizer *ctrlSizer = new wxBoxSizer(wxVERTICAL);
    ctrlPanel->SetSizer(ctrlSizer);
    // ctrlSizer->Add(frameStatic);
    ctrlSizer->AddSpacer(20);
    wxBoxSizer* row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row1Sizer->Add(newButton);
    row1Sizer->Add(delButton);
    wxBoxSizer* row2Sizer = new wxBoxSizer(wxHORIZONTAL);
    row2Sizer->Add(reverseButton);
    row2Sizer->Add(forwardButton);
    wxBoxSizer* row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer->Add(playButton);
    row3Sizer->Add(stopButton);
    wxBoxSizer* row4Sizer = new wxBoxSizer(wxHORIZONTAL);
	row4Sizer->Add(imageButton);

    ctrlSizer->Add(row1Sizer);
    ctrlSizer->Add(row2Sizer);
    ctrlSizer->Add(row3Sizer);
    ctrlSizer->Add(row4Sizer);

    ctrlSizer->AddSpacer(20);

    ctrlSizer->Add(rateText);
    ctrlSizer->Add(frameRate_, 0, wxLEFT);
    ctrlSizer->AddSpacer(20);
    ctrlSizer->Add(repeat_);

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
    if (anim_ != NULL) {
        delete anim_;
    }
    anim_ = NULL;

	std::ifstream ifs(path);
	if (ifs.good())	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("animation", anim_);
        if (anim_ == NULL) {
            std::cerr << "Error loading " << path_ << std::endl;
            return false;
        }
        
        // create the image cache
        image_store* ims = anim_->get_image_store();
        WxRender::init_image_cache(ims);

        m_canvas->set_frame(anim_->get_first_frame());
        m_canvas->set_animation(anim_);

		frameBrowser_->Clear();
		frameBrowser_->Freeze();

		// Set some bright colors
		frameBrowser_->SetUnselectedFilmstripBackgroundColour(*wxWHITE);
		frameBrowser_->SetSelectedFilmstripBackgroundColour(*wxWHITE, *wxWHITE);

        // create the browser objects
		std::list<frame*> frames = anim_->get_frames();
		BOOST_FOREACH(frame* fr, frames) {
			frameBrowser_->Append(new wxStanFilmstripItem(fr));
            // for each frame, load the image cache for each figure
       		BOOST_FOREACH(figure* f, fr->get_figures()) {
               image_store* ims = f->get_image_store();
               WxRender::init_image_cache(ims);
            }
        }

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

    if (anim_ != NULL) {
        std::ofstream ofs(path);
        assert(ofs.good());
        {
            boost::archive::xml_oarchive oa(ofs);
            oa << boost::serialization::make_nvp("animation", anim_);

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

        // create the image cache
        image_store* ims = fig->get_image_store();
        WxRender::init_image_cache(ims);

        m_canvas->add_figure(fig);
        ret = true;
	}
    ifs.close();

    return ret;
}

void MyFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
    if (anim_ != NULL) {
        delete anim_;
    }

    frameBrowser_->Clear();
	anim_ = new animation();

	frame* fr = new frame(0, 0, 640, 480);
	anim_->add_frame(fr);
    m_canvas->set_animation(anim_);

	frameBrowser_->Append(new wxStanFilmstripItem(fr));
    select_frame(0);
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    char path[200];
    sprintf_s(path, 200, "%s\\animations", data_path_.c_str());
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("ANI files (*.ani)|*.ani|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[200];
        strncpy_s( path, 200, (const char*)wx_path.mb_str(wxConvUTF8), 200 );
        LoadAnimation(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    char path[200];
    sprintf_s(path, 200, "%s\\figures", data_path_.c_str());
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[200];
        strncpy_s( path, (const char*)wx_path.mb_str(wxConvUTF8), 200 );
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
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );
        SaveAnimation(path);

        m_canvas->Refresh();
    }
}

frame* MyFrame::select_frame(int index)
{
    frame *fr = NULL;
    if ( (index >= 0) && (index < frameBrowser_->GetCount()) ) {
        frameBrowser_->ClearSelections();
        frameBrowser_->EnsureVisible(index);
        frameBrowser_->Select(index);
        wxStanFilmstripItem* item = static_cast<wxStanFilmstripItem*>(frameBrowser_->GetItem(index));
        fr = item->get_frame();
    }
    m_canvas->set_frame(fr);

    return fr;
}

frame* MyFrame::first_frame()
{
    frame* fr = NULL;
    if (frameBrowser_->GetCount() > 0) {
        fr = select_frame(0);
    }
    return fr;
}

frame* MyFrame::next_frame()
{
    frame *fr = NULL;
    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        sel += 1;
        if (sel < frameBrowser_->GetCount()) {
            fr = select_frame(sel);
        }
    }
    return fr;
}

frame* MyFrame::prev_frame()
{
    frame *fr = NULL;
    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        sel -= 1;
        if (sel >= 0) {
            fr = select_frame(sel);
        }
    }
    return fr;
}

void MyFrame::OnNextFrame(wxCommandEvent& WXUNUSED(event))
{
    if (next_frame() != NULL) {
        m_canvas->Refresh();
    }
}

void MyFrame::OnPrevFrame(wxCommandEvent& WXUNUSED(event))
{
    if (prev_frame() != NULL) {
        m_canvas->Refresh();
    }
}

void MyFrame::OnCopyFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Copy frame." << std::endl;
    frame* fr = m_canvas->get_frame();
    if (fr != NULL) {
        frame* cp_frame = new frame(*fr);
        anim_->add_frame(cp_frame);
        m_canvas->set_frame(cp_frame);
        m_canvas->Refresh();
    }
}

void MyFrame::OnNewFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Create a new frame." << std::endl;

    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        wxStanFilmstripItem* item = static_cast<wxStanFilmstripItem*>(frameBrowser_->GetItem(sel));
        if (item != NULL) {
            frame* sel_frame  = item->get_frame();
            frame* fr = new frame(*sel_frame);
            anim_->insert_frame_after(sel_frame, fr);
   			frameBrowser_->Insert(new wxStanFilmstripItem(fr), sel);
            select_frame(sel + 1);
        }
    }
}

void MyFrame::OnDelFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Delete selected frames." << std::endl;

    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        wxStanFilmstripItem* item = static_cast<wxStanFilmstripItem*>(frameBrowser_->GetItem(sel));
        frame* fr = item->get_frame();
        
        anim_->del_frame(fr);
        frameBrowser_->Delete(sel);

        int count = frameBrowser_->GetCount();
        if (sel >= count) {
            sel = count - 1;
        }

        select_frame(sel);
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is Stick'em Up, by Ben, Simon, and Jerry Fordyce."),
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
    if (timer_.IsRunning()) {
        timer_.Stop();
    }
    else {
        int frame_rate = frameRate_->GetValue();
        std::cout << "Play animation with rate " << frame_rate << std::endl;
        first_frame();
        m_canvas->set_animating(true);
        timer_.Start(1000 / frame_rate);
    }
}

void MyFrame::OnStop(wxCommandEvent& event)
{
    std::cout << "Stop animation." << std::endl;
    if (timer_.IsRunning()) {
        m_canvas->set_animating(false);
        timer_.Stop();
        m_canvas->Refresh();
    }
}

void MyFrame::OnThumbNailSelected(wxFilmstripEvent& event)
{
    std::cout << "Thumbnail selected with item index: " << (int)event.GetIndex() << std::endl;
    wxStanFilmstripItem* item = (wxStanFilmstripItem*)frameBrowser_->GetItem(event.GetIndex());
    m_canvas->set_frame(item->get_frame());
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    // Draw the current frame
    m_canvas->Refresh();

    // Sequence to next frame
    if (next_frame() == NULL) {
        if (repeat_->GetValue() == true) {
            first_frame();
        }
        else {
            timer_.Stop();
            m_canvas->set_animating(false);
            first_frame();
        }
    }
}

void MyFrame::OnSelectImage(wxCommandEvent& event)
{
    char path[200];
    sprintf_s(path, 200, "%s\\images", data_path_.c_str());

    wxThumbnailBrowserDialog dialog(this, wxID_ANY, wxT("Choose an image..."));
    dialog.SetSelection(wxT(path));

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetSelection();

        char path[100];
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );

        // create an image object from the path
        int index = WxRender::cache_anim_image(anim_, std::string(path));

        // associate the image with the selected frame
        frame* fr = m_canvas->get_frame();
        fr->set_image_index(index);
        m_canvas->Refresh();
    }
}

// END of this file -----------------------------------------------------------
