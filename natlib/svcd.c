#include "libstormarray.h"
#include "lrodefs.h"

static int svcd_subdispatch(lua_State *L);

//This file is included into native.c

#define SVCD_SYMBOLS \
    { LSTRKEY( "svcd_init"), LFUNCVAL ( svcd_init ) }, \
    { LSTRKEY( "svcd_subdispatch"), LFUNCVAL ( svcd_subdispatch ) }, 

//If this file is defining only specific functions, or if it
//is defining the whole thing
#define SVCD_PUREC 0

// This is the metatable for the SVCD table. It will allow use to put some constants
// and symbols into ROM. We could of course put everything into ROM but that would
// prevent consumers from overriding the contents of the table for things like
// advert_received, which you may want to hook into
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
static const LUA_REG_TYPE svcd_meta_map[] =
{
    { LSTRKEY( "__index" ), LROVAL ( svcd_meta_map ) },
    { LSTRKEY( "OK" ), LNUMVAL ( 1 ) },
    { LSTRKEY( "TIMEOUT" ), LNUMVAL ( 2 ) },

    { LNILKEY, LNILVAL }
};


//////////////////////////////////////////////////////////////////////////////
// SVCD.init() implementation
// Maintainer: Michael Andersen <michael@steelcode.com>
/////////////////////////////////////////////////////////////

// The anonymous func in init that allows for dynamic binding of advert_received
static int svcd_init_adv_received( lua_State *L )
{
    int numargs = lua_gettop(L);
    lua_getglobal(L, "SVCD");
    lua_pushstring(L, "advert_received");
    //Get the advert_received function from the table
    lua_gettable(L, -2);
    //Move it to before the arguments
    lua_insert(L, 1);
    //Pop off the SVCD table
    lua_settop(L, numargs+1);
    //Note that we now call this function from C, so it cannot use any cord await
    //functions. If it needs to do that sort of thing, it can spawn a new cord to do so
    lua_call(L, numargs, 0);
    return 0;
}

