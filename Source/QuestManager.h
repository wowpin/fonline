#ifndef __QUEST_MANAGER__
#define __QUEST_MANAGER__

#include "Common.h"
#include "Text.h"

#define QUEST_MUL	1000

struct Quest
{
	WORD num;
	string str;
	string info;
	bool isInfo;

	bool operator==(const WORD& _num){return _num==num;}
	Quest(DWORD _num, string _info):num(_num),info(_info),isInfo(false){}
};
typedef vector<Quest> QuestVec;
typedef vector<Quest>::iterator QuestVecIt;
typedef vector<Quest>::value_type QuestVecVal;

class QuestTab 
{
private:
	QuestVec quests;
	string text;
	FOMsg* msg;

	void ReparseText()
	{
		text="";

		char str[128];
		for(DWORD i=0;i<quests.size();++i)
		{
			sprintf(str,msg->GetStr(STR_QUEST_NUMBER),i+1);

			text+=str;
			text+=quests[i].info;
			text+="\n";
			text+=msg->GetStr(STR_QUEST_PROCESS);
			text+=quests[i].str;
			text+="\n\n";
		}
	}

public:
	bool IsEmpty(){return quests.empty();}
	Quest* AddQuest(WORD num, string& info){quests.push_back(Quest(num,info)); return &quests[quests.size()-1]; ReparseText();}
	void RefreshQuest(WORD num, string& str){Quest* quest=GetQuest(num); if(!quest) return; quest->str=str; ReparseText();}
	Quest* GetQuest(WORD num){QuestVecIt it=std::find(quests.begin(),quests.end(),num); return it!=quests.end()?&(*it):NULL;}
	void EraseQuest(WORD num){QuestVecIt it=std::find(quests.begin(),quests.end(),num); if(it!=quests.end()) quests.erase(it); ReparseText();}
	QuestVec* GetQuests(){return &quests;}
	const char* GetText(){return text.c_str();}
	QuestTab(FOMsg* _msg):msg(_msg){}
};
typedef map<string, QuestTab, less<string> > QuestTabMap;
typedef map<string, QuestTab, less<string> >::iterator QuestTabMapIt;
typedef map<string, QuestTab, less<string> >::value_type QuestTabMapVal;

class QuestManager
{
private:
	FOMsg* msg;
	QuestTabMap tabs;

public:
	void Init(FOMsg* quest_msg)
	{
		msg=quest_msg;
	}

	void Clear()
	{
		tabs.clear();
	}

	void OnQuest(DWORD num)
	{
		// Split	
		WORD q_num=num/QUEST_MUL;
		WORD val=num%QUEST_MUL;

		// Check valid Name of Tab
		if(!msg->Count(STR_QUEST_MAP_(q_num))) return;

		// Get Name of Tab	
		string tab_name=string(msg->GetStr(STR_QUEST_MAP_(q_num)));

		// Try get Tab	
		QuestTab* tab=NULL;
		QuestTabMapIt it_tab=tabs.find(tab_name);
		if(it_tab!=tabs.end()) tab=&(*it_tab).second;

		// Try get Quest
		Quest* quest=NULL;
		if(tab) quest=tab->GetQuest(q_num);

		// Erase	quest
		if(!val)
		{
			if(tab)
			{
				tab->EraseQuest(q_num);
				if(tab->IsEmpty()) tabs.erase(tab_name);
			}
			return;
		}

		// Add Tab if not exists
		if(!tab) tab=&(*(tabs.insert(QuestTabMapVal(tab_name,QuestTab(msg)))).first).second;

		// Add Quest if not exists
		if(!quest) quest=tab->AddQuest(q_num,string(msg->GetStr(STR_QUEST_INFO_(q_num))));

		// Get name of quest
		tab->RefreshQuest(q_num,string(msg->GetStr(num)));
	}

	QuestTabMap* GetTabs()
	{
		return &tabs;
	}

	QuestTab* GetTab(DWORD tab_num)
	{
		if(tabs.empty()) return NULL;

		QuestTabMapIt it=tabs.begin();
		while(tab_num)
		{
			++it;
			--tab_num;
			if(it==tabs.end()) return NULL;	
		}

		return &(*it).second;
	}

	Quest* GetQuest(DWORD tab_num, WORD quest_num)
	{
		QuestTab* tab=GetTab(tab_num);
		return tab?tab->GetQuest(quest_num):NULL;
	}

	Quest* GetQuest(DWORD num)
	{
		if(!msg->Count(STR_QUEST_MAP_(num/QUEST_MUL))) return NULL;
		string tab_name=string(msg->GetStr(STR_QUEST_MAP_(num/QUEST_MUL)));
		QuestTabMapIt it_tab=tabs.find(tab_name);
		return it_tab!=tabs.end()?(*it_tab).second.GetQuest(num/QUEST_MUL):NULL;
	}
};

#endif // __QUEST_MANAGER__