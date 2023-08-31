/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2023 Media Design School

File Name : Item.cpp
Description : Implementations for item declarations
Author : Shikomisen (Ayoub Ahmad)
Mail : ayoub.ahmad@mds.ac.nz
**************************************************************************/

#include "Item.h"

Item::Item(const wxString& Name, const ItemType Type, const double Price, const int Quantity)
{
	MName = Name;
	MType = Type;
	MPrice = Price;
	MQuantity = Quantity;
}

Item::Item()
{
}

Item::~Item() = default;


// Mutators
void Item::setName(const wxString& Name)
{
	MName = Name;
}

void Item::setType(const ItemType Type)
{
	MType = Type;
}

void Item::setPrice(const double Price)
{
	MPrice = Price;
}

void Item::setQuantity(const int Quantity)
{
	MQuantity = Quantity;
}


// Accessors
wxString Item::getName() const
{
	return MName;
}

ItemType Item::getType() const
{
	return MType;
}

double Item::getPrice() const
{
	return MPrice;
}

int Item::getQuantity() const
{
	return MQuantity;
}


// Item values are handled here
