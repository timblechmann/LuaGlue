#ifndef LUAGLUE_CTOR_METHOD_H_GUARD
#define LUAGLUE_CTOR_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueApplyTuple.h"

class LuaGlue;
template<typename _Class>
class LuaGlueClass;

template<typename _Class, typename... _Args>
class LuaGlueCtorMethodImpl : public LuaGlueMethodImplBase
{
	public:
		typedef _Class ClassType;
		typedef _Class *ReturnType;
		
		LuaGlueCtorMethodImpl(LuaGlueClass<_Class> *luaClass, const std::string &name) : glueClass(luaClass), name_(name)
		{ }
		
		~LuaGlueCtorMethodImpl() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlue *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		std::tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
		int invoke(lua_State *state)
		{
			_Class *obj = applyTuple<_Class>(state, args);
			lua_pop(state, Arg_Count_);
			_Class **udata = (_Class **)lua_newuserdata(state, sizeof(_Class *));
			*udata = obj;
			
			luaL_getmetatable(state, glueClass->name().c_str());
			lua_setmetatable(state, -2);

			return 1;
		}
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueCtorMethodImpl<_Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_CTOR_METHOD_H_GUARD */