/*
** $Id: lauxlib.h,v 1.131.1.1 2017/04/19 17:20:42 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/

#ifndef lauxlib_h
#define lauxlib_h

#include <stddef.h>
#include <stdio.h>

#include "lua.h"

/* extra error code for 'luaL_loadfilex' */
#define LUA_ERRFILE (LUA_ERRERR + 1)

/* key, in the registry, for table of loaded modules */
#define LUA_LOADED_TABLE "_LOADED"

/* key, in the registry, for table of preloaded loaders */
#define LUA_PRELOAD_TABLE "_PRELOAD"

typedef struct luaL_Reg {
  const char* name;
  lua_CFunction func;
} luaL_Reg;

typedef struct luaL_Enum {
  const char* name;
  lua_Integer value;
} luaL_Enum;

#define LUAL_NUMSIZES (sizeof(lua_Integer) * 16 + sizeof(lua_Number))

LUALIB_API void(luaL_checkversion_)(lua_State* L, lua_Number ver, size_t sz);
#define luaL_checkversion(L) luaL_checkversion_(L, LUA_VERSION_NUM, LUAL_NUMSIZES)

LUALIB_API int(luaL_getmetafield)(lua_State* L, int obj, const char* e);
LUALIB_API int(luaL_callmeta)(lua_State* L, int obj, const char* e);
LUALIB_API const char*(luaL_tolstring)(lua_State* L, int idx, size_t* len);
LUALIB_API const char*(luaL_tolstringex)(lua_State* L, int idx, size_t* len, int level);
LUALIB_API size_t(luaL_escape)(char* dst, const char* str, size_t len);
LUALIB_API int(luaL_isvar)(const char* str, size_t len);
LUALIB_API int(luaL_argerror)(lua_State* L, int arg, const char* extramsg);
LUALIB_API const char*(luaL_checklstring)(lua_State* L, int arg, size_t* l);
LUALIB_API const char*(luaL_optlstring)(lua_State* L, int arg, const char* def, size_t* l);
LUALIB_API lua_Number(luaL_checknumber)(lua_State* L, int arg);
LUALIB_API lua_Number(luaL_optnumber)(lua_State* L, int arg, lua_Number def);

LUALIB_API lua_Integer(luaL_checkinteger)(lua_State* L, int arg);
LUALIB_API lua_Integer(luaL_optinteger)(lua_State* L, int arg, lua_Integer def);

LUALIB_API void(luaL_checkstack)(lua_State* L, int sz, const char* msg);
LUALIB_API void(luaL_checktype)(lua_State* L, int arg, int t);
LUALIB_API void(luaL_checkany)(lua_State* L, int arg);

LUALIB_API int(luaL_newmetatable)(lua_State* L, const char* tname);
LUALIB_API void(luaL_setmetatable)(lua_State* L, const char* tname);
LUALIB_API void*(luaL_testudata)(lua_State* L, int ud, const char* tname);
LUALIB_API void*(luaL_checkudata)(lua_State* L, int ud, const char* tname);
LUALIB_API void*(luaL_testudata_recursive)(lua_State* L, int ud, const char* tname);
LUALIB_API void*(luaL_checkudata_recursive)(lua_State* L, int ud, const char* tname);

LUALIB_API void(luaL_where)(lua_State* L, int lvl);
LUALIB_API int(luaL_error)(lua_State* L, const char* fmt, ...);

LUALIB_API int(luaL_checkoption)(lua_State* L, int arg, const char* def, const char* const lst[]);

LUALIB_API int(luaL_fileresult)(lua_State* L, int stat, const char* fname);
LUALIB_API int(luaL_execresult)(lua_State* L, int stat);

/* predefined references */
#define LUA_NOREF (-2)
#define LUA_REFNIL (-1)

LUALIB_API int(luaL_ref)(lua_State* L, int t);
LUALIB_API void(luaL_unref)(lua_State* L, int t, int ref);

LUALIB_API int(luaL_loadfilex)(lua_State* L, const char* filename, const char* mode);

#define luaL_loadfile(L, f) luaL_loadfilex(L, f, NULL)

LUALIB_API int(luaL_loadbufferx)(lua_State* L, const char* buff, size_t sz, const char* name, const char* mode);
LUALIB_API int(luaL_loadstring)(lua_State* L, const char* s);

