#define uvtcpwrap_c
#include <uvwrap.h>

#define TCP_FUNCTION(name) UVWRAP_FUNCTION(tcp, name)
#define TCP_CALLBACK(name) UVWRAP_CALLBACK(tcp, name)

static int TCP_FUNCTION(bind)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  struct sockaddr* addr = luaL_checksockaddr(L, 2);
  unsigned int flags = (unsigned int)luaL_optinteger(L, 3, 0);

  int err = uv_tcp_bind(handle, addr, flags);
  CHECK_ERROR(L, err);
  return 0;
}

static void TCP_CALLBACK(connectAsync)(uv_connect_t* req, int status) {
  lua_State* L;
  PUSH_REQ_CALLBACK_CLEAN(L, req);
  (void)MEMORY_FUNCTION(free_req)(req);
  lua_pushinteger(L, status);
  CALL_LUA_FUNCTION(L, 1);
}
static int TCP_FUNCTION(connectAsync)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  struct sockaddr* addr = luaL_checksockaddr(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  uv_connect_t* req = (uv_connect_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_connect_t));

  int err = uv_tcp_connect(req, handle, addr, TCP_CALLBACK(connectAsync));
  if (err == UVWRAP_OK) {
    SET_REQ_CALLBACK(L, 3, req);
  }
  lua_pushinteger(L, err);
  return 1;
}

static void TCP_CALLBACK(connectAsyncWait)(uv_connect_t* req, int status) {
  REQ_ASYNC_WAIT_PREPARE();
  UNHOLD_REQ_PARAM(co, req, 2);
  REQ_ASYNC_WAIT_RESUME(connectAsyncWait);
}
static int TCP_FUNCTION(connectAsyncWait)(lua_State* co) {
  CHECK_COROUTINE(co);
  uv_tcp_t* handle = luaL_checktcp(co, 1);
  struct sockaddr* addr = luaL_checksockaddr(co, 2);

  uv_connect_t* req = (uv_connect_t*)MEMORY_FUNCTION(malloc_req)(sizeof(uv_connect_t));

  int err = uv_tcp_connect(req, handle, addr, TCP_CALLBACK(connectAsyncWait));
  if (err == UVWRAP_OK) {
    HOLD_COROUTINE(co);
    HOLD_REQ_PARAM(co, req, 2, 2);
    return lua_yield(co, 0);
  }
  lua_pushinteger(co, err);
  return 1;
}

static int TCP_FUNCTION(noDelay)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int enable = lua_toboolean(L, 2);
  int err = uv_tcp_nodelay(handle, enable);
  CHECK_ERROR(L, err);
  return 0;
}

static int TCP_FUNCTION(keepAlive)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int enable = lua_toboolean(L, 2);
  unsigned int delay = luaL_checkinteger(L, 3);
  int err = uv_tcp_keepalive(handle, enable, delay);
  CHECK_ERROR(L, err);
  return 0;
}

static int TCP_FUNCTION(simultaneousAccepts)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  luaL_checktype(L, 2, LUA_TBOOLEAN);
  int enable = lua_toboolean(L, 2);
  int err = uv_tcp_simultaneous_accepts(handle, enable);
  CHECK_ERROR(L, err);
  return 0;
}

static int TCP_FUNCTION(getSockName)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  struct sockaddr_storage* addr = (struct sockaddr_storage*)SOCKADDR_FUNCTION(create)(L);
  int len = sizeof(struct sockaddr_storage);
  int err = uv_tcp_getsockname(handle, (struct sockaddr*)addr, &len);
  CHECK_ERROR(L, err);
  return 1;
}

static int TCP_FUNCTION(getPeerName)(lua_State* L) {
  uv_tcp_t* handle = luaL_checktcp(L, 1);
  struct sockaddr_storage* addr = (struct sockaddr_storage*)SOCKADDR_FUNCTION(create)(L);
  int len = sizeof(struct sockaddr_storage);
  int err = uv_tcp_getpeername(handle, (struct sockaddr*)addr, &len);
  CHECK_ERROR(L, err);
  return 1;
}

static int TCP_FUNCTION(__gc)(lua_State* L) {
  return STREAM_FUNCTION(__gc)(L);
}

#define EMPLACE_TCP_FUNCTION(name) \
  { #name, TCP_FUNCTION(name) }

const luaL_Reg TCP_FUNCTION(metafuncs)[] = {
    EMPLACE_TCP_FUNCTION(bind),
    EMPLACE_TCP_FUNCTION(connectAsync),
    EMPLACE_TCP_FUNCTION(connectAsyncWait),
    EMPLACE_TCP_FUNCTION(noDelay),
    EMPLACE_TCP_FUNCTION(keepAlive),
    EMPLACE_TCP_FUNCTION(simultaneousAccepts),
    EMPLACE_TCP_FUNCTION(getSockName),
    EMPLACE_TCP_FUNCTION(getPeerName),
    EMPLACE_TCP_FUNCTION(__gc),
    {NULL, NULL},
};

static void TCP_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, UVWRAP_TCP_TYPE);
  luaL_setfuncs(L, TCP_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  luaL_setmetatable(L, UVWRAP_STREAM_TYPE);

  lua_pop(L, 1);
}

static int TCP_FUNCTION(Tcp)(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  unsigned int flags = luaL_optinteger(L, 2, AF_UNSPEC);

  uv_tcp_t* handle = (uv_tcp_t*)lua_newuserdata(L, sizeof(uv_tcp_t));

  int err = uv_tcp_init_ex(loop, handle, flags); // return 0 when success
  CHECK_ERROR(L, err);

  luaL_setmetatable(L, UVWRAP_TCP_TYPE);
  (void)STREAM_FUNCTION(ctor)(L, (uv_stream_t*)handle);
  return 1;
}

static const luaL_Reg TCP_FUNCTION(funcs)[] = {
    EMPLACE_TCP_FUNCTION(Tcp),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(tcp_flag)[] = {
    {"IPV6ONLY", UV_TCP_IPV6ONLY},
    {NULL, 0},
};

DEFINE_INIT_API_BEGIN(tcp)
PUSH_LIB_TABLE(tcp);
REGISTE_ENUM_UVWRAP(tcp_flag);
INVOKE_INIT_METATABLE(tcp);
DEFINE_INIT_API_END(tcp)
