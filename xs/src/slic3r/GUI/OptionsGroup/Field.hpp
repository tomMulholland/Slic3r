#include <functional>
/// Templated base class for deriving further items from. 
// temporary forward class
class wxWindow;
class wxString;
class wxCommandEvent;
class wxCheckBox;
template <typename T>
class Option;

template <class T> 
class Field {

public:
    /// Object parent
    wxWindow* const parent;
    /// reference to Slic3r::GUI::OptionsGroup::Option
    const Option<T>& option;

    Field(wxWindow* const parent, const Option<T>& option) : parent(parent), option(option) {}

    Field() : parent(nullptr), option(Option<T>()) {} 

    /// Functors to manage event handling for this Field. 
    std::function<void(wxCommandEvent&)> _on_change;
    std::function<void(wxCommandEvent&)> _on_kill_focus;
    bool disable_change_event;

    // templated 
    virtual void set_value(const T& value);
    virtual const T& get_value();

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

/// Generic box template class with 
template <class T, class wxWindowType>
class Window : public Field<T> {
protected:
    wxWindowType* window;
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
        window->SetValue(this->option.default);
        if (this->option.readonly) window->Disable();
        
        // Lambda function to call out to on_change
        this->_on_change = [=](wxCommandEvent& a) 
        {  if (!this->disable_change_event) 
        this->on_change(this->option, this->window->GetValue() ); };

        // Load on_change lambda
        window->Bind(wxEVT_CHECKBOX, this->_on_change, this->option.opt_id);
    }
};
