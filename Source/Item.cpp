#include "Item.h"
#include "ItemManager.h"

//”jŠü
void Item::Destroy()
{
	ItemManager::Instance().Remove(this);
}
