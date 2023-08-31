#include "IoFile.h"

IoFile::IoFile(wxFrame* Frame)
{
    // Create file dialogs with parent
    MOpenFileDialog = std::make_unique<wxFileDialog>(Frame, "Open File", "", "", "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    MSaveFileDialog = std::make_unique<wxFileDialog>(Frame, "Save File", "", "", "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
}

IoFile::IoFile(): MOpenFileDialog(nullptr), MSaveFileDialog(nullptr)
{
}

IoFile::~IoFile() = default;

wxString IoFile::openFile() const
{
    if (MOpenFileDialog->ShowModal() == wxID_OK)
    {
        return MOpenFileDialog->GetPath();
    }
    else
    {
        return "";
    }
}

wxString IoFile::saveFile() const
{
    if (MSaveFileDialog->ShowModal() == wxID_OK)
    {
        return MSaveFileDialog->GetPath();
    }
    else
    {
        return "";
    }
}

void IoFile::trimWhitespace(wxString& Str)
{
    if (const size_t First = Str.find_first_not_of(" \t"); wxString::npos != First)
    {
        const size_t Last = Str.find_last_not_of(" \t");
        Str = Str.substr(First, (Last - First + 1));
    }
}
