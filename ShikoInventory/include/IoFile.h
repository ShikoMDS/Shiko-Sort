#pragma once

#include <wx/wx.h>


class IoFile
{
public:
    explicit IoFile(wxFrame* Frame);
    IoFile();
    ~IoFile();

    [[nodiscard]] wxString openFile() const;
    [[nodiscard]] wxString saveFile() const;

    static void trimWhitespace(wxString& Str);

private:
    std::unique_ptr<wxFileDialog> MOpenFileDialog;
    std::unique_ptr<wxFileDialog> MSaveFileDialog;
};