LUALIB_API lua_State*(luaL_newstate)(void);

LUALIB_API lua_Integer(luaL_len)(lua_State* L, int idx);

LUALIB_API const char*(luaL_gsub)(lua_State* L, const char* s, const char* p, const char* r);

LUALIB_API void(luaL_setfuncs)(lua_State* L, const luaL_Reg* l, int nup);

LUALIB_API void(luaL_setenums)(lua_State* L, const luaL_Enum* l);

LUALIB_API void(luaL_setenums_r)(lua_State* L, const luaL_Enum* l);

LUALIB_API int(luaL_getsubtable)(lua_State* L, int idx, const char* fname);

LUALIB_API void(luaL_traceback)(lua_State* L, lua_State* L1, const char* msg, int level);

LUALIB_API void(luaL_ptraceback)(lua_State* L);

LUALIB_API void(luaL_pstack)(lua_State* L, int level);

LUALIB_API void(luaL_pstackdepth)(lua_State* L);

LUALIB_API void(luaL_plocal)(lua_State* L, int level);

LUALIB_API void(luaL_pupvalue)(lua_State* L, int level, int idx);

LUALIB_API void(luaL_pfuncinfo)(lua_State* L, int level, int idx, int recursive);

LUALIB_API void(luaL_pinject)(lua_State* L, const char* source, int level);

LUALIB_API void(luaL_breakpoint)(lua_State* L);

LUALIB_API void(luaL_requiref)(lua_State* L, const char* modname, lua_CFunction openf, int glb);

LUALIB_API const char*(luaL_protoinfo)(lua_State* L, int idx, int recursive, const char* options);

LUALIB_API int(luaL_inject)(lua_State* L, const char* source, int level);

LUALIB_API lua_State*(luaL_newstate_z)(void);
LUALIB_API void(luaL_close_z)(lua_State* L);

#define LUA_ATEXIT "_atexit_"
LUALIB_API void(luaL_atexit)(lua_State* L);

#define LUA_ATREPL "_atrepl_"
LUALIB_API void(luaL_atrepl)(lua_State* L);

LUALIB_API void luaL_printbuffer(lua_State* L);

LUALIB_API void luaL_printfinish(lua_State* L);

/*
** ===============================================================
** some useful macros
** ===============================================================
*/

// [-0, +1]
#define luaL_newlibtable(L, l) lua_createtable(L, 0, sizeof(l) / sizeof((l)[0]) - 1)
// [-0, +1]
#define luaL_newlib(L, l) (luaL_checkversion(L), luaL_newlibtable(L, l), luaL_setfuncs(L, l, 0))

#define luaL_newenum(L, l) (luaL_checkversion(L), luaL_newlibtable(L, l), luaL_setenums(L, l))
#define luaL_newenum_r(L, l) (luaL_checkversion(L), luaL_newlibtable(L, l), luaL_setenums_r(L, l))

#define luaL_argcheck(L, cond, arg, extramsg) ((void)((cond) || luaL_argerror(L, (arg), (extramsg))))
#define luaL_checkstring(L, n) (luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L, n, d) (luaL_optlstring(L, (n), (d), NULL))

#define luaL_typename(L, i) lua_typename(L, lua_type(L, (i)))

#define luaL_dofile(L, fn) (luaL_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_dostring(L, s) (luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_getmetatable(L, n) (lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_opt(L, f, n, d) (lua_isnoneornil(L, (n)) ? (d) : f(L, (n)))

#define luaL_loadbuffer(L, s, sz, n) luaL_loadbufferx(L, s, sz, n, NULL)

#define luaL_checklightuserdata(L, idx) (luaL_checktype(L, idx, LUA_TLIGHTUSERDATA), lua_touserdata(L, idx))
#define luaL_optlightuserdata(L, idx, dft) luaL_opt(L, luaL_checklightuserdata, idx, dft)

#define luaL_checkboolean(L, idx) (luaL_checktype(L, idx, LUA_TBOOLEAN), lua_toboolean(L, idx))
#define luaL_optboolean(L, idx, dft) luaL_opt(L, luaL_checkboolean, idx, dft)

/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/

typedef struct luaL_Buffer {
  char* b; /* buffer address */
  size_t size; /* buffer size */
  size_t n; /* number of characters in buffer */
  lua_State* L;
  char initb[LUAL_BUFFERSIZE]; /* initial buffer */
} luaL_Buffer;

