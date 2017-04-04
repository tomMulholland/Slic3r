#include "Field.hpp"
#include <regex>

// Pair of regular expressions 
constexpr std::regex false_regex = std::regex("^[ ]*false[ ]*$", std::regex_constants::ECMAScript | std::regex_constants::icase);
constexpr std::regex true_regex = std::regex("^[ ]*true[ ]*$", std::regex_constants::ECMAScript | std::regex_constants::icase);
bool false_str(std::string s) { return std::regex_search(s, false_regex); }
bool true_str(std::string s) { return std::regex_search(s, true_regex); }

namespace Slic3r { namespace GUI {

void CheckBox::BUILD() {
    bool default_value = false;
    if (opt.default_value != nullptr) {
        default_value = 
        ( false_str(opt.default_value) ? false : 
          (true_str(opt.default_value) ? true : false));
    }
    wxCheckBox* temp = new wxCheckBox(_parent, wxID_ANY, "");
    temp->SetValue(default_value);
    if (opt.readonly) { temp->Disable(); }

    _on_change = [=](wxCommandEvent& a) { this->__on_change(a);};

    temp->Bind(wxEVT_CHECKBOX, _on_change, temp->GetId());
    _window = dynamic_cast<wxWindow*>(temp);
}

// Fixed (temporary) function. We can (and probably should) use lambdas instead.
void CheckBox::__on_change(wxCommandEvent& a) {
    printf("Calling _on_change for %d.\n", opt.label);
}

void TextCtrl::BUILD() {
    wxString default_value = "";
    // Empty wxString object fails cast, default to "" if 
    // the recast fails from boost::any.
    try {
        if (opt.default_value != nullptr) {
            default_value = boost::any_cast<wxString>(*(opt.default_value));
        }
    } catch (const boost::bad_any_cast& e) {
        //TODO log error
    }
    auto size = wxSize( (opt.width > 0 ? opt.width : 60), (opt.height > 0 ? opt.height : -1) );

    wxTextCtrl* temp = new wxTextCtrl(_parent, wxID_ANY, default_value, wxDefaultPosition, size, (opt.multiline ? wxTE_MULTILINE : 0));

    _on_change = [=](wxCommandEvent& a) { this->__on_change(a);};

    // This replaces the generic EVT_TEXT call to set the table up, it works with function objects.
    temp->Bind(wxEVT_TEXT, _on_change, temp->GetId());
    if (opt.tooltip.length() > 0) { temp->SetToolTip(opt.tooltip); }

    // recast as a wxWindow to fit the calling convention
    _window = dynamic_cast<wxWindow*>(temp);
}

// Fixed (temporary) function. We can (and probably should) use lambdas instead.
void TextCtrl::__on_change(wxCommandEvent& a) {
    printf("Calling _on_change for %d.\n", opt.label);
}

} }
