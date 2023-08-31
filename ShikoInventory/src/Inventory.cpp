#include <wx/wx.h>
#include <wx/textfile.h>
#include "Inventory.h"

Inventory::Node::Node(const Item& NewItem, Node* NextItem, Node* PrevItem)
{
	Data = NewItem;
	Previous = PrevItem;
	Next = NextItem;
}

Inventory::Inventory()
{
	MHead = nullptr;
	MTail = nullptr;
	MSize = 0;
}

Inventory::~Inventory()
{
	// Destructor deletes all nodes to prevent memory leaks
	// Delete from the front and pass through to next front
	const Node* Select = MHead; // Sets the deletion to head of list
	while (Select != nullptr) // Runs through the list until all are deleted and head points to null
	{
		const Node* ToDelete = Select; // Stores current head to ToDelete variable
		Select = Select->Next; // Passes next item as the next head before deleting 
		delete ToDelete; // Deletes stored variable to remove the head and allows the passed item to be the next head
	}
}

wxString Inventory::itemTypeToString(const ItemType Type)
{
	switch (Type)
	{
	case ItemType::Weapon:
		return "Weapon";
	case ItemType::Armor:
		return "Armor";
	case ItemType::Consumable:
		return "Consumable";
	case ItemType::Utility:
		return "Utility";
	}
}

ItemType Inventory::stringToEnum(const wxString& TypeStr)
	{
		if (TypeStr == "Armor")
			return ItemType::Armor;
		if (TypeStr == "Weapon")
			return ItemType::Weapon;
		if (TypeStr == "Consumable")
			return ItemType::Consumable;
		if (TypeStr == "Utility")
			return ItemType::Utility;
		wxLogDebug(wxT("Comparing TypeStr: ") + TypeStr);
	};

void Inventory::loadFile(const wxString& Filename)
{
	if (!Filename.empty())
	{
		wxTextFile File;
		if (File.Open(Filename))
		{
			wxString Line;
			Line = File.GetFirstLine(); // Skip the header line
			while (!File.Eof())
			{
				Line = File.GetNextLine();
				if (!Line.IsEmpty())
				{
					Line.Trim(); // Trim leading and trailing whitespace from the line

					// "//" delimiters are parsed to find line segments
					const size_t Pos0 = Line.find("//", 0); // Finds first "//" - (Name) > First position (1) < (2)
					const size_t Pos1 = Line.find("//", Pos0 + 1); // Finds second "//" - (Type) > Second position (2) < (3)
					const size_t Pos2 = Line.find("//", Pos1 + 1); // Finds third "//" - (Price) > Third Position (3) < (Quantity)

					if (Pos0 != wxString::npos && Pos1 != wxString::npos && Pos2 != wxString::npos)
					{
						wxString ReadName = Line.substr(0, Pos0 - 1).Trim();
						wxString ReadType = Line.substr(Pos0 + 3, Pos1 - Pos0 - 4).Trim();
						wxString ReadPrice = Line.substr(Pos1 + 3, Pos2 - Pos1 - 4).Trim();
						wxString ReadQuantity = Line.substr(Pos2 + 3).Trim();

						if (ReadType == wxT("Weapon") || ReadType == wxT("Armor") || ReadType == wxT("Consumable") || ReadType == wxT("Utility"))
						{
							double Price;
							int Quantity;

							if (ReadPrice.ToDouble(&Price) && Price >= 0 &&
								ReadQuantity.IsNumber())
							{
								if (ReadQuantity.ToInt(&Quantity) && Quantity > 0)
								{
									try
									{
										ItemType Type = stringToEnum(ReadType); // Assign the returned value to a variable
										if (!itemExists(ReadName))
										{
											Item NewItem(ReadName, Type, Price, Quantity); // Use the assigned variable
											addFileItem(NewItem);
										}
										else
										{
											wxLogError(wxT("Item duplicate found, already exists! Item not loaded."));
										}
									}
									catch (const std::exception& E)
									{
										wxLogError(wxString::FromAscii(E.what()));
									}
								}
								else
								{
									wxLogError(wxT("Invalid quantity value: ") + ReadQuantity + wxT(" - Item not loaded."));
								}
							}
							else
							{
								wxLogError(wxT("Invalid price value: ") + ReadPrice + wxT(" - Item not loaded."));
							}
						}
						else
						{
							wxLogError(wxT("Invalid item type: ") + ReadType + wxT(" - Item not loaded."));
						}
					}
				}
			}
			File.Close();
		}
		else
		{
			wxLogError(wxT("Error: Unable to open file."));
		}
	}
}


