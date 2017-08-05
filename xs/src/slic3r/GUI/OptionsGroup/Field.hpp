#include <functional>
#include "../wxinit.h"
#include "Option.hpp"

/// Templated base class for deriving further items from. 
template <typename T>
class Option;

template <class T> 
class Field {
private:
    void default_handler_init() {
        this->_on_change = [=](wxCommandEvent& a) { if (!this->disable_change_event) this->on_change(this->option.opt_id, this->get_value() ); };
        this->_on_kill_focus = [=](wxCommandEvent& a) { a.Skip(1); this->on_kill_focus(this->option.opt_id); };

        // Default to doing nothing
        this->on_change = [=](const wxString&, T&) { };;
        this->on_kill_focus = [=](const wxString&) { };;
    }

public:
    /// Object parent
    wxWindow* const parent;
    /// reference to Slic3r::GUI::OptionsGroup::Option
    const Option<T>& option;

    Field(wxWindow* const parent, const Option<T>& option) : parent(parent), option(option) 
    {
        default_handler_init();
    }

    Field() : parent(nullptr), option(Option<T>()) {
        default_handler_init();
    } 

    /// Functors to manage event handling for this Field. Implemented as functors to permit overriding the 
    /// behavior as desired.
    /// The functors prefaced with _ are for handling the event itself. 

    std::function<void(wxCommandEvent&)> _on_change;
    std::function<void(wxCommandEvent&)> _on_kill_focus;

    /// Used for implementing the actual behavior based on the option id.
    std::function<void(const wxString&, T&)> on_change;
    std::function<void(const wxString&)> on_kill_focus;

    /// Whether or not the associated change event is disabled.
    bool disable_change_event;

    // templated 
    virtual void set_value(const T& value);
    virtual const T& get_value() const;

    /// Public interface 
    virtual void set_tooltip(const wxString& tip) = 0;

    void toggle(const bool newval) { 
        if (newval) { this->enable(); } 
        else { this->disable(); }
    }
        
    virtual void enable() = 0;
    virtual void disable() = 0;

};
// TODO: Add constructor

/// Generic box template class for different kinds of wxObjects 
template <class T, class wxWindowType>
class Window : public Field<T> {
protected:
    wxWindowType* window;

    wxSize _default_size() const {
        return wxSize(
                (this->option.width != 0 ? this->option.width : 60),
                (this->option.height != 0 ? this->option.height : -1)
        );
    }
public:
    void set_tooltip(const wxString& tip) {
        this->window->SetToolTip(tip);
    }
    Window(wxWindow* const parent, const Option<T>& option) : Field<T>(parent, option) {};

    /// _trigger_wxWindow is implemented as an setter for window


};

/// Derivation and full specialization of a wxCheckBox for a Boolean option.
class CheckBox : Window<bool, wxCheckBox> {
public:
    CheckBox(wxWindow* const parent, const Option<bool>& option) : Window<bool,wxCheckBox>(parent, option) {
        window = new wxCheckBox(this->parent, -1, "");
        window->SetValue(this->option.default_value);
        if (this->option.readonly) window->Disable();
        
        // Load on_change lambda
        window->Bind(wxEVT_CHECKBOX, this->_on_change, this->window->GetId());
    }
};

class TextCtrl : Window<wxString, wxTextCtrl> {
public:
    TextCtrl(wxWindow* const parent, const Option<wxString>& option) : Window<wxString,wxTextCtrl>(parent, option) {
        const auto style = ( this->option.multiline ? wxTE_MULTILINE : 0 );

        this->window = new wxTextCtrl(this->parent, -1, this->option.default_value, 
            wxDefaultPosition, this->_default_size(), style);

        window->Bind(wxEVT_TEXT, this->_on_change, this->window->GetId());
        window->Bind(wxEVT_KILL_FOCUS, this->_on_kill_focus, this->window->GetId());
    }

    void enable() {
        this->window->Enable();
        this->window->SetEditable(true);
    }

    void disable() {
        this->window->Disable();
        this->window->SetEditable(false);
    }

};
