/* Lua C Library */

#define _bcfx_wrap_c_
#include <bcfx_wrap.h>

/*
** {======================================================
** BCFX Resource Manage
** =======================================================
*/

static void on_frame_completed(void* ud, uint32_t frameId) {
  lua_State* L = (lua_State*)ud;
  lua_checkstack(L, 2);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (const void*)on_frame_completed);
  lua_pushnil(L);
  lua_rawseti(L, -2, frameId);
}

static void init_resource_manage(lua_State* L) {
  lua_createtable(L, 0, 2);
  lua_rawsetp(L, LUA_REGISTRYINDEX, (const void*)on_frame_completed);
  bcfx_setFrameCompletedCallback(on_frame_completed, (void*)L);
}

static void hold_frame_resourse(lua_State* L, uint32_t frameId, int idx) {
  idx = lua_absindex(L, idx);
  lua_rawgetp(L, LUA_REGISTRYINDEX, (const void*)on_frame_completed);
  lua_rawgeti(L, -1, frameId);
  if (lua_isnil(L, -1)) {
    lua_pop(L, 1);
    lua_createtable(L, 0, 4);
    lua_pushvalue(L, -1);
    lua_rawseti(L, -3, frameId);
  }
  lua_pushvalue(L, idx);
  lua_pushboolean(L, true);
  lua_rawset(L, -3);
  lua_pop(L, 2);
}

/* }====================================================== */

/*
** {======================================================
** BCFX Wrap Functions
** =======================================================
*/

static int BCWRAP_FUNCTION(setThreadFuncs)(lua_State* L) {
  void* create = luaL_checklightuserdata(L, 1);
  void* self = luaL_checklightuserdata(L, 2);
  void* invalid = luaL_checklightuserdata(L, 3);
  void* join = luaL_checklightuserdata(L, 4);
  void* equal = luaL_checklightuserdata(L, 5);
  bcfx_setThreadFuncs(
      (bcfx_ThreadCreate)create,
      (bcfx_ThreadSelf)self,
      (bcfx_ThreadInvalid)invalid,
      (bcfx_ThreadJoin)join,
      (bcfx_ThreadEqual)equal);
  return 0;
}
static int BCWRAP_FUNCTION(setSemFuncs)(lua_State* L) {
  void* init = luaL_checklightuserdata(L, 1);
  void* destroy = luaL_checklightuserdata(L, 2);
  void* post = luaL_checklightuserdata(L, 3);
  void* wait = luaL_checklightuserdata(L, 4);
  void* tryWait = luaL_checklightuserdata(L, 5);
  bcfx_setSemFuncs(
      (bcfx_SemInit)init,
      (bcfx_SemDestroy)destroy,
      (bcfx_SemPost)post,
      (bcfx_SemWait)wait,
      (bcfx_SemTryWait)tryWait);
  return 0;
}
static int BCWRAP_FUNCTION(setWinCtxFuncs)(lua_State* L) {
  void* makeCurrent = luaL_checklightuserdata(L, 1);
  void* swapBuffers = luaL_checklightuserdata(L, 2);
  void* swapInterval = luaL_checklightuserdata(L, 3);
  void* getProcAddress = luaL_checklightuserdata(L, 4);
  void* getWindowSize = luaL_checklightuserdata(L, 5);
  bcfx_setWinCtxFuncs(
      (bcfx_MakeContextCurrent)makeCurrent,
      (bcfx_SwapBuffers)swapBuffers,
      (bcfx_SwapInterval)swapInterval,
      (bcfx_GetProcAddress)getProcAddress,
      (bcfx_GetWindowSize)getWindowSize);
  return 0;
}
static int BCWRAP_FUNCTION(setMiscFuncs)(lua_State* L) {
  void* getTime = luaL_checklightuserdata(L, 1);
  bcfx_setMiscFuncs((bcfx_GetTime)getTime);
  return 0;
}

static int BCWRAP_FUNCTION(init)(lua_State* L) {
  void* mainWin = luaL_checklightuserdata(L, 1);
  bcfx_init(mainWin);
  return 0;
}
static int BCWRAP_FUNCTION(apiFrame)(lua_State* L) {
  uint32_t renderCount = luaL_optinteger(L, 1, -1);
  bcfx_apiFrame(renderCount);
  return 0;
}
static int BCWRAP_FUNCTION(shutdown)(lua_State* L) {
  bcfx_shutdowm();
  return 0;
}

