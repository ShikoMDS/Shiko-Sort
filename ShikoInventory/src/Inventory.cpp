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


/* Each of these functions sorts the inventory by a different attribute: Name, Type, Price, or Quantity.

A lambda function is a small, anonymous function (function without a name) defined in the scope where it is used, often passed as an argument to other functions. In C++, lambda functions are a convenient way to define simple function objects.

In this case, each quickSort... function calls the general quickSort function, passing a lambda function that compares two Items by the desired attribute. The quickSort function uses this lambda to compare Items while sorting.

The syntax for a lambda function is: [captures](parameters) {body}. 
Here, captures is a list of variables from the surrounding scope that the lambda needs to access. parameters is the list of parameters the lambda takes, and body is the code the lambda executes. */

// Function to sort the inventory by name.
void Inventory::quickSortByName(const bool isAscending)
{
	// Call the general quickSort function, passing a lambda function that compares the names of two items.
	quickSort(MHead, MTail, isAscending, [](const Item& a, const Item& b) {return a.getName() < b.getName();});
}

// Function to sort the inventory by type.
void Inventory::quickSortByType(const bool isAscending)
{
	// Call the general quickSort function, passing a lambda function that compares the types of two items.
	quickSort(MHead, MTail, isAscending, [](const Item& a, const Item& b) {return a.getType() < b.getType();});
}

// Function to sort the inventory by price.
void Inventory::quickSortByPrice(const bool isAscending)
{
	// Call the general quickSort function, passing a lambda function that compares the prices of two items.
	quickSort(MHead, MTail, isAscending, [](const Item& a, const Item& b) {return a.getPrice() < b.getPrice();});
}

// Function to sort the inventory by quantity.
void Inventory::quickSortByQuantity(const bool isAscending)
{
	// Call the general quickSort function, passing a lambda function that compares the quantities of two items.
	quickSort(MHead, MTail, isAscending, [](const Item& a, const Item& b) {return a.getQuantity() < b.getQuantity();});
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

// Quick sort function for a doubly-linked list.
template <typename Compare>
void Inventory::quickSort(Node* Left, Node* Right, const bool isAscending, const Compare& compFunction)
{
	// Base condition: if either pointer is null, or if they point to the same node or adjacent nodes, return.
	if (Right != nullptr && Left != Right && Left != Right->next)
	{
		// Partition the list and get the partition node.
		const Node* partition = createPartition(Left, Right, isAscending, compFunction);
		
		// Sort the list to the left of the partition.
		quickSort(Left, partition->Previous, isAscending, compFunction);
		
		// Sort the list to the right of the partition.
		quickSort(partition->Next, Right, isAscending, compFunction);
	}
}

// Function to partition the doubly-linked list around a pivot element.
template <typename Compare>
Inventory::Node* Inventory::createPartition(Node* Left, Node* Right, const bool isAscending, const Compare& compFunction) const
{
	// Choose the data of the node pointed to by 'Right' as the pivot.
	const Item& pivot = Right->Data;
	
	// Initialize pointers for the partition logic.
	Node* leftIndex = Left->Previous;
	Node* rightIndex = Left;

	// Loop through each node and re-arrange the list so that all elements smaller
	// than the pivot come before it and all elements greater come after it.
	for (; rightIndex != Right; rightIndex = rightIndex->Next)
	{
		// The condition for swapping depends on whether sorting is ascending or descending. Uses the Lambda comparison function we passed in.
		if ((isAscending && compFunction(rightIndex->Data, pivot)) || (!isAscending && compFunction(pivot, rightIndex->Data)))
		{
			// Move the left index one step, taking care of the edge case.
			leftIndex = (leftIndex == nullptr) ? Left : leftIndex->Next;
			
			// Swap the current element with the left index.
			SwapItems(leftIndex, rightIndex);
		}
	}

	// Finally, swap the pivot with the left index so that the pivot is in its final sorted position.
	leftIndex = (leftIndex == nullptr) ? Left : leftIndex->Next;
	SwapItems(leftIndex, Right);

	// Return the node that is now at the pivot position.
	return leftIndex;
}

// Function to swap the data between two nodes.
void Inventory::swapNodes(Node* nodeA, Node* nodeB) const 
{
	// std::swap is a C++ standard library function that swaps two elements.
	std::swap(nodeA->Data, nodeB->Data); // Assuming that the 'Data' type supports swap, probably not, so have no fucking clue if this works but it is something, else create a temp item and swap data that way

	// You should preferably attempt to swap the nodes themself rather then the item data. This would then require you to fix the pointers of the nodes both left and right of the swapped nodes
}