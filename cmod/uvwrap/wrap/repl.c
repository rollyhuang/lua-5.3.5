#define repl_c
#include <uvwrap.h>

#ifndef _WIN32 /* { */

#include <readline/readline.h>
#include <readline/history.h>

#define RL_INIT()
#define READLINE(p) readline(p)
#define SAVELINE(line) add_history(line)
#define FREELINE(b) free(b)

#else /* }{ */

#define LUA_MAXINPUT 512
#define RL_INIT() char buffer[LUA_MAXINPUT]

#define READLINE(p) \
  (fputs(p, stdout), \
   fflush(stdout), /* show prompt */ \
   fgets(buffer, LUA_MAXINPUT, stdin)) /* get line */
#define SAVELINE(line) \
  { \
    (void)line; \
  }
#define FREELINE(b) \
  { \
    (void)b; \
  }

#endif /* } */

#if !defined(lua_assert)
#define lua_assert(x) ((void)0)
#endif

/*
** {======================================================
** Functions from lua.c
** =======================================================
*/

#define LUA_PROGNAME "lua"
#define LUA_PROMPT "> "
#define LUA_PROMPT2 ">> "

static const char* progname = LUA_PROGNAME;

/*
** Prints an error message, adding the program name in front of it
** (if present)
*/
static void l_message(const char* pname, const char* msg) {
  if (pname)
    lua_printf_err("%s: ", pname);
  lua_printf_err("%s\n", msg);
}

/*
** Check whether 'status' is not OK and, if so, prints the error
** message on the top of the stack. It assumes that the error object
** is a string, as it was either generated by Lua or by 'msghandler'.
*/
// [-(0|1), +0, -], need 0 slot
static int report(lua_State* L, int status) {
  if (status != LUA_OK) {
    const char* msg = lua_tostring(L, -1);
    l_message(progname, msg);
    lua_pop(L, 1); /* remove message */
  }
  return status;
}

/*
** Message handler used to run all chunks
*/
static int msghandler(lua_State* L) {
  const char* msg = lua_tostring(L, 1);
  if (msg == NULL) { /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") && /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING) /* that produces a string? */
      return 1; /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1); /* append a standard traceback */
  return 1; /* return the traceback */
}

/*
** Returns the string to be used as a prompt by the interpreter.
*/
// [-0, +1, -]
static const char* get_prompt(lua_State* L, int firstline) {
  const char* p;
  lua_getglobal(L, firstline ? "_PROMPT" : "_PROMPT2");
  p = lua_tostring(L, -1);
  if (p == NULL)
    p = (firstline ? LUA_PROMPT : LUA_PROMPT2);
  return p;
}

/* mark in error messages for incomplete statements */
#define EOFMARK "<eof>"
#define marklen (sizeof(EOFMARK) / sizeof(char) - 1)

/*
** Check whether 'status' signals a syntax error and the error
** message at the top of the stack ends with the above mark for
** incomplete statements.
*/
// [-(0|1), +0, -], need 0 slot
static int incomplete(lua_State* L, int status) {
  if (status == LUA_ERRSYNTAX) {
    size_t lmsg;
    const char* msg = lua_tolstring(L, -1, &lmsg);
    if (lmsg >= marklen && strcmp(msg + lmsg - marklen, EOFMARK) == 0) {
      lua_pop(L, 1);
      return 1;
    }
  }
  return 0; /* else... */
}

/*
** Prints (calling the Lua 'print' function) any values on the stack
*/
// [-n, +0, -]
static void l_print(lua_State* L) {
  int n = lua_gettop(L);
  if (n > 0) { /* any result to be printed? */
    luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
    lua_getglobal(L, "print");
    lua_insert(L, 1);
    if (lua_pcall(L, n, 0, 0) != LUA_OK) {
      l_message(progname, lua_pushfstring(L, "error calling 'print' (%s)", lua_tostring(L, -1)));
      lua_settop(L, 0);
    }
  }
}

