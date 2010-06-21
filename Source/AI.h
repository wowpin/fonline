#ifndef __AI__
#define __AI__

#include "Common.h"

#define BAGS_FILE_NAME            "Bags.cfg"
#define MAX_NPC_BAGS              (50)
#define MAX_NPC_BAGS_PACKS        (20)
#define NPC_GO_HOME_WAIT_TICK     (Random(4000,6000))

#define AI_PLANE_MISC             (0)
#define AI_PLANE_ATTACK           (1)
#define AI_PLANE_WALK             (2)
#define AI_PLANE_PICK             (3)
#define AI_PLANE_PATROL           (4)
#define AI_PLANE_COURIER          (5)

#define AI_PLANE_MISC_PRIORITY    (10)
#define AI_PLANE_ATTACK_PRIORITY  (50)
#define AI_PLANE_WALK_PRIORITY    (20)
#define AI_PLANE_PICK_PRIORITY    (35)
#define AI_PLANE_PATROL_PRIORITY  (25)
#define AI_PLANE_COURIER_PRIORITY (30)

struct AIDataPlane
{
	int Type;
	DWORD Priority;
	int Identifier;
	DWORD IdentifierExt;
	AIDataPlane* ChildPlane;
	bool IsMove;

	union
	{
		struct
		{
			bool IsRun;
			DWORD WaitMinute;
			int ScriptBindId;
		} Misc;

		struct
		{
			bool IsRun;
			DWORD TargId;
			int MinHp;
			bool IsGag;
			WORD GagHexX,GagHexY;
			WORD LastHexX,LastHexY;
		} Attack;

		struct
		{
			bool IsRun;
			WORD HexX;
			WORD HexY;
			BYTE Dir;
			BYTE Cut;
		} Walk;

		struct
		{
			bool IsRun;
			WORD HexX;
			WORD HexY;
			WORD Pid;
			DWORD UseItemId;
			bool ToOpen;
		} Pick;

		struct 
		{
			DWORD Buffer[8];
		} Buffer;
	};

	struct 
	{
		DWORD PathNum;
		DWORD Iter;
		bool IsRun;
		DWORD TargId;
		WORD HexX;
		WORD HexY;
		BYTE Cut;
		BYTE Trace;
	} Move;

	AIDataPlane* GetCurPlane(){return ChildPlane?ChildPlane->GetCurPlane():this;}
	bool IsSelfOrHas(int type){return Type==type || (ChildPlane?ChildPlane->IsSelfOrHas(type):false);}
	DWORD GetChildIndex(AIDataPlane* child){DWORD index=0; for(AIDataPlane* child_=this;child_;index++) {if(child_==child) break; else child_=child_->ChildPlane;} return index;}
	DWORD GetChildsCount(){DWORD count=0; AIDataPlane* child=ChildPlane; for(;child;count++,child=child->ChildPlane); return count;}
	void DeleteLast(){if(ChildPlane){if(ChildPlane->ChildPlane) ChildPlane->DeleteLast(); else SAFEREL(ChildPlane);}}

	AIDataPlane* GetCopy()
	{
		AIDataPlane* copy=new AIDataPlane(Type,Priority);
		if(!copy) return NULL;
		memcpy(copy->Buffer.Buffer,Buffer.Buffer,sizeof(Buffer.Buffer));
		AIDataPlane* result=copy;
		AIDataPlane* plane_child=ChildPlane;
		while(plane_child)
		{
			copy->ChildPlane=new AIDataPlane(plane_child->Type,plane_child->Priority);
			if(!copy->ChildPlane) return NULL;
			copy->ChildPlane->Assigned=true;
			memcpy(copy->ChildPlane->Buffer.Buffer,plane_child->Buffer.Buffer,sizeof(plane_child->Buffer.Buffer));
			plane_child=plane_child->ChildPlane;
			copy=copy->ChildPlane;
		}
		return result;
	}

	bool Assigned;
	int RefCounter;
	void AddRef(){RefCounter++;}
	void Release(){RefCounter--; if(!RefCounter) delete this;}
	AIDataPlane(DWORD type, DWORD priority):Type(type),Priority(priority),Identifier(0),IdentifierExt(0),ChildPlane(NULL),IsMove(false),Assigned(false),RefCounter(1){ZeroMemory(&Buffer,sizeof(Buffer));ZeroMemory(&Move,sizeof(Move)); MEMORY_PROCESS(MEMORY_NPC_PLANE,sizeof(AIDataPlane));}
	~AIDataPlane(){SAFEREL(ChildPlane); MEMORY_PROCESS(MEMORY_NPC_PLANE,-(int)sizeof(AIDataPlane));}
	private: AIDataPlane(){} // Disable default constructor
};
typedef vector<AIDataPlane*> AIDataPlaneVec;
typedef vector<AIDataPlane*>::iterator AIDataPlaneVecIt;

class NpcBagItem
{
public:
	DWORD ItemPid;
	DWORD MinCnt;
	DWORD MaxCnt;
	DWORD ItemSlot;

	NpcBagItem():ItemPid(0),MinCnt(0),MaxCnt(0),ItemSlot(SLOT_INV){}
	NpcBagItem(const NpcBagItem& r):ItemPid(r.ItemPid),MinCnt(r.MinCnt),MaxCnt(r.MaxCnt),ItemSlot(r.ItemSlot){}
};
typedef vector<NpcBagItem> NpcBagItems;
typedef vector<NpcBagItems> NpcBagCombination;
typedef vector<NpcBagCombination> NpcBag;
typedef vector<NpcBag> NpcBagVec;
typedef map<string,NpcBagCombination> StringNpcBagCombMap;

/************************************************************************/
/*                                                                      */
/************************************************************************/

class NpcAIMngr
{
public:
	bool Init();
	void Finish();

	NpcBag& GetBag(DWORD num);

private:
	NpcBagVec npcBags;
	bool LoadNpcBags();
};
extern NpcAIMngr AIMngr;


// Plane begin/end/run reasons
	// Begin
#define REASON_GO_HOME              (10)
#define REASON_FOUND_IN_ENEMY_STACK (11)
#define REASON_FROM_DIALOG          (12)
#define REASON_FROM_SCRIPT          (13)
#define REASON_RUN_AWAY             (14)
	// End
#define REASON_SUCCESS              (30)
#define REASON_HEX_TOO_FAR          (31)
#define REASON_HEX_BUSY             (32)
#define REASON_HEX_BUSY_RING        (33)
#define REASON_DEADLOCK             (34)
#define REASON_TRACE_FAIL           (35)
#define REASON_POSITION_NOT_FOUND   (36)
#define REASON_FIND_PATH_ERROR      (37)
#define REASON_CANT_WALK            (38)
#define REASON_TARGET_DISAPPEARED   (39)
#define REASON_USE_ITEM_NOT_FOUND   (40)
#define REASON_GAG_CRITTER          (41)
#define REASON_GAG_DOOR             (42)
#define REASON_GAG_ITEM             (43)
#define REASON_NO_UNARMED           (44)
	// Run
#define REASON_ATTACK_TARGET        (50)
#define REASON_ATTACK_WEAPON        (51)
#define REASON_ATTACK_DISTANTION    (52)
#define REASON_ATTACK_USE_AIM       (53)

#endif // __AI__