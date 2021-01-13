// Integer wrapper class to store the DMod index associated with a
// listbox entry from mDModListBox. Previously I was abusing 'void*'
// as 'int' but g++ produces an error under amd64.  If we were using
// glib, we'd use GINT_TO_POINTER().  Sadly, we're not using glib :/
// Using wxClientData as a base class to make the wxListBox
// automatically free client data when needed.
class Integer : public wxClientData
{
 public:
  int i;
  Integer(int i) : i(i) {}
};

// Same for wxString
class ClientDataString : public wxClientData
{
 public:
  wxString s;
  ClientDataString(wxString s) : s(s) {}
};
