#ifndef __SCRIPT__
#define __SCRIPT__

#include "Common.h"
#include "AngelScript/angelscript.h"
#include "AngelScript/scriptarray.h"
#include "AngelScript/scriptstring.h"
#include "AngelScript/Preprocessor/pragma.h"
#include <vector>
#include <string>

#define GLOBAL_CONTEXT_STACK_SIZE      (10)
#define CONTEXT_BUFFER_SIZE            (512)

typedef void(*EndExecutionCallback)();

struct ReservedScriptFunction
{
	int* BindId;
	char FuncName[256];
	char FuncDecl[256];
};

namespace Script
{
	bool Init(bool with_log, Preprocessor::PragmaCallback* pragma_callback);
	void Finish();
	bool InitThread();
	void FinisthThread();

	void* LoadDynamicLibrary(const char* dll_name);
	size_t* GetFunctionAddress(void* dll, const char* func_name);
	void SetWrongGlobalObjects(StrVec& names);
	void SetConcurrentExecution(bool enabled);
	void SetLoadLibraryCompiler(bool enabled);

	void UnloadScripts();
	bool ReloadScripts(const char* config, const char* key, bool skip_binaries, const char* file_pefix = NULL);
	bool BindReservedFunctions(const char* config, const char* key, ReservedScriptFunction* bind_func, uint bind_func_count);

	void AddRef(void*); // Dummy
	void Release(void*); // Dummy

	asIScriptEngine* GetEngine();
	void SetEngine(asIScriptEngine* engine);
	asIScriptEngine* CreateEngine(Preprocessor::PragmaCallback* pragma_callback);
	void FinishEngine(asIScriptEngine*& engine);

	asIScriptContext* CreateContext();
	void FinishContext(asIScriptContext*& ctx);
	asIScriptContext* GetGlobalContext();
	void PrintContextCallstack(asIScriptContext* ctx);

	const char* GetActiveModuleName();
	const char* GetActiveFuncName();
	asIScriptModule* GetModule(const char* name);
	asIScriptModule* CreateModule(const char* module_name);

#ifdef FONLINE_SERVER
	uint GetGCStatistics();
	void ScriptGarbager();
	void CollectGarbage(asDWORD flags = asGC_FULL_CYCLE);
#else
	void SetGarbageCollectTime(uint ticks);
	void CollectGarbage(bool force = true);
#endif

	void SetRunTimeout(uint suspend_timeout, uint message_timeout);

	void SetScriptsPath(int path_type);
	void Define(const char* def);
	void Undefine(const char* def);
	char* Preprocess(const char* fname, bool process_pragmas);
	void CallPragmas(const StrVec& pragmas);
	bool LoadScript(const char* module_name, const char* source, bool skip_binary, const char* file_pefix = NULL);
	bool LoadScript(const char* module_name, const uchar* bytecode, uint len);

	int BindImportedFunctions();
	int Bind(const char* module_name, const char* func_name, const char* decl, bool is_temp, bool disable_log = false);
	int Bind(const char* script_name, const char* decl, bool is_temp, bool disable_log = false);
	int RebindFunctions();
	bool ReparseScriptName(const char* script_name, char* module_name, char* func_name, bool disable_log = false);

	const StrVec& GetScriptFuncCache();
	void ResizeCache(uint count);
	uint GetScriptFuncNum(const char* script_name, const char* decl);
	int GetScriptFuncBindId(uint func_num);
	string GetScriptFuncName(uint func_num);

	// Script execution
	void BeginExecution();
	void EndExecution();
	void AddEndExecutionCallback(EndExecutionCallback func);

	bool PrepareContext(int bind_id, const char* call_func, const char* ctx_info);
	void SetArgUShort(ushort value);
	void SetArgUInt(uint value);
	void SetArgUChar(uchar value);
	void SetArgBool(bool value);
	void SetArgObject(void* value);
	void SetArgAddress(void* value);
	bool RunPrepared();
	uint GetReturnedUInt();
	bool GetReturnedBool();
	void* GetReturnedObject();

	bool SynchronizeThread();
	bool ResynchronizeThread();

	// Logging
	bool StartLog();
	void EndLog();
	void Log(const char* str);
	void LogA(const char* str);
	void LogError(const char* call_func, const char* error);
	void SetLogDebugInfo(bool enabled);

	void CallbackMessage(const asSMessageInfo* msg, void* param);
	void CallbackException(asIScriptContext* ctx, void* param);

	// Arrays stuff
	CScriptArray* CreateArray(const char* type);

	template<typename Type>
	void AppendVectorToArray(vector<Type>& vec, CScriptArray* arr)
	{
		if(!vec.empty() && arr)
		{
			asUINT i=arr->GetSize();
			arr->Resize(arr->GetSize()+vec.size());
			for(uint k=0,l=vec.size();k<l;k++,i++)
			{
				Type* p=(Type*)arr->At(i);
				*p=vec[k];
			}
		}
	}
	template<typename Type>
	void AppendVectorToArrayRef(vector<Type>& vec, CScriptArray* arr)
	{
		if(!vec.empty() && arr)
		{
			asUINT i=arr->GetSize();
			arr->Resize(arr->GetSize()+vec.size());
			for(uint k=0,l=vec.size();k<l;k++,i++)
			{
				Type* p=(Type*)arr->At(i);
				*p=vec[k];
				(*p)->AddRef();
			}
		}
	}
	template<typename Type>
	void AssignScriptArrayInVector(vector<Type>& vec, CScriptArray* arr)
	{
		if(arr)
		{
			asUINT count=arr->GetSize();
			if(count)
			{
				vec.resize(count);
				for(asUINT i=0;i<count;i++)
				{
					Type* p=(Type*)arr->At(i);
					vec[i]=*p;
				}
			}
		}
	}
}

class CBytecodeStream : public asIBinaryStream
{
private:
	int readPos;
	int writePos;
	std::vector<asBYTE> binBuf;

public:
	CBytecodeStream(){writePos=0; readPos=0;}
	void Write(const void* ptr, asUINT size){if(!ptr || !size) return; binBuf.resize(binBuf.size()+size); memcpy(&binBuf[writePos],ptr,size); writePos+=size;}
	void Read(void* ptr, asUINT size){if(!ptr || !size) return; memcpy(ptr,&binBuf[readPos],size); readPos+=size;}
	std::vector<asBYTE>& GetBuf(){return binBuf;}
};

#endif // __SCRIPT__