void Inventory::saveFile(const wxString& Filename) const
{
	if (!Filename.empty())
	{
		wxTextFile File;
		if (File.Create(Filename))
		{
			File.AddLine(wxT("NAME // TYPE // PRICE // QUANTITY"));

			const Node* CurrentNode = getHead(); // Assuming Node is a nested class within Inventory
			while (CurrentNode)
			{
				const Item& CurrentItem = CurrentNode->Data;
				File.AddLine(CurrentItem.getName() + wxT(" // ") +
					wxString(itemTypeToString(CurrentItem.getType())) + wxT(" // ") +
					wxString::Format(wxT("%.2f"), CurrentItem.getPrice()) + wxT(" // ") +
					wxString::Format(wxT("%d"), CurrentItem.getQuantity()));

				CurrentNode = CurrentNode->Next;
			}

			if (File.Write())
			{
				std::cout << "Items saved to " << Filename << std::endl;
			}
			else
			{
				wxLogError(wxT("Error: Unable to write to file."));
			}
		}
		else
		{
			wxLogError(wxT("Error: Unable to create file."));
		}
	}
	else
	{
		wxLogError(wxT("Error: No filename provided."));
	}
}

Inventory::Node* Inventory::findItem(const wxString& Name) const
{
	Node* Select = MHead; // Start at the head to run through the list

	while (Select)
	{
		if (Select->Data.getName() == Name)
		{
			return Select; // Return the memory address of the found node
		}
		Select = Select->Next; // Pass through next item
	}

	return nullptr; // End of list - item by name not found
}

bool Inventory::itemExists(const wxString& Name) const
{
	return findItem(Name) != nullptr; // If name exists, return true
}

void Inventory::addFileItem(const Item& NewItem)
{
	// Check if an item with the same name already exists
	if (itemExists(NewItem.getName()))
	{
		wxLogError(wxT("An item with the same name already exists. Item not loaded."));
		return;
	}

	// Create a new Node to hold the Item and set its pointers
	const auto NewNode = new Node(NewItem, nullptr, MTail);

	if (MTail) // Sends the new node to the end of the list
	{
		MTail->Next = NewNode;
	}
	else
	{
		MHead = NewNode; // If the list was empty, set the head to the new node
	}

	MTail = NewNode; // Update the tail to the new node

	MSize++; // Increment the size of the inventory

	wxLogMessage(wxT("Item added to inventory."));
}

bool Inventory::addItem(const int Index, const Item& NewItem)
{
	if (Index < 0 || Index > MSize)
	{
		return false; // Invalid index, can't insert
	}

	if (!itemExists(NewItem.getName()))
	{
		// Create a new Node to hold the Item
		const auto NewNode = new Node(NewItem, nullptr, nullptr);

		if (!MHead)
		{
			// Empty list, set both MHead and MTail to the new node
			MHead = NewNode;
			MTail = NewNode;
		}
		else if (Index == 0)
		{
			// Insert at the beginning
			NewNode->Next = MHead;
			MHead->Previous = NewNode;
			MHead = NewNode;
		}
		else if (Index == MSize)
		{
			// Insert at the end
			NewNode->Previous = MTail;
			MTail->Next = NewNode;
			MTail = NewNode;
		}
		else
		{
			// Insert at a middle index
			Node* Temp = MHead;
			for (int I = 0; I < Index - 1; ++I)
			{
				Temp = Temp->Next;
			}
			NewNode->Next = Temp->Next;
			NewNode->Previous = Temp;
			Temp->Next->Previous = NewNode;
			Temp->Next = NewNode;
		}

		MSize++;
		return true; // Return true to indicate successful insertion
	}
	else
	{
		return false; // Return false to indicate item not inserted
	}
}

