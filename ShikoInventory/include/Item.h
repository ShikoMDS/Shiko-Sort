/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2023 Media Design School

File Name : Item.h
Description : Declarations for item object related functions
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#pragma once

#include <wx/wx.h>

enum class ItemType
{
	Weapon,
	Armor,
	Consumable,
	Utility
};

class Item
{
public:
	explicit Item(const wxString& Name, ItemType Type, double Price, int Quantity);
	Item();
	~Item();

	// Mutators
	void setName(const wxString& Name);
	void setType(ItemType Type);
	void setPrice(double Price);
	void setQuantity(int Quantity);

	// Accessors
	[[nodiscard]] wxString getName() const;
	[[nodiscard]] ItemType getType() const;
	[[nodiscard]] double getPrice() const;
	[[nodiscard]] int getQuantity() const;
	

private:
	wxString MName;
	ItemType MType;
	double MPrice{};
	int MQuantity{};
};


// Item value handling
