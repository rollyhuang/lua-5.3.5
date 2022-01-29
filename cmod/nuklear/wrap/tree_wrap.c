#define _tree_wrap_c_
#include <nuklear_wrap.h>

/*
** {======================================================
** Nuklear Tree
** =======================================================
*/

// nk_tree_push
// nk_tree_push_id
static int NKWRAP_FUNCTION(tree_push_hashed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_tree_type type = luaL_checknktreetype(L, 2);
  const char* title = luaL_checkstring(L, 3);
  nk_collapse_states initialState = luaL_checknkcollapsestates(L, 4);
  size_t sz;
  const char* hash = luaL_checklstring(L, 5, &sz);
  int seed = luaL_checkinteger(L, 6);

  nk_bool ret = nk_tree_push_hashed(ctx, type, title, initialState, hash, sz, seed);
  lua_pushboolean(L, (int)ret);
  return 1;
}
// nk_tree_image_push
// nk_tree_image_push_id
static int NKWRAP_FUNCTION(tree_image_push_hashed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_tree_type type = luaL_checknktreetype(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);
  const char* title = luaL_checkstring(L, 4);
  nk_collapse_states initialState = luaL_checknkcollapsestates(L, 5);
  size_t sz;
  const char* hash = luaL_checklstring(L, 6, &sz);
  int seed = luaL_checkinteger(L, 7);

  nk_bool ret = nk_tree_image_push_hashed(ctx, type, *img, title, initialState, hash, sz, seed);
  lua_pushboolean(L, (int)ret);
  return 1;
}
static int NKWRAP_FUNCTION(tree_pop)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_tree_pop(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(tree_state_push)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_tree_type type = luaL_checknktreetype(L, 2);
  const char* title = luaL_checkstring(L, 3);
  nk_collapse_states state = luaL_checknkcollapsestates(L, 4);

  nk_bool ret = nk_tree_state_push(ctx, type, title, &state);
  lua_pushboolean(L, (int)ret);
  lua_pushboolean(L, (int)state);
  return 2;
}
static int NKWRAP_FUNCTION(tree_state_image_push)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_tree_type type = luaL_checknktreetype(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);
  const char* title = luaL_checkstring(L, 4);
  nk_collapse_states state = luaL_checknkcollapsestates(L, 5);

  nk_bool ret = nk_tree_state_image_push(ctx, type, *img, title, &state);
  lua_pushboolean(L, (int)ret);
  lua_pushboolean(L, (int)state);
  return 2;
}
static int NKWRAP_FUNCTION(tree_state_pop)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_tree_state_pop(ctx);
  return 0;
}
static int NKWRAP_FUNCTION(tree_element_push_hashed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_tree_type type = luaL_checknktreetype(L, 2);
  const char* title = luaL_checkstring(L, 3);
  nk_collapse_states initialState = luaL_checknkcollapsestates(L, 4);
  nk_bool selected = luaL_checknkbool(L, 5);
  size_t sz;
  const char* hash = luaL_checklstring(L, 6, &sz);
  int seed = luaL_checkinteger(L, 7);

  nk_bool ret = nk_tree_element_push_hashed(ctx, type, title, initialState, &selected, hash, sz, seed);
  lua_pushboolean(L, (int)ret);
  lua_pushboolean(L, (int)selected);
  return 2;
}
static int NKWRAP_FUNCTION(tree_element_image_push_hashed)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);
  nk_tree_type type = luaL_checknktreetype(L, 2);
  nk_image* img = luaL_checknkimage(L, 3);
  const char* title = luaL_checkstring(L, 4);
  nk_collapse_states initialState = luaL_checknkcollapsestates(L, 5);
  nk_bool selected = luaL_checknkbool(L, 6);
  size_t sz;
  const char* hash = luaL_checklstring(L, 7, &sz);
  int seed = luaL_checkinteger(L, 8);

  nk_bool ret = nk_tree_element_image_push_hashed(ctx, type, *img, title, initialState, &selected, hash, sz, seed);
  lua_pushboolean(L, (int)ret);
  lua_pushboolean(L, (int)selected);
  return 2;
}
static int NKWRAP_FUNCTION(tree_element_pop)(lua_State* L) {
  nk_context* ctx = luaL_checkcontext(L, 1);

  nk_tree_element_pop(ctx);
  return 0;
}

/* }====================================================== */

#define EMPLACE_NKWRAP_FUNCTION(name) \
  { #name, NKWRAP_FUNCTION(name) }
static const luaL_Reg wrap_funcs[] = {
    EMPLACE_NKWRAP_FUNCTION(tree_push_hashed),
    EMPLACE_NKWRAP_FUNCTION(tree_image_push_hashed),
    EMPLACE_NKWRAP_FUNCTION(tree_pop),
    EMPLACE_NKWRAP_FUNCTION(tree_state_push),
    EMPLACE_NKWRAP_FUNCTION(tree_state_image_push),
    EMPLACE_NKWRAP_FUNCTION(tree_state_pop),
    EMPLACE_NKWRAP_FUNCTION(tree_element_push_hashed),
    EMPLACE_NKWRAP_FUNCTION(tree_element_image_push_hashed),
    EMPLACE_NKWRAP_FUNCTION(tree_element_pop),
    {NULL, NULL},
};

static const luaL_Enum NKWRAP_ENUM(tree_type)[] = {
    {"NODE", NK_TREE_NODE},
    {"TAB", NK_TREE_TAB},
    {NULL, 0},
};

void NKWRAP_FUNCTION(init_tree)(lua_State* L) {
  luaL_setfuncs(L, wrap_funcs, 0);

  REGISTE_ENUM_NKWRAP(tree_type);
}