void Inventory::removeItem(const size_t Index)
{
	if (Index < 1 || Index > MSize) {
		wxLogError(wxT("Invalid index. Item not removed."));
		return;
	}

	const Node* ItemToRemove = nullptr;

	if (Index == 1) {
		ItemToRemove = MHead;
		MHead = MHead->Next;
		if (MHead) {
			MHead->Previous = nullptr;
		}
		else {
			MTail = nullptr; // List becomes empty
		}
	}
	else if (Index == MSize) {
		ItemToRemove = MTail;
		MTail = MTail->Previous;
		if (MTail) {
			MTail->Next = nullptr;
		}
		else {
			MHead = nullptr; // List becomes empty
		}
	}
	else {
		const Node* Current = MHead;
		for (size_t I = 1; I < Index; ++I) {
			Current = Current->Next;
		}
		Node* Prev = Current->Previous;
		Node* Next = Current->Next;
		Prev->Next = Next;
		Next->Previous = Prev;
		ItemToRemove = Current;
	}

	if (ItemToRemove) {
		delete ItemToRemove;
		MSize--;
	}
}

void Inventory::accessItem(const wxString& Name, Item& NewItem) const
{
	const Node* ItemNode = findItem(Name);
	if (ItemNode) {
		NewItem = ItemNode->Data; // Copy the item details to NewItem
	}
	// Else handle the case when the item is not found
}

void Inventory::clearInventory()
{
	Node* Current = MHead;
	while (Current)
	{
		const Node* Temp = Current;
		Current = Current->Next;
		delete Temp;
	}
	MHead = nullptr;
	MTail = nullptr;
	MSize = 0;
}

void Inventory::quickSortByName(const bool Ascending)
{
	quickSortByNameRecursive(MHead, MTail, Ascending);
}

void Inventory::quickSortByType(const bool Ascending)
{
	quickSortByTypeRecursive(MHead, MTail, Ascending);
}

void Inventory::quickSortByPrice(const bool Ascending)
{
	quickSortByPriceRecursive(MHead, MTail, Ascending);
}

void Inventory::quickSortByQuantity(const bool Ascending)
{
	quickSortByQuantityRecursive(MHead, MTail, Ascending);
}

Inventory::Node* Inventory::getHead() const
{
	return MHead;
}

Inventory::Node* Inventory::getTail() const
{
	return MTail;
}

size_t Inventory::getSize() const
{
	return MSize;
}

void Inventory::quickSortByNameRecursive(Node* Left, Node* Right, const bool Ascending)
{
	if (Left && Right && Left != Right && Left != Right->Next)
	{
		if (const Node* Pivot = partitionByName(Left, Right, Ascending); Pivot != nullptr)
		{
			quickSortByNameRecursive(Left, Pivot->Previous, Ascending);
			quickSortByNameRecursive(Pivot->Next, Right, Ascending);
		}
	}
}


void Inventory::quickSortByTypeRecursive(Node* Left, Node* Right, const bool Ascending)
{
	if (Left && Right && Left != Right && Left != Right->Next)
	{
		if (const Node* Pivot = partitionByType(Left, Right, Ascending); Pivot != nullptr)
		{
			quickSortByTypeRecursive(Left, Pivot->Previous, Ascending);
			quickSortByTypeRecursive(Pivot->Next, Right, Ascending);
		}
	}
}

void Inventory::quickSortByPriceRecursive(Node* Left, Node* Right, const bool Ascending)
{
	if (Left && Right && Left != Right && Left != Right->Next)
	{
		if (const Node* Pivot = partitionByPrice(Left, Right, Ascending); Pivot != nullptr)
		{
			quickSortByPriceRecursive(Left, Pivot->Previous, Ascending);
			quickSortByPriceRecursive(Pivot->Next, Right, Ascending);
		}
	}
}

