#define handle_wrap_c
#include <uvwrap.h>

#define HANDLE_CALLBACK(name) UVWRAP_CALLBACK(handle, name)

void HANDLE_FUNCTION(ctor)(lua_State* L, uv_handle_t* handle) {
  lua_getfield(L, LUA_REGISTRYINDEX, UVWRAP_HANDLE_TRACE);
  lua_pushvalue(L, -2);
  lua_rawsetp(L, -2, (void*)handle);
  lua_pop(L, 1);
}

static int HANDLE_FUNCTION(is_active)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushboolean(L, uv_is_active(handle));
  return 1;
}

static int HANDLE_FUNCTION(is_closing)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushboolean(L, uv_is_closing(handle));
  return 1;
}

static void HANDLE_CALLBACK(close)(uv_handle_t* handle) {
  lua_State* L;
  PUSH_HANDLE_CLOSE_CALLBACK_CLEAN(L, handle);
  UNHOLD_HANDLE_ITSELF(L, handle);
  CLEAR_HANDLE_CALLBACK(L, handle, IDX_HANDLE_CALLBACK);
  CALL_LUA_FUNCTION(L, 0, 0);
}
static int HANDLE_FUNCTION(close)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  if (!uv_is_closing(handle)) {
    luaL_checktype(L, 2, LUA_TFUNCTION);
    SET_HANDLE_CLOSE_CALLBACK(L, handle, 2);
    HOLD_HANDLE_ITSELF(L, handle, 1);
    uv_close(handle, HANDLE_CALLBACK(close));
  }
  return 0;
}

static void HANDLE_CALLBACK(__gc)(uv_handle_t* handle) {
  lua_State* L;
  GET_HANDLE_DATA(L, handle);
  CLEAR_HANDLE_CALLBACK(L, handle, IDX_HANDLE_CALLBACK);
  UNHOLD_HANDLE_ITSELF(L, handle);
}
int HANDLE_FUNCTION(__gc)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  if (!uv_is_closing(handle)) {
    HOLD_HANDLE_ITSELF(L, handle, 1);
    uv_close(handle, HANDLE_CALLBACK(__gc));
  }
  return 0;
}

static int HANDLE_FUNCTION(ref)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_ref(handle);
  return 0;
}

static int HANDLE_FUNCTION(unref)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_unref(handle);
  return 0;
}

static int HANDLE_FUNCTION(has_ref)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushboolean(L, uv_has_ref(handle));
  return 1;
}

static int HANDLE_FUNCTION(send_buffer_size)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  int value = luaL_optinteger(L, 2, 0);
  lua_pushinteger(L, uv_send_buffer_size(handle, &value));
  return 1;
}

static int HANDLE_FUNCTION(recv_buffer_size)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  int value = luaL_optinteger(L, 2, 0);
  lua_pushinteger(L, uv_recv_buffer_size(handle, &value));
  return 1;
}

static int HANDLE_FUNCTION(fileno)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  uv_os_fd_t fd;
  int err = uv_fileno(handle, &fd);
  CHECK_ERROR(L, err);
  lua_pushinteger(L, fd);
  return 1;
}

static int HANDLE_FUNCTION(get_loop)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushlightuserdata(L, uv_handle_get_loop(handle));
  return 1;
}

static int HANDLE_FUNCTION(get_type)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  lua_pushinteger(L, uv_handle_get_type(handle));
  return 1;
}

static int HANDLE_FUNCTION(type_name)(lua_State* L) {
  uv_handle_t* handle = luaL_checkhandle(L, 1);
  int t = luaL_optinteger(L, 2, uv_handle_get_type(handle));
  lua_pushstring(L, uv_handle_type_name(t));
  return 1;
}

#define EMPLACE_HANDLE_FUNCTION(name) \
  { #name, HANDLE_FUNCTION(name) }

static const luaL_Reg HANDLE_FUNCTION(metafuncs)[] = {
    EMPLACE_HANDLE_FUNCTION(is_active),
    EMPLACE_HANDLE_FUNCTION(is_closing),
    EMPLACE_HANDLE_FUNCTION(close),
    EMPLACE_HANDLE_FUNCTION(ref),
    EMPLACE_HANDLE_FUNCTION(unref),
    EMPLACE_HANDLE_FUNCTION(has_ref),
    EMPLACE_HANDLE_FUNCTION(send_buffer_size),
    EMPLACE_HANDLE_FUNCTION(recv_buffer_size),
    EMPLACE_HANDLE_FUNCTION(fileno),
    EMPLACE_HANDLE_FUNCTION(get_loop),
    EMPLACE_HANDLE_FUNCTION(get_type),
    EMPLACE_HANDLE_FUNCTION(type_name),
    {NULL, NULL},
};

static void HANDLE_FUNCTION(init_metatable)(lua_State* L) {
  lua_createtable(L, 0, 32);
  lua_createtable(L, 0, 1);
  lua_pushliteral(L, "v");
  lua_setfield(L, -2, "__mode");
  lua_setmetatable(L, -2);
  lua_setfield(L, LUA_REGISTRYINDEX, UVWRAP_HANDLE_TRACE);

  luaL_newmetatable(L, UVWRAP_HANDLE_TYPE);
  luaL_setfuncs(L, HANDLE_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

void HANDLE_FUNCTION(init)(lua_State* L) {
  HANDLE_FUNCTION(init_metatable)
  (L);
}