// [-0, +0, -]
static void call_registry_funcs(lua_State* L, const char* name, const char* msg) {
  if (lua_getfield(L, LUA_REGISTRYINDEX, name) == LUA_TTABLE) {
    int idx = lua_gettop(L);
    lua_pushcfunction(L, msghandler);
    lua_pushnil(L);
    while (lua_next(L, idx)) {
      if (lua_pcall(L, 0, 0, idx + 1) != LUA_OK) {
        lua_printf_err(msg, lua_tostring(L, -1));
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);
  }
  lua_pop(L, 1);
}

/* }====================================================== */

static int docall(lua_State* L, int narg, int nres) {
  int base = lua_gettop(L) - narg; /* function index */
  lua_pushcfunction(L, msghandler); /* push message handler */
  lua_insert(L, base); /* put it under function and args */
  int status = lua_pcall(L, narg, nres, base);
  lua_remove(L, base); /* remove message handler from the stack */
  return status;
}

// [-1, +0, -], need 0 slot
static void keep_history(lua_State* L) {
  const char* line = lua_tostring(L, -1);
  if (line != NULL && line[0] != '\0') { /* non empty? */
    SAVELINE(line); /* keep history */
  }
  lua_pop(L, 1);
}

#define REPL_PROMPT "_REPL_PROMPT_"
#define REPL_OBJECT "_REPL_OBJECT_"
#define REPL_STRING "_REPL_STRING_"
#define REPL_ONSTOP "_REPL_ONSTOP_"
#define REPL_HOLDIT "_REPL_HOLDIT_"

#define CLEAR_REGISTRY_FIELD(field) \
  lua_pushnil(L); \
  lua_setfield(L, LUA_REGISTRYINDEX, field)

#define HOLD_REPL_OBJECT() \
  lua_getfield(L, LUA_REGISTRYINDEX, REPL_OBJECT); \
  lua_setfield(L, LUA_REGISTRYINDEX, REPL_HOLDIT)
#define UNHOLD_REPL_OBJECT() CLEAR_REGISTRY_FIELD(REPL_HOLDIT)

// [-0, +0, -]
static const char* get_prompt_hold(lua_State* L, int firstline) {
  const char* prmt = get_prompt(L, firstline);
  lua_setfield(L, LUA_REGISTRYINDEX, REPL_PROMPT);
  return prmt;
}

typedef struct {
  uv_thread_t tid[1];
  uv_async_t async[1];
  uv_sem_t sem[1];
  const char* prmt;
  char* buffer;
  lua_State* L;
  int firstline;
  int running;
  int waiting; // waiting for main thread process the script
} lua_REPL;

#define MSG_START "Multi thread REPL starting...\n"
#define MSG_STOP "Multi thread REPL end.\n"
static void read_line_thread(void* arg) {
  lua_REPL* repl = (lua_REPL*)arg;
  RL_INIT();
  lua_flushstring(MSG_START, sizeof(MSG_START));
  while (repl->running) { // for Ctrl-D in Unix or Ctrl-Z in Windows
    repl->buffer = READLINE(repl->prmt);
    if (!repl->running) { // for exit repl manually
      FREELINE(repl->buffer);
      break;
    }
    repl->waiting = 1;
    uv_async_send(repl->async);
    uv_sem_wait(repl->sem);
    repl->waiting = 0;
    FREELINE(repl->buffer);
  }
  lua_flushstring(MSG_STOP, sizeof(MSG_STOP));
}

// [-0, +0, -]
void end_multi_line(lua_State* L, lua_REPL* repl) {
  repl->firstline = 1;
  lua_getfield(L, LUA_REGISTRYINDEX, REPL_STRING);
  keep_history(L);
  CLEAR_REGISTRY_FIELD(REPL_STRING);
  lua_writeline();
}

// [-0, +2, -]
int compile_source_code(lua_State* L, lua_REPL* repl) {
  char* b = repl->buffer;
  size_t l = strlen(b);
  if (l > 0 && b[l - 1] == '\n') { /* line ends with newline? */
    b[--l] = '\0'; /* remove it */
  }
  if (repl->firstline) {
    if (b[0] == '=') { /* for compatibility with 5.2, ... */
      lua_pushfstring(L, "return %s", b + 1); /* change '=' to 'return' */
    } else {
      lua_pushlstring(L, b, l);
      const char* retline = lua_pushfstring(L, "return %s;", b);
      if (luaL_loadbuffer(L, retline, strlen(retline), "=stdin") == LUA_OK) {
        lua_remove(L, -2); /* remove modified line */
        return LUA_OK;
      }
      lua_pop(L, 2); /* pop result from 'luaL_loadbuffer' and modified line */
    }
  } else {
    lua_getfield(L, LUA_REGISTRYINDEX, REPL_STRING);
    lua_pushliteral(L, "\n"); /* add newline... */
    lua_pushlstring(L, b, l);
    lua_concat(L, 3); /* join them */
    CLEAR_REGISTRY_FIELD(REPL_STRING);
  }
  size_t len = 0;
  const char* line = lua_tolstring(L, -1, &len);
  return luaL_loadbuffer(L, line, len, "=stdin"); /* try it */
}

// [-0, +0, -]
void deal_source_code(lua_State* L, lua_REPL* repl) {
  int status = compile_source_code(L, repl);
  if (incomplete(L, status)) { // incomplete will pop the error message
    lua_setfield(L, LUA_REGISTRYINDEX, REPL_STRING);
    repl->firstline = 0;
    return;
  }
  repl->firstline = 1;
  lua_insert(L, -2);
  keep_history(L); // will pop the source string
  if (status == LUA_OK) {
    status = docall(L, 0, LUA_MULTRET);
    if (status == LUA_OK) {
      l_print(L);
    }
  }
  report(L, status); // will pop the error message
  call_registry_funcs(L, LUA_ATREPL, "Call atrepl failed: %s\n");
}

static void exit_repl(lua_State* L, lua_REPL* repl) {
  repl->running = 0;
  uv_sem_post(repl->sem); // for Ctrl-D or run 'repl_stop' in terminal by type it
  uv_thread_join(repl->tid);
  uv_sem_destroy(repl->sem);
  uv_close((uv_handle_t*)repl->async, NULL);

  lua_getfield(L, LUA_REGISTRYINDEX, REPL_ONSTOP);
  if (lua_isfunction(L, -1)) {
    report(L, docall(L, 0, 0));
  } else {
    lua_pop(L, 1);
  }

  CLEAR_REGISTRY_FIELD(REPL_PROMPT);
  CLEAR_REGISTRY_FIELD(REPL_OBJECT);
  CLEAR_REGISTRY_FIELD(REPL_STRING);
  CLEAR_REGISTRY_FIELD(REPL_ONSTOP);
}

static int lua_doREPL(lua_State* L) {
  lua_REPL* repl = (lua_REPL*)luaL_checklightuserdata(L, 1);
  if (repl->buffer == NULL && repl->firstline) { // Ctrl-D
    lua_writeline();
    exit_repl(L, repl);
    return 0;
  }
  lua_settop(L, 0);
  HOLD_REPL_OBJECT();
  if (repl->buffer == NULL) {
    end_multi_line(L, repl);
  } else {
    deal_source_code(L, repl);
  }
  if (repl->running) { // maybe stop repl during run the script
    repl->prmt = get_prompt_hold(L, repl->firstline);
    uv_sem_post(repl->sem);
  }
  UNHOLD_REPL_OBJECT();
  lua_assert(0 == lua_gettop(L));
  return 0;
}

static void async_doREPL(uv_async_t* handle) {
  lua_REPL* repl = (lua_REPL*)uv_handle_get_data((uv_handle_t*)handle);
  lua_State* L = repl->L;
#ifndef NDEBUG
  int top = lua_gettop(L);
#endif
  lua_checkstack(L, 3);
  lua_pushcfunction(L, msghandler);
  int base = lua_gettop(L);
  lua_pushcfunction(L, lua_doREPL);
  lua_pushlightuserdata(L, (void*)repl);
  report(L, lua_pcall(L, 1, 0, base));
  lua_pop(L, 1);
  lua_assert(top == lua_gettop(L));
}

int uvwrap_repl_start(lua_State* L) {
  uv_loop_t* loop = luaL_checkuvloop(L, 1);
  lua_settop(L, 2);
  lua_setfield(L, LUA_REGISTRYINDEX, REPL_ONSTOP);

  lua_REPL* repl = (lua_REPL*)lua_newuserdata(L, sizeof(lua_REPL));
  uv_async_init(loop, repl->async, async_doREPL);
  uv_sem_init(repl->sem, 0);
  repl->buffer = NULL;
  repl->L = L;
  repl->firstline = 1;
  repl->running = 1;
  repl->waiting = 0;
  repl->prmt = get_prompt_hold(L, repl->firstline);
  uv_handle_set_data((uv_handle_t*)repl->async, (void*)repl);
  signal(SIGINT, SIG_DFL); /* reset C-signal handler */
  uv_thread_create(repl->tid, read_line_thread, (void*)repl);

  lua_setfield(L, LUA_REGISTRYINDEX, REPL_OBJECT);
  return 0;
}

#define EXIT_MESSAGE "Press Enter to exit multi thread REPL"

int uvwrap_repl_stop(lua_State* L) {
  lua_getfield(L, LUA_REGISTRYINDEX, REPL_OBJECT);
  if (lua_isuserdata(L, -1)) {
    lua_REPL* repl = (lua_REPL*)lua_touserdata(L, -1);
    if (!repl->waiting) {
      lua_writeline();
      lua_writestring(EXIT_MESSAGE, sizeof(EXIT_MESSAGE));
      fflush(stdout);
    }
    exit_repl(L, repl);
  }
  return 0;
}
