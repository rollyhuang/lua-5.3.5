#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <common.h>
#include <frame.h>
#include <handle.h>
#include <sortkey.h>
#include <utils.h>

/*
** {======================================================
** RendererContext
** =======================================================
*/

typedef struct RendererContext RendererContext;

typedef void (*RendererInit)(RendererContext* ctx, Window mainWin);
typedef void (*RendererShutdown)(RendererContext* ctx);

typedef void (*RendererCreateVertexLayout)(RendererContext* ctx, Handle handle, const void* layout);
typedef void (*RendererCreateVertexBuffer)(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem, Handle layoutHandle);
typedef void (*RendererCreateIndexBuffer)(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem);
typedef void (*RendererCreateShader)(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem, ShaderType type);
typedef void (*RendererCreateProgram)(RendererContext* ctx, Handle handle, Handle vsh, Handle fsh);
typedef void (*RendererCreateUniform)(RendererContext* ctx, Handle handle, const char* name, bcfx_UniformType type, uint16_t num);
typedef void (*RendererCreateTexture)(RendererContext* ctx, Handle handle, bcfx_MemBuffer* mem, bcfx_ETextureFormat format);
typedef void (*RendererCreateFrameBuffer)(RendererContext* ctx, Handle handle, uint8_t num, Handle* handles);

typedef void (*RendererUpdateVertexBuffer)(RendererContext* ctx, Handle handle, size_t offset, bcfx_MemBuffer* mem);

typedef void (*RendererBeginFrame)(RendererContext* ctx);
typedef void (*RendererSubmit)(RendererContext* ctx, Frame* _render);
typedef void (*RendererEndFrame)(RendererContext* ctx);
typedef void (*RendererFlip)(RendererContext* ctx);

typedef void (*RendererDestroyVertexLayout)(RendererContext* ctx, Handle handle);
typedef void (*RendererDestroyVertexBuffer)(RendererContext* ctx, Handle handle);
typedef void (*RendererDestroyIndexBuffer)(RendererContext* ctx, Handle handle);
typedef void (*RendererDestroyShader)(RendererContext* ctx, Handle handle);
typedef void (*RendererDestroyProgram)(RendererContext* ctx, Handle handle);
typedef void (*RendererDestroyUniform)(RendererContext* ctx, Handle handle);
typedef void (*RendererDestroyTexture)(RendererContext* ctx, Handle handle);

struct RendererContext {
  RendererInit init;
  RendererShutdown shutdown;

  RendererCreateVertexLayout createVertexLayout;
  RendererCreateVertexBuffer createVertexBuffer;
  RendererCreateIndexBuffer createIndexBuffer;
  RendererCreateShader createShader;
  RendererCreateProgram createProgram;
  RendererCreateUniform createUniform;
  RendererCreateTexture createTexture;
  RendererCreateFrameBuffer createFrameBuffer;

  RendererUpdateVertexBuffer updateVertexBuffer;

  RendererBeginFrame beginFrame;
  RendererSubmit submit;
  RendererEndFrame endFrame;
  RendererFlip flip;

  RendererDestroyVertexLayout destroyVertexLayout;
  RendererDestroyVertexBuffer destroyVertexBuffer;
  RendererDestroyIndexBuffer destroyIndexBuffer;
  RendererDestroyShader destroyShader;
  RendererDestroyProgram destroyProgram;
  RendererDestroyUniform destroyUniform;
  RendererDestroyTexture destroyTexture;
};

typedef RendererContext* (*RendererCreator)(void);
typedef void (*RendererDestroyers)(RendererContext*);

/* }====================================================== */

#endif /* _RENDERER_H_ */
