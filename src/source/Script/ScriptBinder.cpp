#include <Script/ScriptBinder.hpp>

struct Acc {
	int v;
	Acc(int v = 0) : v(v) {}
	void add(int x) { v += x; }
	void mul(int x) { v *= x; }
};

void MessageCallback(const asSMessageInfo *msg, void *param) {
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING ) 
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION ) 
		type = "INFO";
	
	
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

namespace ScriptBinder {
	
	void angelscriptTest() {
		asIScriptEngine *engine = asCreateScriptEngine();
		if (!engine) {
			printf("ENGINE IS nullptr\n");
			return;
		}
		
		engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		// register class
		registerEngine(engine);
		engine->RegisterObjectType("Acc", sizeof(Acc), asOBJ_VALUE | asOBJ_POD);
		engine->RegisterObjectMethod("Acc", "void add(int)", asMETHOD(Acc, add), asCALL_THISCALL);
		engine->RegisterObjectMethod("Acc", "void mul(int)", asMETHOD(Acc, mul), asCALL_THISCALL);
		engine->RegisterObjectProperty("Acc", "int value", asOFFSET(Acc, v));
		
		// compiling script
		std::string scriptSrc = File::Read("res/entitydefs/fib.as");
		asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
		mod->AddScriptSection("fib.as", scriptSrc.c_str());
		int r = mod->Build();
		if (r < 0) printf("err\n");
		// get function
		asIScriptContext *ctx = engine->CreateContext();
		asIScriptFunction *fib = mod->GetFunctionByDecl("int fib(int)");
		// call function
		Benchmark b;
		for (int i = 0; i < 35; ++i) {
			ctx->Prepare(fib);
			ctx->SetArgDWord(0, i);
			int r = ctx->Execute();
			if (r == asEXECUTION_FINISHED) {
				asDWORD ret = ctx->GetReturnDWord();
				printf("fib.as(%d) = %d\n", i, ret);
			}
		}
		ctx->Release();
		b.stop();
		printf("fib.as(0..35) took %gs\n", b.s());
		
		
		engine->ShutDownAndRelease();
	}
	
	void luaTest() {
		sol::state lua;
		lua.open_libraries();
		lua.script_file("res/entitydefs/fib.lua");
		auto fib = lua["fib"];
		
		Benchmark b;
		
		for (int i = 0; i < 35; ++i) {
			int r = fib(i);
			printf("fib.lua(%d) = %d\n", i, r);
		}
		
		b.stop();
		printf("fib.lua(0..35) took %gs\n", b.s());
	}
	
	void registerEngine(asIScriptEngine *engine) {
		//registerComponents(engine);
	}
	
	
	int registerComponents(asIScriptEngine *engine) {
		int r; // result
		
		// CPosition
		engine->RegisterObjectType("CPosition", 0, asOBJ_REF);
		return_if(r);
		
		return 0;
	}
}