static int BCWRAP_FUNCTION(createVertexLayout)(lua_State* L) {
  bcfx_VertexLayout* layout = luaL_checkvertexlayout(L, 1);

  Handle handle = bcfx_createVertexLayout(layout);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createVertexBuffer)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  Handle layoutHandle = luaL_checkinteger(L, 2);

  Handle handle = bcfx_createVertexBuffer(mb, layoutHandle);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createIndexBuffer)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);
  if (mb->dt != DT_Uint8 &&
      mb->dt != DT_Uint16 &&
      mb->dt != DT_Uint32) {
    return luaL_error(L, "index buffer only support Uint8/Uint16/Uint32");
  }

  Handle handle = bcfx_createIndexBuffer(mb);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createShader)(lua_State* L) {
  bcfx_MemBuffer buffer;
  bcfx_MemBuffer* mb = &buffer;
  if (lua_isstring(L, 1)) {
    MEMBUFFER_CLEAR(mb);
    mb->ptr = (void*)lua_tolstring(L, 1, &mb->sz);
    hold_frame_resourse(L, bcfx_frameId(), 1);
  } else {
    mb = luaL_checkmembuffer(L, 1);
  }
  uint8_t type = (uint8_t)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createShader(mb, (ShaderType)type);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createProgram)(lua_State* L) {
  Handle vs = (Handle)luaL_checkinteger(L, 1);
  Handle fs = (Handle)luaL_checkinteger(L, 2);

  Handle handle = bcfx_createProgram(vs, fs);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createUniform)(lua_State* L) {
  const char* name = luaL_checkstring(L, 1);
  bcfx_UniformType type = (bcfx_UniformType)luaL_checkinteger(L, 2);
  uint16_t num = luaL_optinteger(L, 3, 1);

  Handle handle = bcfx_createUniform(name, type, num);
  lua_pushinteger(L, handle);
  return 1;
}
static int BCWRAP_FUNCTION(createTexture)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  Handle handle = bcfx_createTexture(mb);
  MEMBUFFER_CLEAR(mb); // because pass bcfx_MemBuffer to bcfx as Value, not Reference

  lua_pushinteger(L, handle);
  return 1;
}

static int BCWRAP_FUNCTION(setViewWindow)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Window win = (Window)luaL_checklightuserdata(L, 2);
  bcfx_setViewWindow(id, win);
  return 0;
}
static int BCWRAP_FUNCTION(setViewClear)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t flags = (uint16_t)luaL_checkinteger(L, 2);
  uint32_t rgba = (uint32_t)luaL_checkinteger(L, 3);
  float depth = (float)luaL_checknumber(L, 4);
  uint8_t stencil = (uint8_t)luaL_checkinteger(L, 5);
  bcfx_setViewClear(id, flags, rgba, depth, stencil);
  return 0;
}
static int BCWRAP_FUNCTION(setViewRect)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  uint16_t x = (uint16_t)luaL_checkinteger(L, 2);
  uint16_t y = (uint16_t)luaL_checkinteger(L, 3);
  uint16_t width = (uint16_t)luaL_checkinteger(L, 4);
  uint16_t height = (uint16_t)luaL_checkinteger(L, 5);
  bcfx_setViewRect(id, x, y, width, height);
  return 0;
}

static int BCWRAP_FUNCTION(setUniform)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  Mat* mat;
  if (luaL_testudata(L, 2, BCFX_VECTOR_TYPE)) {
    Vec4* vec = luaL_checkvec4(L, 2);
    bcfx_setUniformVec4(handle, vec, 1);
  } else if ((mat = (Mat*)luaL_testudata(L, 2, BCFX_MATRIX_TYPE))) {
    if (IS_MAT3x3(mat)) {
      bcfx_setUniformMat3x3(handle, (Mat3x3*)mat, 1);
    } else if (IS_MAT4x4(mat)) {
      bcfx_setUniformMat4x4(handle, (Mat4x4*)mat, 1);
    } else {
    }
  }
  return 0;
}

static int BCWRAP_FUNCTION(setVertexBuffer)(lua_State* L) {
  uint8_t stream = luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);

  bcfx_setVertexBuffer(stream, handle);
  return 0;
}
static int BCWRAP_FUNCTION(setIndexBuffer)(lua_State* L) {
  Handle handle = (Handle)luaL_checkinteger(L, 1);
  uint32_t start = luaL_optinteger(L, 2, 0);
  uint32_t count = luaL_optinteger(L, 2, -1);
  bcfx_setIndexBuffer(handle, start, count);
  return 0;
}
static int BCWRAP_FUNCTION(setTransform)(lua_State* L) {
  Mat4x4* mat = luaL_checkmat4x4(L, 1);

  bcfx_setTransform(mat);
  return 0;
}
static int BCWRAP_FUNCTION(setTexture)(lua_State* L) {
  uint8_t stage = luaL_checkinteger(L, 1);
  Handle sampler = (Handle)luaL_checkinteger(L, 2);
  Handle texture = (Handle)luaL_checkinteger(L, 3);
  uint32_t flags = luaL_checkinteger(L, 4);

  bcfx_setTexture(stage, sampler, texture, flags);
  return 0;
}
static int BCWRAP_FUNCTION(submit)(lua_State* L) {
  ViewId id = (ViewId)luaL_checkinteger(L, 1);
  Handle handle = (Handle)luaL_checkinteger(L, 2);

  bcfx_submit(id, handle);
  return 0;
}

