#include "App.h"
#include "Gui.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    const auto MainFrame = new Gui("ShikoSort");
    MainFrame->SetClientSize(1600, 900);
    MainFrame->Center();
    MainFrame->Show();
    return true;
}

