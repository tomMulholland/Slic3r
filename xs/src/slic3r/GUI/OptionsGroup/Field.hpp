#ifndef FIELD_HPP
#define FIELD_HPP

#include "../wxinit.h"

#include <functional>
#include <boost/any.hpp>
#include <map>

#ifdef SLIC3R_DEBUG
#include <iostream>
#endif

// ConfigOptionDef definition
#include "Config.hpp"

namespace Slic3r { 
class ConfigOptionDef;
namespace GUI {

/// Interface class for fields
/// 
class Field 
{
protected:
    wxWindow* _parent;

    /// Instantiate the underlying wxWidgets control/window.
    /// This function is expected to be called by the constructor.
    virtual void BUILD() = 0;

    /// Reference to underlying ConfigOptionDef this Field is
    /// implementing.
    /// TODO: This may not be necessary.
    const ConfigOptionDef& opt; 

public:
    std::function<void(wxCommandEvent&)> _on_change;
    std::function<void(wxCommandEvent&)> _on_kill_focus;
    std::function<void(const ConfigOptionDef& opt, boost::any value)> on_change; 

    // used if we need to know which ConfigOptionDef this corresponds.
    Field() : opt(ConfigOptionDef()), _on_change(nullptr), _on_kill_focus(nullptr), on_change(nullptr) {}
    Field(const ConfigOptionDef& opt) : opt(opt), _on_change(nullptr), _on_kill_focus(nullptr), on_change(nullptr) { }
    Field(wxFrame* parent, const ConfigOptionDef& opt) : opt(opt),  _parent(parent),
        _on_change(nullptr), _on_kill_focus(nullptr), on_change(nullptr) { }

    /// Return the wxWidgets ID for this object.
    ///
    wxWindowID get_id() { if (this->window() != nullptr) return this->window()->GetId(); }
    virtual wxWindow* window() = 0;
    virtual wxSizer* sizer() = 0;

    /// Sets a value for this control.
    /// subclasses should overload with a specific version
    virtual void set_value(boost::any value) = 0;
    
    /// Gets a boost::any representing this control.
    /// subclasses should overload with a specific version
    virtual boost::any get_value() = 0;

    /// subclasses should overload with a specific version
    virtual void enable() = 0;
    virtual void disable() = 0;

};

class Window : public Field 
{
    protected:
    wxWindow* _window;
    public:
    Window(wxFrame* parent, const ConfigOptionDef& opt) : Field(parent, opt) { };
    wxWindow* window() { return _window; }
    wxSizer* sizer() { return nullptr; }
};

class Sizer : public Field 
{
    protected: 
    wxSizer* _sizer;
    public:
    Sizer(wxFrame* parent, const ConfigOptionDef& opt) : Field(parent, opt) { };
    wxSizer* sizer() { return _sizer; }
    wxWindow* window() { return nullptr; }
};

class CheckBox : public Window 
{
    protected:
    void BUILD();
    public:
    CheckBox();
    CheckBox(wxFrame* parent, const ConfigOptionDef& opt) : Window(parent, opt) { BUILD(); };

    void set_value(bool value);
    void set_value(boost::any value);
    boost::any get_value() { return boost::any(dynamic_cast<wxCheckBox*>(_window)->GetValue()); }

    void enable() { dynamic_cast<wxCheckBox*>(_window)->Enable(); }
    void disable() { dynamic_cast<wxCheckBox*>(_window)->Disable(); }
};

class TextCtrl : public Window {
    protected:
    void BUILD();
    public:
    TextCtrl();
    TextCtrl(wxFrame* parent, const ConfigOptionDef& opt) : Window(parent, opt) { BUILD(); };

    void set_value(std::string value) { 
            dynamic_cast<wxTextCtrl*>(_window)->SetValue(wxString(value));
    }
    void set_value(boost::any value) { 
        try {
            dynamic_cast<wxTextCtrl*>(_window)->SetValue(boost::any_cast<wxString>(value));
        } catch (boost::bad_any_cast) {
            // TODO Log error and do nothing
            #ifdef SLIC3R_DEBUG
                std::cerr << "Bad cast from TextCtrl set_value, not a string?\n";
            #endif
        }
    }
    boost::any get_value() { return boost::any(dynamic_cast<wxTextCtrl*>(_window)->GetValue()); }
    void enable() { dynamic_cast<wxTextCtrl*>(_window)->Enable(); dynamic_cast<wxTextCtrl*>(_window)->SetEditable(1); }
    void disable() { dynamic_cast<wxTextCtrl*>(_window)->Disable(); dynamic_cast<wxTextCtrl*>(_window)->SetEditable(0); }
    
};

} }
#endif
