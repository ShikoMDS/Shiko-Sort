#pragma once

#pragma once

#include "Item.h"

class Inventory
{
public:
	// Doubly-Linked list
	// Item/Node class doubly linked list data structure
	class Node
	{
	public:
		// Node holds 2 ends and a body for DLL
		explicit Node(const Item& NewItem, Node* NextItem, Node* PrevItem);
		Item Data; // Core
		Node* Previous; // Head
		Node* Next; // Tail
	};

	Inventory(); // Constructor
	~Inventory(); // Destructor


	static wxString itemTypeToString(ItemType Type);
	static ItemType stringToEnum(const wxString& TypeStr);
	// Load/Save functions
	void loadFile(const wxString& Filename);
	void saveFile(const wxString& Filename) const;

	[[nodiscard]] Node* findItem(const wxString& Name) const; // Find a selected item from index
	[[nodiscard]] bool itemExists(const wxString& Name) const; // Check if name exists - cannot duplicate
	void addFileItem(const Item& NewItem); // Add a new item to index
	bool addItem(int Index, const Item& NewItem);
	void removeItem(size_t Index); // Remove a selected item from index
	void accessItem(const wxString& Name, Item& NewItem) const; // Access and edit a selected item from index
	void clearInventory(); // Deletes all items from current inventory
	
	void quickSortByName(bool isAscending);
	void quickSortByType(bool isAscending);
	void quickSortByPrice(bool isAscending);
	void quickSortByQuantity(bool isAscending);

	[[nodiscard]] Node* getHead() const;
	[[nodiscard]] Node* getTail() const;
	[[nodiscard]] size_t getSize() const;

private:
	// Front and back end list
	Node* MHead;
	Node* MTail;
	// Index size 
	size_t MSize; // size_t array indexing/loop counting https://en.cppreference.com/w/c/types/size_t

	void quickSort(Node* Left, Node* Right, const bool isAscending, const auto& compFunction);
	Node* createPartition(Node* Left, Node* Right, bool Ascending, const auto& *compFunc) const;
	void swapNodes(Node* nodeA, Node* nodeB) const;
};