#define luaL_addchar(B, c) ((void)((B)->n < (B)->size || luaL_prepbuffsize((B), 1)), ((B)->b[(B)->n++] = (c)))

#define luaL_addsize(B, s) ((B)->n += (s))

LUALIB_API void(luaL_buffinit)(lua_State* L, luaL_Buffer* B);
LUALIB_API char*(luaL_prepbuffsize)(luaL_Buffer* B, size_t sz);
LUALIB_API void(luaL_addlstring)(luaL_Buffer* B, const char* s, size_t l);
LUALIB_API void(luaL_addstring)(luaL_Buffer* B, const char* s);
LUALIB_API void(luaL_addvalue)(luaL_Buffer* B);
LUALIB_API void(luaL_pushresult)(luaL_Buffer* B);
LUALIB_API void(luaL_pushresultsize)(luaL_Buffer* B, size_t sz);
LUALIB_API char*(luaL_buffinitsize)(lua_State* L, luaL_Buffer* B, size_t sz);

#define luaL_prepbuffer(B) luaL_prepbuffsize(B, LUAL_BUFFERSIZE)

/* }====================================================== */

/*
** {======================================================
** File handles for IO library
** =======================================================
*/

/*
** A file handle is a userdata with metatable 'LUA_FILEHANDLE' and
** initial structure 'luaL_Stream' (it may contain other fields
** after that initial structure).
*/

#define LUA_FILEHANDLE "FILE*"

typedef struct luaL_Stream {
  FILE* f; /* stream (NULL for incompletely created streams) */
  lua_CFunction closef; /* to close stream (NULL for closed streams) */
} luaL_Stream;

/* }====================================================== */

/* compatibility with old module system */
#if defined(LUA_COMPAT_MODULE)

LUALIB_API void(luaL_pushmodule)(lua_State* L, const char* modname, int sizehint);
LUALIB_API void(luaL_openlib)(lua_State* L, const char* libname, const luaL_Reg* l, int nup);

#define luaL_register(L, n, l) (luaL_openlib(L, (n), (l), 0))

#endif

/*
** {==================================================================
** "Abstraction Layer" for basic report of messages and errors
** ===================================================================
*/

/* print a string */
#if !defined(lua_writestring)
#define lua_writestring(s, l) fwrite((s), sizeof(char), (l), stdout)
#endif

/* flush a string */
#if !defined(lua_flushstring)
#define lua_flushstring(s, l) (fwrite((s), sizeof(char), (l), stdout), fflush(stdout))
#endif

/* print a newline and flush the output */
#if !defined(lua_writeline)
#define lua_writeline() (lua_writestring("\n", 1), fflush(stdout))
#endif

/* print format string */
#if !defined(lua_printf)
#define lua_printf(...) (fprintf(stdout, ##__VA_ARGS__), fflush(stdout))
#endif

/* print a string to stderr */
#if !defined(lua_writestring_err)
#define lua_writestring_err(s, l) fwrite((s), sizeof(char), (l), stderr)
#endif

/* flush a string to stderr */
#if !defined(lua_flushstring_err)
#define lua_flushstring_err(s, l) (fwrite((s), sizeof(char), (l), stderr), fflush(stderr))
#endif

/* print a newline and flush the output to stderr */
#if !defined(lua_writeline_err)
#define lua_writeline_err() (lua_writestring_err("\n", 1), fflush(stderr))
#endif

/* print format string to stderr */
#if !defined(lua_printf_err)
#define lua_printf_err(...) (fprintf(stderr, ##__VA_ARGS__), fflush(stderr))
#endif

/* }================================================================== */

/*
** {============================================================
** Compatibility with deprecated conversions
** =============================================================
*/
#if defined(LUA_COMPAT_APIINTCASTS)

#define luaL_checkunsigned(L, a) ((lua_Unsigned)luaL_checkinteger(L, a))
#define luaL_optunsigned(L, a, d) ((lua_Unsigned)luaL_optinteger(L, a, (lua_Integer)(d)))

#define luaL_checkint(L, n) ((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L, n, d) ((int)luaL_optinteger(L, (n), (d)))

#define luaL_checklong(L, n) ((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L, n, d) ((long)luaL_optinteger(L, (n), (d)))

#endif
/* }============================================================ */

#endif
