#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Inventory.h"
#include "IoFile.h"


class Gui final : public wxFrame
{
public:
    explicit Gui(const wxString& Title);

private:

    Inventory MInventory;
    IoFile MFileManager;
    wxFileDialog* MOpenFileDialog{};
    wxFileDialog* MSaveFileDialog{};
    void createUi();

    void populateInventoryList() const;

    void bindEventHandlers();

    // Add event handlers
    void onLoadButton(wxCommandEvent& Event);
    void onSaveButton(wxCommandEvent& Event);
    void onAddButton(wxCommandEvent& Event);
    void onDelButton(wxCommandEvent& Event);
    void onClearButton(wxCommandEvent& Event);
    void onEditButton(wxCommandEvent& Event);
    void onSortButton(wxCommandEvent& Event);

    wxListCtrl* MInventoryPanel{};
    wxButton* MLoadButton{};
    wxButton* MSaveButton{};
    wxStaticText* MIndexLabel{};
    wxSpinCtrl* MIndexBox{};
    wxTextCtrl* MNameBox{};
    wxChoice* MTypeDropdown{};
    wxTextCtrl* MPriceBox{};
    wxStaticText* MQtyLabel{};
    wxSpinCtrl* MQtySpinner{};
    wxButton* MAddButton{};
    wxButton* MClearButton{};
    wxButton* MDelButton{};
    wxStaticText* MSortLabel{};
    wxChoice* MSortDropdown{};
    wxRadioBox* MSortBox{};
    wxButton* MSortButton{};
    wxButton* MEditButton{};

};