// Lua: storm.n.svcd_init ( id, onready )
// Initialises the SVCD module, in global scope
static int svcd_init( lua_State *L )
{
    if (lua_gettop(L) != 2) return luaL_error(L, "Expected (id, onready)");
#if SVCD_PUREC
//If we are going for a pure C implementation, then this would create the global
//SVCD table, otherwise it is created by the Lua code
        //Create the SVCD global table
        lua_createtable(L, 0, 8);
        //Set the metatable
        lua_pushrotable(L, (void*)svcd_meta_map);
        lua_setmetatable(L, 3);
        //Create the empty tables
        lua_pushstring(L, "manifest_map");
        lua_newtable(L);
        lua_settable(L, 3);
        lua_pushstring(L, "blsmap");
        lua_newtable(L);
        lua_settable(L, 3);
        lua_pushstring(L, "blamap");
        lua_newtable(L);
        lua_settable(L, 3);
        lua_pushstring(L, "oursubs");
        lua_newtable(L);
        lua_settable(L, 3);
        lua_pushstring(L, "subscribers");
        lua_newtable(L);
        lua_settable(L, 3);
        lua_pushstring(L, "handlers");
        lua_newtable(L);
        lua_settable(L, 3);
        lua_pushstring(L, "ivkid");
        lua_pushnumber(L, 0);
        lua_settable(L, 3);
        //Duplicate the TOS so the table is still there after
        //setglobal
        lua_pushvalue(L, -1);
        lua_setglobal(L, "SVCD");
#else
    //Load the SVCD table that Lua created
    //This will be index 3
    lua_getglobal(L, "SVCD");
    printf("Put table at %d\n", lua_gettop(L));
#endif
    //Now begins the part that corresponds with the lua init function

    //SVCD.asock
    lua_pushstring(L, "asock");
    lua_pushlightfunction(L, libstorm_net_udpsocket);
    lua_pushnumber(L, 2525);
    lua_pushlightfunction(L, svcd_init_adv_received);
    lua_call(L, 2, 1);
    lua_settable(L, 3); //Store it in the table

    //SVCD.ssock
    lua_pushstring(L, "ssock");
    lua_pushlightfunction(L, libstorm_net_udpsocket);
    lua_pushnumber(L, 2526);
    lua_pushstring(L, "wdispatch");
    lua_gettable(L, 3);
    lua_call(L, 2, 1);
    lua_settable(L, 3); //Store

    //SVCD.nsock
    lua_pushstring(L, "nsock");
    lua_pushlightfunction(L, libstorm_net_udpsocket);
    lua_pushnumber(L, 2527);
    lua_pushstring(L, "ndispatch");
    lua_gettable(L, 3);
    lua_call(L, 2, 1);
    lua_settable(L, 3); //Store

    //SVCD.wcsock
    lua_pushstring(L, "wcsock");
    lua_pushlightfunction(L, libstorm_net_udpsocket);
    lua_pushnumber(L, 2528);
    lua_pushstring(L, "wcdispatch");
    lua_gettable(L, 3);
    lua_call(L, 2, 1);
    lua_settable(L, 3); //Store

    //SVCD.ncsock
    lua_pushstring(L, "ncsock");
    lua_pushlightfunction(L, libstorm_net_udpsocket);
    lua_pushnumber(L, 2529);
    lua_pushstring(L, "ncdispatch");
    lua_gettable(L, 3);
    lua_call(L, 2, 1);
    lua_settable(L, 3); //Store

    //SVCD.subsock
    lua_pushstring(L, "subsock");
    lua_pushlightfunction(L, libstorm_net_udpsocket);
    lua_pushnumber(L, 2530);
    lua_pushlightfunction(L, svcd_subdispatch);
    lua_call(L, 2, 1);
    lua_settable(L, 3); //Store

    //manifest table
    lua_pushstring(L, "manifest");
    lua_newtable(L);
    lua_pushstring(L, "id");
    lua_pushvalue(L ,1);
    lua_settable(L, -3);
    lua_settable(L, 3);

     //If id ~= nil
    if (!lua_isnil(L, 1)) {
        lua_pushlightfunction(L, libstorm_os_invoke_periodically);
        lua_pushnumber(L, 3*SECOND_TICKS);
        lua_pushlightfunction(L, libstorm_net_sendto);
        lua_pushstring(L, "asock");
        lua_gettable(L, 3);
        //Pack SVCD.manifest
        lua_pushlightfunction(L, libmsgpack_mp_pack);
        lua_pushstring(L, "manifest");
        lua_gettable(L, 3);
        lua_call(L, 1, 1);
        //Address
        lua_pushstring(L, "ff02::1");
        lua_pushnumber(L, 2525);
        cord_dump_stack(L);
        lua_call(L, 6, 0);

        //Enable the bluetooth
        lua_pushlightfunction(L, libstorm_bl_enable);
        lua_pushvalue(L, 1);
        lua_pushstring(L, "cchanged");
        lua_gettable(L, 3);
        lua_pushvalue(L, 2);
        lua_call(L, 3, 0);
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////
// SVCD.subdispatch() implementation
// Maintainer: Running with scissors
//////////////////////////////////////////////////////////////////////////////

static int svcd_subdispatch(lua_State *L) {
  //local parr = storm.array.fromstr(pay)
  size_t len;
  uint8_t *parr __attribute__((aligned(2))) = lua_tolstring(L, 1, &len);
  uint8_t *srcip = lua_tostring(L, 2);
  uint32_t strcport = lua_tointeger(L, 3);

  //local cmd = parr:get(1);
  uint8_t cmd = parr[0];
  //local svc_id = parr:get_as(storm.array.UINT16,1);
  int16_t svc_id = *(int16_t*)(parr+1);
  //local attr_id = parr:get_as(storm.array.UINT16,3);
  int16_t attr_id = *(int16_t*)(parr+3);
  //local ivkid = parr:get_as(storm.array.UINT16, 5);
  int16_t ivkid = *(int16_t*)(parr+5);

  lua_getglobal(L, "SVCD");
  lua_pushstring(L, "subscribers");
  lua_gettable(L, -2);
  lua_pushinteger(L, svc_id);
  lua_gettable(L, -2);
  //stack: SVCD, SVCD[subscribers], SVCD[subscribers][svc_id]
  if (cmd == 1){ //subscribe command
    if (lua_isnil(L,-1)){ // if (SVCD.subscribers[svc_id] == nil){
      lua_pop(L, 1); //stack: SVCD, SVCD[subscribers]
      //SVCD.subscribers[svc_id] = {};
      lua_newtable(L);
      lua_pushinteger(L, svc_id);
      lua_pushvalue(L, -2);
      //stack: SVCD, SVCD[subscribers], <newTable>, svc_id, <newTable>
      lua_settable(L, -4);
    }
    //stack: SVCD, SVCD[subscribers], SVCD[subscribers][svc_id],
    //if (SVCD.subscribers[svc_id][attr_id] == nil){
    lua_pushstring(L,"attr_id");
    lua_gettable(L, -2);
    if (lua_isnil(L, -1)){
      //SVCD.subscribers[svc_id][attr_id] = {}
      lua_pop(L, 1);
      lua_newtable(L);
      lua_pushinteger(L, attr_id);
      lua_pushvalue(L, -2);
      lua_settable(L, -4);
    }
    //stack: SVCD[subscribers], SVCD[subscribers][svc_id], SVCD[subscribers][svc_id][attr_id]

    //SVCD.subscribers[svc_id][attr_id][srcip] = ivkid;
    lua_pushstring(L, srcip);
    lua_pushnumber(L, ivkid);
    lua_settable(L, -3);

  }else if (cmd == 0){ //unsubscribe command
    //stack: SVCD, SVCD[subscribers], SVCD[subscribers][svc_id]

    //if (SVCD.subscribers[svc_id] == nil){
    if (lua_isnil(L, -1)){
      return 0;
    }
    //if (SVCD.subscribers[svc_id][attr_id] == nil){
    lua_pushnumber(L, attr_id);
    lua_gettable(L, -2);
    if (lua_isnil(L, -1)){
      return 0;
    }
    //stack: SVCD, SVCD[subscribers], SVCD[subscribers][svc_id], SVCD[subscribers][svc_id][attr_id]
    //SVCD.subscribers[svc_id][attr_id][srcip] = nil;
    lua_pushstring(L, srcip);
    lua_pushnil(L);
    lua_settable(L, -3);
  }
  return 0;
}
