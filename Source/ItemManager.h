#ifndef __ITEM_MANAGER__
#define __ITEM_MANAGER__

#include "Defines.h"
#include "Item.h"
//#include "StaticPool.h"

#ifdef FONLINE_SERVER
class Critter;
class Map;
#endif

class ItemManager
{
private:
	bool isActive;
	ItemPtrMap gameItems;
	ProtoItem allProto[MAX_ITEM_PROTOTYPES]; // All
	ProtoItemVec typeProto[ITEM_MAX_TYPES]; // By type
	DWORD protoHash[ITEM_MAX_TYPES]; // Hash types protos
	char* protoScript[MAX_ITEM_PROTOTYPES];

	IniParser txtFile;
	int GetProtoValue(const char* key);
	bool SerializeTextProto(bool save, ProtoItem& proto_item, FILE* f, ProtoItemVec* protos);

public:
	ItemPtrMap& GetGameItems(){return gameItems;}
	DWORD GetItemsCount(){return gameItems.size();}
	ProtoItemVec& GetProtos(int type){return typeProto[type];}
	DWORD GetProtosHash(int type){return protoHash[type];}

	bool Init();
	void Finish();
	bool IsInit(){return isActive;};

#if defined(FONLINE_SERVER) || defined(FONLINE_OBJECT_EDITOR)
	bool LoadProtos();
	bool LoadProtos(ProtoItemVec& protos, const char* full_path);
#endif

#ifdef FONLINE_OBJECT_EDITOR
	void SaveProtos(const char* full_path);
	void SaveProtos(ProtoItemVec& protos, const char* full_path);
#endif

	void ParseProtos(ProtoItemVec& protos);
	void PrintProtosHash();
	ProtoItem* GetProtoItem(WORD pid);
	void GetAllProtos(ProtoItemVec& protos);
	bool IsInitProto(WORD pid);
	const char* GetProtoScript(WORD pid);
	void ClearProtos(BYTE type = 0xFF); // 0xFF - All
	void ClearProto(WORD pid);

#ifdef FONLINE_SERVER
	
#endif

#ifdef FONLINE_SERVER
private:
	// StaticPool<Item,1000000> itemsPool;
	DwordVec itemToDelete;
	DWORD lastItemId;

public:
	void SaveAllItemsFile(void(*save_func)(void*,size_t));
	bool LoadAllItemsFile(FILE* f);
	bool CheckProtoFunctions();
	void RunInitScriptItems();

	Item* CreateItem(WORD pid, DWORD count, DWORD item_id = 0);
	Item* GetItem(DWORD item_id);
	void EraseItem(DWORD item_id);
	void FullEraseItemIt(ItemPtrMapIt& it);

	void ClearAllItems();
	void ItemToGarbage(Item* item, int from);
	void ItemGarbager(DWORD cycle_tick);

	void NotifyChangeItem(Item* item);

	void EraseItemHolder(Item* item);
	void MoveItem(Item* item, DWORD count, Critter* to_cr);
	void MoveItem(Item* item, DWORD count, Map* to_map, WORD to_hx, WORD to_hy);
	void MoveItem(Item* item, DWORD count, Item* to_cont, DWORD stack_id);
	void DeleteItem(Item* item);

	Item* AddItemContainer(Item* cont, WORD pid, DWORD count, DWORD stack_id);
	Item* AddItemCritter(Critter* cr, WORD pid, DWORD count);
	bool SubItemCritter(Critter* cr, WORD pid, DWORD count, ItemPtrVec* erased_items = NULL);
	bool SetItemCritter(Critter* cr, WORD pid, DWORD count);
	bool MoveItemCritters(Critter* from_cr, Critter* to_cr, DWORD item_id, DWORD count);
	bool MoveItemCritterToCont(Critter* from_cr, Item* to_cont, DWORD item_id, DWORD count, DWORD stack_id);
	bool MoveItemCritterFromCont(Item* from_cont, Critter* to_cr, DWORD item_id, DWORD count);
	bool MoveItemsContainers(Item* from_cont, Item* to_cont, DWORD from_stack_id, DWORD to_stack_id);
	bool MoveItemsContToCritter(Item* from_cont, Critter* to_cr, DWORD stack_id);
#endif

#ifdef ITEMS_STATISTICS
// Items statistics
private:
	__int64 itemCount[MAX_ITEM_PROTOTYPES];

public:
	void AddItemStatistics(WORD pid, DWORD val);
	void SubItemStatistics(WORD pid, DWORD val);
	__int64 GetItemStatistics(WORD pid);
	string GetItemsStatistics();
#endif

	ItemManager():isActive(false){MEMORY_PROCESS(MEMORY_STATIC,sizeof(ItemManager));};
};

extern ItemManager ItemMngr;

#endif // __ITEM_MANAGER__