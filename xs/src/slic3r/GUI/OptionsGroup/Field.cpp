#include "Field.hpp"
#include <regex>

// Pair of regular expressions 
const static std::regex false_regex = std::regex("^[ ]*false[ ]*$", std::regex_constants::ECMAScript | std::regex_constants::icase);
const static std::regex true_regex = std::regex("^[ ]*true[ ]*$", std::regex_constants::ECMAScript | std::regex_constants::icase);
bool false_str(std::string s) { return std::regex_search(s, false_regex); }
bool true_str(std::string s) { return std::regex_search(s, true_regex); }

namespace Slic3r { namespace GUI {

void CheckBox::BUILD() {
    bool default_value = false;
    if (opt.default_value != nullptr) {
        // TODO Process default value text to a Boolean
    }
    wxCheckBox* temp = new wxCheckBox(_parent, wxID_ANY, "");
    temp->SetValue(default_value);
    if (opt.readonly) { temp->Disable(); }

    _on_change = [=](wxCommandEvent& a) 
        {  if (this->_window->IsEnabled()) 
            this->on_change(this->opt, boost::any(dynamic_cast<wxCheckBox*>(this->_window)->GetValue()) ); };

    temp->Bind(wxEVT_CHECKBOX, _on_change, temp->GetId());
    _window = dynamic_cast<wxWindow*>(temp);
}

void TextCtrl::BUILD() {
    wxString default_value = "";
    // Empty wxString object fails cast, default to "" if 
    // the recast fails from boost::any.
    try {
        if (opt.default_value != nullptr) {
        // TODO: Figure out what type this is so we can cast it correctly
        //    default_value = boost::any_cast<wxString>(*(opt.default_value));
        }
    } catch (const boost::bad_any_cast& e) {
        //TODO log error
    }
    auto size = wxSize( (opt.width > 0 ? opt.width : 60), (opt.height > 0 ? opt.height : -1) );

    wxTextCtrl* temp = new wxTextCtrl(_parent, wxID_ANY, default_value, wxDefaultPosition, size, (opt.multiline ? wxTE_MULTILINE : 0));

    // TODO Work out better names for the on_change passthrough, we the local on_change needs to be registered
    // with the Event handler through Bind().
    // Set the event handler to call the registered OptionsGroup function (which actually processes the 
    // event and modifies Slic3r::Config)
    _on_change = [=](wxCommandEvent& a) 
        {  if (this->_window->IsEnabled()) 
        this->on_change(this->opt, boost::any(dynamic_cast<wxTextCtrl*>(this->_window)->GetValue()) ); };
    // This replaces the generic EVT_TEXT call to set the table up, it works with function objects.
    temp->Bind(wxEVT_TEXT, _on_change, temp->GetId());
    if (opt.tooltip.length() > 0) { temp->SetToolTip(opt.tooltip); }

    // recast as a wxWindow to fit the calling convention
    _window = dynamic_cast<wxWindow*>(temp);
}

} }
