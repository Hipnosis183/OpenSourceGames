// -*- C++ -*-
//
// generated by wxGlade not found on Tue Jun  5 23:21:41 2018
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef INSTALLVERIFYFRAME_BASE_H
#define INSTALLVERIFYFRAME_BASE_H

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/intl.h>

#ifndef APP_CATALOG
#define APP_CATALOG "app"  // replace with the appropriate catalog name
#endif


// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class InstallVerifyFrame_Base: public wxDialog {
public:
    // begin wxGlade: InstallVerifyFrame_Base::ids
    // end wxGlade

    InstallVerifyFrame_Base(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);

private:
    // begin wxGlade: InstallVerifyFrame_Base::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: InstallVerifyFrame_Base::attributes
    wxTextCtrl* mDmodDescription;
    wxRadioBox* mDestdirBox;
    wxButton* mInstallButton;
    wxButton* mCancelButton;
    // end wxGlade
}; // wxGlade: end class


#endif // INSTALLVERIFYFRAME_BASE_H
