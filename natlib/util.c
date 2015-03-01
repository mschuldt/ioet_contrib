//This file is included into native.c

static void cord_dump_stack (lua_State *L)
{ //Stolen from the book
      int i;
      int top = lua_gettop(L);
      for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {

          case LUA_TSTRING:  /* strings */
            printf("`%s'", lua_tostring(L, i));
            break;

          case LUA_TBOOLEAN:  /* booleans */
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;

          case LUA_TNUMBER:  /* numbers */
            printf("%d", (int) lua_tonumber(L, i));
            break;

          default:  /* other values */
            printf("%s", lua_typename(L, t));
            break;

        }
        printf("  ");  /* put a separator */
      }
      printf("\n");  /* end the listing */
    }

//Not a lua function
//First push the upvalues
//then call this
static inline void cord_set_continuation(lua_State *L, lua_CFunction continuation, int num_upvalues)
{
    lua_pushcclosure(L, continuation, num_upvalues);
}

//First call set_continuation
//Then push fn
//Then push arguments
static int cord_invoke_custom(lua_State *L, int num_arguments)
{
  printf("cord_invoke_custom()\n");
    int argidx = lua_gettop(L) - num_arguments + 1;
    int i;
    lua_createtable(L, num_arguments, 0);
    lua_insert(L, argidx);
    for (i=num_arguments;i>=1;i--)
        lua_rawseti(L, argidx, i);
    printf(" cord_invoke_custom yielding...\n");
    return lua_yield(L, 3); //continuation, targetfn, table of args
}

//Push your return values
//then call this
static int cord_return(lua_State *L, int num_vals)
{
    int i;
    int rvidx = lua_gettop(L) - num_vals + 1;
    lua_createtable(L, num_vals, 0);
    lua_insert(L, rvidx);
    for (i=num_vals;i>=1;i--)
        lua_rawseti(L, rvidx, i);
    lua_pushnumber(L, -1);
    lua_insert(L, -2);
    return lua_yield(L, 2);
}

static int nc_invoke_sleep(lua_State *L, int ticks)
{
    lua_getglobal(L, "cord");
    lua_pushstring(L, "await");
    lua_rawget(L, -2);
    lua_remove(L, -2);
    lua_pushlightfunction(L, libstorm_os_invoke_later);
    lua_pushnumber(L, ticks);
    return cord_invoke_custom(L, 2);
}

#include "libstormarray.h" //for storm_array_t
//#include <libstorm.h>
#define nc_invoke_i2c_read(L, addr, flags, arr) _nc_invoke_i2c(L, addr, flags, arr, 0)
#define nc_invoke_i2c_write(L, addr, flags, arr) _nc_invoke_i2c(L, addr, flags, arr, 1)
static int 
_nc_invoke_i2c(lua_State *L, uint32_t address, uint32_t flags, storm_array_t *arr, char x)
{
  printf("_nc_invoke_i2c()\n");
  lua_getglobal(L, "cord");
  lua_pushstring(L, "await");
  lua_rawget(L, -2);
  lua_remove(L, -2);
  if (x){
    printf("   write\n");
    lua_pushlightfunction(L, libstorm_i2c_write);
  }else{
    printf("   read\n");
    lua_pushlightfunction(L, libstorm_i2c_read);
  }
  lua_pushnumber(L, address);
  lua_pushnumber(L, flags);
  lua_pushlightuserdata(L, arr);
  printf("ending _nc_invoke_i2c...\n");
  return cord_invoke_custom(L, 4);
}

/**
 * Take the function at the top of the stack and NC wrap it
 * so that it can be invoked directly from a cord
 */
static int cord_wrap_nc(lua_State *L)
{
    lua_getglobal(L, "cord");
    lua_pushstring(L, "ncw");
    lua_rawget(L, -2);
    lua_remove(L, -2);
    lua_insert(L, -2);
    lua_call(L, 1, 1);
    return 1;
}

