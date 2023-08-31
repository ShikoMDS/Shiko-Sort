#include "Gui.h"

#include <wx/file.h>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>

#include "IoFile.h"
#include "Inventory.h"


Gui::Gui(const wxString& Title) : wxFrame(nullptr, wxID_ANY, Title, wxDefaultPosition, wxDefaultSize,
                                          wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{

    createUi();
	bindEventHandlers();
    
}

void Gui::createUi()
{
	// Change the background color to pink
	wxWindowBase::SetBackgroundColour(wxColour(255, 192, 203)); // RGB values for pink

	constexpr int WindowWidth = 1600;
	constexpr int WindowHeight = 900;
	constexpr int Gap = 20;

	// Create a list control for the inventory
	MInventoryPanel = new wxListCtrl(this, wxID_ANY, wxPoint(Gap, Gap), wxSize(1325, 810), wxLC_REPORT);
	MInventoryPanel->InsertColumn(0, "Item No.");
	MInventoryPanel->InsertColumn(1, "Name");
	MInventoryPanel->InsertColumn(2, "Type");
	MInventoryPanel->InsertColumn(3, "Price");
	MInventoryPanel->InsertColumn(4, "Quantity");

	// Set initial column widths (adjust the values as needed)
	MInventoryPanel->SetColumnWidth(0, 100); // Item Number column width
	MInventoryPanel->SetColumnWidth(1, 500); // Name column width
	MInventoryPanel->SetColumnWidth(2, 100); // Type column width
	MInventoryPanel->SetColumnWidth(3, 100); // Price column width
	MInventoryPanel->SetColumnWidth(4, 100); // Quantity column width

	// Calculate positions based on the gap
	constexpr int InputX = Gap;
	constexpr int InputY = WindowHeight - Gap - 30; // 30 is the estimated control height

	// Input controls
	MIndexLabel = new wxStaticText(this, wxID_ANY, "Item No.:", wxPoint(InputX, InputY + 3));
	MIndexBox = new wxSpinCtrl(this, wxID_ANY, "", wxPoint(InputX + 80, InputY), wxSize(50, -1));
	// Set the range of the item index spinner based on the number of items in inventory
	MIndexBox->SetRange(1, MInventory.getSize() + 1); // Set the range from 1 to size + 1

	MNameBox = new wxTextCtrl(this, wxID_ANY, "<Item Name>", wxPoint(InputX + 150, InputY), wxSize(200, -1)); // Larger width
	MTypeDropdown = new wxChoice(this, wxID_ANY, wxPoint(InputX + 360, InputY), wxSize(100, -1)); // Adjusted position
	MTypeDropdown->Append("<Item Type>");
	MTypeDropdown->Append("Weapon");
	MTypeDropdown->Append("Armor");
	MTypeDropdown->Append("Consumable");
	MTypeDropdown->Append("Utility");
	MTypeDropdown->SetSelection(0);
	MPriceBox = new wxTextCtrl(this, wxID_ANY, "<Price>", wxPoint(InputX + 470, InputY), wxSize(100, -1), wxTE_RIGHT); // Adjusted position
	MQtyLabel = new wxStaticText(this, wxID_ANY, "Qty:", wxPoint(InputX + 580, InputY + 3)); // Adjusted position
	MQtySpinner = new wxSpinCtrl(this, wxID_ANY, "", wxPoint(InputX + 610, InputY), wxSize(50, -1)); // Adjusted position
	MAddButton = new wxButton(this, wxID_ANY, "Add", wxPoint(InputX + 670, InputY), wxSize(70, -1)); // Adjusted position


	// Load and Save buttons
	constexpr int SaveButtonWidth = 100;
	constexpr int LoadSaveGap = 10; // Adjust this value for the desired gap

	MSaveButton = new wxButton(this, wxID_ANY, "Save",
	                           wxPoint(WindowWidth - Gap - SaveButtonWidth, Gap), wxSize(SaveButtonWidth, -1));

	MLoadButton = new wxButton(this, wxID_ANY, "Load",
	                           wxPoint(MSaveButton->GetPosition().x - LoadSaveGap - SaveButtonWidth, Gap),
	                           wxSize(SaveButtonWidth, -1));

	// Clear button
	MClearButton = new wxButton(this, wxID_ANY, "Clear", wxPoint(WindowWidth - Gap - 100, InputY), wxSize(100, -1));

	// Delete and edit button
	constexpr int EditGap = 10; // Adjust this value for the desired gap
	constexpr int ButtonWidth = 100;     // Adjust this value for the desired width

	// Calculate the position of the DeleteButton and EditButton
	const int DeleteButtonX = MInventoryPanel->GetPosition().x + MInventoryPanel->GetSize().GetWidth() -
		ButtonWidth;
	constexpr int DeleteButtonY = InputY;

	const int EditButtonX = DeleteButtonX - EditGap - ButtonWidth;
	constexpr int EditButtonY = InputY;

	// Create the EditButton and DeleteButton
	MEditButton = new wxButton(this, wxID_ANY, "Edit",
		wxPoint(EditButtonX, EditButtonY), wxSize(ButtonWidth, -1));

	MDelButton = new wxButton(this, wxID_ANY, "Delete",
		wxPoint(DeleteButtonX, DeleteButtonY), wxSize(ButtonWidth, -1));


	// Sort controls
	MSortLabel = new wxStaticText(this, wxID_ANY, "Sort by:", wxPoint(WindowWidth - 175, 350));
	MSortDropdown = new wxChoice(this, wxID_ANY, wxPoint(WindowWidth - 175, 370), wxSize(100, -1));
	MSortDropdown->Append("Name");
	MSortDropdown->Append("Type");
	MSortDropdown->Append("Price");
	MSortDropdown->Append("Quantity");
	MSortDropdown->SetSelection(0);

	wxArrayString Order;
	Order.Add("Ascending");
	Order.Add("Descending");

	MSortBox = new wxRadioBox(this, wxID_ANY, "Order", wxPoint(WindowWidth - 175, 400), wxSize(100, -1),
	                                     Order, 1); // 3, wxRA_VERTICAL

	MSortButton = new wxButton(this, wxID_ANY, "Sort", wxPoint(WindowWidth - 175, 480), wxSize(100, -1));

	MOpenFileDialog = new wxFileDialog(this, _("Open File"), wxEmptyString, wxEmptyString,
		_("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	MSaveFileDialog = new wxFileDialog(this, _("Save File"), wxEmptyString, wxEmptyString,
		_("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
}

void Gui::populateInventoryList() const
{
	// Clear the existing items from the list control
	MInventoryPanel->DeleteAllItems();

	const Inventory::Node* CurrentNode = MInventory.getHead();
	int ItemNo = 1;

	while (CurrentNode)
	{
		const Item& CurrentItem = CurrentNode->Data;

		// Insert a new item into the list control and get its index
		const int Index = MInventoryPanel->InsertItem(MInventoryPanel->GetItemCount(), wxString::Format("%d", ItemNo));

		// Make sure the index is valid before setting item values
		if (Index != wxNOT_FOUND)
		{
			MInventoryPanel->SetItem(Index, 1, CurrentItem.getName());
			MInventoryPanel->SetItem(Index, 2, Inventory::itemTypeToString(CurrentItem.getType()));
			MInventoryPanel->SetItem(Index, 3, wxString::Format("%.2f", CurrentItem.getPrice()));
			MInventoryPanel->SetItem(Index, 4, wxString::Format("%d", CurrentItem.getQuantity()));
		}
		else
		{
			// Handle the case where the index is invalid
			wxLogError("Error inserting item into list control.");
		}

		CurrentNode = CurrentNode->Next;
		ItemNo++;
	}
}

void Gui::bindEventHandlers()
{
	MLoadButton->Bind(wxEVT_BUTTON, &Gui::onLoadButton, this);
	MSaveButton->Bind(wxEVT_BUTTON, &Gui::onSaveButton, this);
	MAddButton->Bind(wxEVT_BUTTON, &Gui::onAddButton, this);
	MDelButton->Bind(wxEVT_BUTTON, &Gui::onDelButton, this);
	MClearButton->Bind(wxEVT_BUTTON, &Gui::onClearButton, this);
	MEditButton->Bind(wxEVT_BUTTON, &Gui::onEditButton, this);
	MSortButton->Bind(wxEVT_BUTTON, &Gui::onSortButton, this);

}

void Gui::onLoadButton(wxCommandEvent& Event)
{
	if (MOpenFileDialog->ShowModal() == wxID_OK)
	{
		const wxString Filename = MOpenFileDialog->GetPath();
		MInventory.loadFile(Filename.ToStdString());
		wxLogMessage("Items loaded from %s", Filename);

		// Update and display the inventory list
		populateInventoryList();
	}
	else
	{
		wxLogMessage("Operation canceled or no file name entered.");
	}
}

void Gui::onSaveButton(wxCommandEvent& Event)
{
	if (MSaveFileDialog->ShowModal() == wxID_OK)
	{
		const wxString Filename = MSaveFileDialog->GetPath();
		MInventory.saveFile(Filename.ToStdString());
		wxLogMessage("Items saved to %s", Filename);
	}
	else
	{
		wxLogMessage("Operation canceled or no file name entered.");
	}
}

void Gui::onAddButton(wxCommandEvent& Event)
{
	// Get the input values from the controls
	const wxString Name = MNameBox->GetValue().Strip(wxString::both);
	const wxString TypeStr = MTypeDropdown->GetStringSelection();
	const wxString PriceStr = MPriceBox->GetValue();
	const int Quantity = MQtySpinner->GetValue();
	double Price;

	if (Name == wxT("<Item Name>") || Name.IsEmpty())
	{
		wxLogError(wxT("Please enter a valid item name! Item not added."));
		return;
	}

	// Check for the default selection
	if (TypeStr == wxT("<Item Type>"))
	{
		wxLogError(wxT("Please select a valid item type! Item not added."));
		return;
	}

	if (PriceStr == wxT("<Price>") || !PriceStr.ToDouble(&Price))
	{
		wxLogError(wxT("Invalid price value! Item not added."));
		return;
	}

	if (Price < 0)
	{
		wxLogError(wxT("Price cannot be negative! Item not added."));
		return;
	}

	if (Quantity < 1)
	{
		wxLogError(wxT("Quantity must be at least 1! Item not added."));
		return;
	}

	// Convert the wxString itemTypeStr to ItemType enum using the stringToEnum function
	const ItemType Type = MInventory.stringToEnum(TypeStr);

	// Create an Item object with the input values
	const Item NewItem(Name, Type, Price, Quantity);

	// Get the selected item index from the spinner
		int selectedIndex = MIndexBox->GetValue(); // No need to subtract 1

	// Ignore index 0
	if (selectedIndex == 0) {
		wxLogError(wxT("Invalid index! Item not added."));
		return;
	}

	// Convert to zero-based index for the inventory
	selectedIndex -= 1;

	// Try to insert the item into the inventory at the specified index
	if (MInventory.addItem(selectedIndex, NewItem))
	{
		// Clear input fields or update GUI as needed
		MNameBox->SetValue(wxT("<Item Name>"));
		MTypeDropdown->SetSelection(0); // Set to default selection
		MPriceBox->SetValue(wxT("<Price>"));
		MQtySpinner->SetValue(0); // Set to default value
		// Update the spinner's range and value
		const int NewSize = MInventory.getSize() + 1;
		MIndexBox->SetRange(1, NewSize); // Update the range
		MIndexBox->SetValue(selectedIndex + 2); // Set the spinner value to the next item number

		// Update or refresh the GUI as needed
		populateInventoryList();

		// Display a dialog box to inform the user
		const wxString Message = wxString::Format(wxT("Item \"%s\" has been added."), Name, selectedIndex);
		wxMessageBox(Message, wxT("Item Added"), wxOK | wxICON_INFORMATION, this);
	}
	else
	{
		wxLogError(wxT("Invalid index or index is out of bounds! Item not added."));
	}
}

void Gui::onDelButton(wxCommandEvent& Event)
{
	// Get the selected index from the list control
	const long SelectedIndex = MInventoryPanel->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (SelectedIndex != wxNOT_FOUND) {
		MInventory.removeItem(SelectedIndex + 1); // Index is 1-based
		// Update or refresh the GUI as needed
		populateInventoryList();
		wxMessageBox(wxT("Item removed successfully."), wxT("Item Removed"), wxOK | wxICON_INFORMATION, this);
	}
	else {
		wxMessageBox(wxT("Please select an item to remove."), wxT("No Item Selected"), wxOK | wxICON_WARNING, this);
	}
}

void Gui::onClearButton(wxCommandEvent& Event)
{
	if (MInventory.getSize() == 0)
	{
		// Display a message if inventory is already empty
		wxMessageBox(wxT("Inventory is already empty!"), wxT("Empty Inventory"), wxOK | wxICON_INFORMATION, this);
		return;
	}

	// Display a confirmation dialog
	const wxString Message = wxT("Are you sure you want to remove all items from the inventory?");
	const wxString Caption = wxT("Confirm Clear");
	constexpr int Style = wxYES_NO | wxICON_QUESTION;
	const int Result = wxMessageBox(Message, Caption, Style, this);

	if (Result == wxYES)
	{
		// User confirmed, clear the inventory
		MInventory.clearInventory();
		populateInventoryList(); // Update the GUI

		// Display a message to inform the user
		wxMessageBox(wxT("All items have been removed from the inventory."), wxT("Inventory Cleared"), wxOK | wxICON_INFORMATION, this);
	}
}

void Gui::onEditButton(wxCommandEvent& Event)
{

}

void Gui::onSortButton(wxCommandEvent& Event)
{
	const int SelectedSortIndex = MSortDropdown->GetSelection();
	const bool AscendingOrder = (MSortBox->GetSelection() == 0);

	if (SelectedSortIndex == 0)  // Name
	{
		MInventory.quickSortByName(AscendingOrder);
	}
	else if (SelectedSortIndex == 1)  // Type
	{
		MInventory.quickSortByType(AscendingOrder);
	}
	else if (SelectedSortIndex == 2)  // Price
	{
		MInventory.quickSortByPrice(AscendingOrder);
	}
	else if (SelectedSortIndex == 3)  // Quantity
	{
		MInventory.quickSortByQuantity(AscendingOrder);
	}

	// Update the inventory list display
	populateInventoryList();

	// Explicitly refresh and update the GUI elements
	MInventoryPanel->Refresh();
	MInventoryPanel->Update();
}
