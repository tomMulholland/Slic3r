#include "../wxinit.h"

template <typename T>
class Option {
public:
    wxString opt_id;
    std::string type;
    T default_value;
    bool readonly;

};
