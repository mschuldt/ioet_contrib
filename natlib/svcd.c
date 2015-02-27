
#include "libstormarray.h"
#include "lrodefs.h"

int SVCD_subdispatch(lua_State *L){
  //local parr = storm.array.fromstr(pay)
  size_t len;
  char *parr = lua_tolstring(L, 1, &len);
  char* srcip = lua_tostring(L, 2); //? what type is the ip?
  int strcport = lua_tointeger(L, 3);

  //local cmd = parr:get(1);
  char cmd = parr[0];
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