void Inventory::quickSortByQuantityRecursive(Node* Left, Node* Right, const bool Ascending)
{
	if (Left && Right && Left != Right && Left != Right->Next)
	{
		if (const Node* Pivot = partitionByQuantity(Left, Right, Ascending); Pivot != nullptr)
		{
			quickSortByQuantityRecursive(Left, Pivot->Previous, Ascending);
			quickSortByQuantityRecursive(Pivot->Next, Right, Ascending);
		}
	}
}

Inventory::Node* Inventory::partitionByName(Node* Left, Node* Right, const bool Ascending) const
{
	const wxString& PivotName = Right->Data.getName();
	Node* I = Left->Previous;

	for (Node* J = Left; J != Right && J != nullptr; J = J->Next)
	{
		if ((Ascending && J->Data.getName() <= PivotName) || (!Ascending && J->Data.getName() >= PivotName))
		{
			I = (I == nullptr) ? Left : I->Next;
			if (I != nullptr)
			{
				std::swap(I->Data, J->Data);
			}
		}
	}

	if (I != nullptr)
	{
		I = I->Next;
		if (I != nullptr)
		{
			std::swap(I->Data, Right->Data);
		}
	}
	return I;
}

static std::unordered_map<ItemType, std::string> ItemTypeToString =
{
	{ItemType::Weapon, "Weapon"},
	{ItemType::Armor, "Armor"},
	{ItemType::Consumable, "Consumable"},
	{ItemType::Utility, "Utility"}
};

Inventory::Node* Inventory::partitionByType(Node* Left, Node* Right, const bool Ascending) const
{
	const std::string& PivotTypeStr = ItemTypeToString[Right->Data.getType()];
	Node* I = Left->Previous;

	for (Node* J = Left; J != Right && J != nullptr; J = J->Next)
	{
		if (const std::string& JTypeStr = ItemTypeToString[J->Data.getType()]; (Ascending && JTypeStr <= PivotTypeStr)
			|| (!Ascending && JTypeStr >= PivotTypeStr))
		{
			I = (I == nullptr) ? Left : I->Next;
			if (I != nullptr)
			{
				std::swap(I->Data, J->Data);
			}
		}
	}

	if (I != nullptr)
	{
		I = I->Next;
		if (I != nullptr)
		{
			std::swap(I->Data, Right->Data);
		}
	}
	return I;
}

Inventory::Node* Inventory::partitionByPrice(Node* Left, Node* Right, const bool Ascending) const
{
	const double PivotPrice = Right->Data.getPrice();
	Node* I = Left->Previous;

	for (Node* J = Left; J != Right && J != nullptr; J = J->Next)
	{
		if ((Ascending && J->Data.getPrice() <= PivotPrice) || (!Ascending && J->Data.getPrice() >= PivotPrice))
		{
			I = (I == nullptr) ? Left : I->Next;
			if (I != nullptr)
			{
				std::swap(I->Data, J->Data);
			}
		}
	}

	if (I != nullptr)
	{
		I = I->Next;
		if (I != nullptr)
		{
			std::swap(I->Data, Right->Data);
		}
	}
	return I;
}

Inventory::Node* Inventory::partitionByQuantity(Node* Left, Node* Right, const bool Ascending) const
{
	const int PivotQuantity = Right->Data.getQuantity();
	Node* I = Left->Previous;

	for (Node* J = Left; J != Right && J != nullptr; J = J->Next)
	{
		if ((Ascending && J->Data.getQuantity() <= PivotQuantity) || (!Ascending && J->Data.getQuantity() >=
			PivotQuantity))
		{
			I = (I == nullptr) ? Left : I->Next;
			if (I != nullptr)
			{
				std::swap(I->Data, J->Data);
			}
		}
	}

	if (I != nullptr)
	{
		I = I->Next;
		if (I != nullptr)
		{
			std::swap(I->Data, Right->Data);
		}
	}
	return I;
}