static int BCWRAP_FUNCTION(destroy)(lua_State* L) {
  int top = lua_gettop(L);
  for (int idx = 1; idx <= top; idx++) {
    Handle handle = (Handle)luaL_checkinteger(L, idx);
    bcfx_destroy(handle);
  }
  return 0;
}

/* }====================================================== */

static const luaL_Enum BCWRAP_ENUM(clear_flag)[] = {
    {"NONE", BCFX_CLEAR_NONE},
    {"COLOR", BCFX_CLEAR_COLOR},
    {"DEPTH", BCFX_CLEAR_DEPTH},
    {"STENCIL", BCFX_CLEAR_STENCIL},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(vertex_attrib)[] = {
    {"Position", VA_Position},
    {"Normal", VA_Normal},
    {"Tangent", VA_Tangent},
    {"Bitangent", VA_Bitangent},
    {"Color0", VA_Color0},
    {"Color1", VA_Color1},
    {"Color2", VA_Color2},
    {"Color3", VA_Color3},
    {"Indices", VA_Indices},
    {"Weight", VA_Weight},
    {"TexCoord0", VA_TexCoord0},
    {"TexCoord1", VA_TexCoord1},
    {"TexCoord2", VA_TexCoord2},
    {"TexCoord3", VA_TexCoord3},
    {"TexCoord4", VA_TexCoord4},
    {"TexCoord5", VA_TexCoord5},
    {"TexCoord6", VA_TexCoord6},
    {"TexCoord7", VA_TexCoord7},
    {"Count", VA_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(attrib_type)[] = {
    {"Uint8", AT_Uint8},
    {"Uint10", AT_Uint10},
    {"Int16", AT_Int16},
    {"Half", AT_Half},
    {"Float", AT_Float},
    {"Count", AT_Count},
    {NULL, 0},
};
static const luaL_Enum BCWRAP_ENUM(shader_type)[] = {
    {"Vertex", ST_Vertex},
    {"Fragment", ST_Fragment},
    {NULL, 0},
};

#define EMPLACE_BCWRAP_FUNCTION(name) \
  { #name, BCWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    /* Static function features */
    EMPLACE_BCWRAP_FUNCTION(setThreadFuncs),
    EMPLACE_BCWRAP_FUNCTION(setSemFuncs),
    EMPLACE_BCWRAP_FUNCTION(setWinCtxFuncs),
    EMPLACE_BCWRAP_FUNCTION(setMiscFuncs),
    /* Basic APIs */
    EMPLACE_BCWRAP_FUNCTION(init),
    EMPLACE_BCWRAP_FUNCTION(apiFrame),
    EMPLACE_BCWRAP_FUNCTION(shutdown),
    /* Create Render Resource */
    EMPLACE_BCWRAP_FUNCTION(createVertexLayout),
    EMPLACE_BCWRAP_FUNCTION(createVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(createShader),
    EMPLACE_BCWRAP_FUNCTION(createProgram),
    EMPLACE_BCWRAP_FUNCTION(createUniform),
    EMPLACE_BCWRAP_FUNCTION(createTexture),
    /* View */
    EMPLACE_BCWRAP_FUNCTION(setViewWindow),
    EMPLACE_BCWRAP_FUNCTION(setViewClear),
    EMPLACE_BCWRAP_FUNCTION(setViewRect),
    /* Submit Drawcall */
    EMPLACE_BCWRAP_FUNCTION(setUniform),
    EMPLACE_BCWRAP_FUNCTION(setVertexBuffer),
    EMPLACE_BCWRAP_FUNCTION(setIndexBuffer),
    EMPLACE_BCWRAP_FUNCTION(setTransform),
    EMPLACE_BCWRAP_FUNCTION(setTexture),
    EMPLACE_BCWRAP_FUNCTION(submit),
    /* Create Render Resource */
    EMPLACE_BCWRAP_FUNCTION(destroy),

    {NULL, NULL},
};

LUAMOD_API int luaopen_libbcfx(lua_State* L) {
  luaL_newlib(L, wrap_funcs);

  REGISTE_ENUM(clear_flag);
  REGISTE_ENUM(vertex_attrib);
  REGISTE_ENUM(attrib_type);
  REGISTE_ENUM(shader_type);

  (void)VL_FUNCTION(init_metatable)(L);
  (void)COLOR_FUNCTION(init)(L);
  (void)MEMBUF_FUNCTION(init)(L);

  lua_createtable(L, 0, 3);
  (void)VECTOR_FUNCTION(init)(L);
  (void)MATRIX_FUNCTION(init)(L);
  (void)G3D_FUNCTION(init)(L);
  lua_setfield(L, -2, "math");

  (void)UTILS_FUNCTION(init)(L);

  init_resource_manage(L);

  return 1;
}
