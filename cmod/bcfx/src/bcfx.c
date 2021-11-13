#define _bcfx_c_

#include <common.h>
#include <context.h>

// clang-format off
uint8_t sizeof_DataType[] = {
    1,
#define XX(name, type) sizeof(type),
    DATA_TYPE_MAP(XX)
#undef XX
    2,
    sizeof(float),
    0,
};
// clang-format on

/*
** {======================================================
** Vertex Layout
** =======================================================
*/

uint8_t sizeof_AttribType[] = {
    1,
    sizeof(GLubyte), // GL_UNSIGNED_BYTE
    sizeof(GLuint), // GL_UNSIGNED_INT_10_10_10_2
    sizeof(GLshort), // GL_SHORT
    sizeof(GLhalf), // GL_HALF_FLOAT
    sizeof(GLfloat), // GL_FLOAT
    0,
};

BCFX_API void bcfx_VL_init(bcfx_VertexLayout* layout) {
  memset((void*)layout, 0, sizeof(bcfx_VertexLayout));
}

BCFX_API void bcfx_VL_add(bcfx_VertexLayout* layout, bcfx_EVertexAttrib attrib, uint8_t num, bcfx_EAttribType type, bool normalized) {
  assert(num >= 1 && num <= 4);
  bcfx_Attrib* att = &layout->attributes[attrib];
  att->num = num;
  att->type = type;
  att->normal = normalized;
  layout->offset[attrib] = layout->stride;
  layout->stride += sizeof_AttribType[type] * num;
}

BCFX_API void bcfx_VL_skip(bcfx_VertexLayout* layout, uint8_t num_byte) {
  layout->stride += num_byte;
}

/* }====================================================== */

/*
** {======================================================
** Basic API
** =======================================================
*/

static Context s_ctx[1];

BCFX_API uint32_t bcfx_frameId(void) {
  return ctx_frameId(s_ctx);
}

BCFX_API void bcfx_setFrameCompletedCallback(bcfx_OnFrameCompleted cb, void* ud) {
  ctx_setFrameCompletedCallback(s_ctx, cb, ud);
}

BCFX_API void bcfx_init(Window mainWin) {
  ctx_init(s_ctx, mainWin);
}

BCFX_API void bcfx_apiFrame(uint32_t renderCount) {
  ctx_apiFrame(s_ctx, renderCount);
}

BCFX_API void bcfx_shutdowm(void) {
  ctx_shutdowm(s_ctx);
}

/* }====================================================== */

/*
** {======================================================
** Create Render Resource
** =======================================================
*/

BCFX_API Handle bcfx_createVertexLayout(bcfx_VertexLayout* layout) {
  return ctx_createVertexLayout(s_ctx, layout);
}

BCFX_API Handle bcfx_createVertexBuffer(bcfx_MemBuffer* mem, Handle handle) {
  return ctx_createVertexBuffer(s_ctx, mem, handle);
}

BCFX_API Handle bcfx_createIndexBuffer(bcfx_MemBuffer* mem) {
  return ctx_createIndexBuffer(s_ctx, mem);
}

BCFX_API Handle bcfx_createShader(bcfx_MemBuffer* mem, ShaderType type) {
  return ctx_createShader(s_ctx, mem, type);
}

BCFX_API Handle bcfx_createProgram(Handle vs, Handle fs) {
  return ctx_createProgram(s_ctx, vs, fs);
}

BCFX_API Handle bcfx_createUniform(const char* name, bcfx_UniformType type, uint16_t num) {
  return ctx_createUniform(s_ctx, name, type, num);
}

BCFX_API Handle bcfx_createTexture(bcfx_MemBuffer* mem) {
  return ctx_createTexture(s_ctx, mem);
}

/* }====================================================== */

/*
** {======================================================
** Destroy Render Resource
** =======================================================
*/

BCFX_API void bcfx_destroy(Handle handle) {
  ctx_destroy(s_ctx, handle);
}

/* }====================================================== */

/*
** {======================================================
** View
** =======================================================
*/

BCFX_API void bcfx_setViewWindow(ViewId id, Window win) {
  ctx_setViewWindow(s_ctx, id, win);
}

BCFX_API void bcfx_setViewFrameBuffer(ViewId id, Handle handle) {
  ctx_setViewFrameBuffer(s_ctx, id, handle);
}

BCFX_API void bcfx_setViewClear(ViewId id, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
  ctx_setViewClear(s_ctx, id, flags, rgba, depth, stencil);
}

BCFX_API void bcfx_setViewClearRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewClearRect(s_ctx, id, x, y, width, height);
}

BCFX_API void bcfx_setViewRect(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewRect(s_ctx, id, x, y, width, height);
}

BCFX_API void bcfx_setViewScissor(ViewId id, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  ctx_setViewScissor(s_ctx, id, x, y, width, height);
}

BCFX_API void bcfx_setViewTransform(ViewId id, Mat4x4* viewMat, Mat4x4* projMat) {
  ctx_setViewTransform(s_ctx, id, viewMat, projMat);
}

BCFX_API void bcfx_setViewMode(ViewId id, ViewMode mode) {
  ctx_setViewMode(s_ctx, id, mode);
}

BCFX_API void bcfx_resetView(ViewId id) {
  ctx_resetView(s_ctx, id);
}

/* }====================================================== */

/*
** {======================================================
** Submit drawcall
** =======================================================
*/

BCFX_API void bcfx_setUniformVec4(Handle handle, Vec4* vec, uint16_t num) {
  ctx_setUniformVec4(s_ctx, handle, vec, num);
}
BCFX_API void bcfx_setUniformMat3x3(Handle handle, Mat3x3* mat, uint16_t num) {
  ctx_setUniformMat3x3(s_ctx, handle, mat, num);
}
BCFX_API void bcfx_setUniformMat4x4(Handle handle, Mat4x4* mat, uint16_t num) {
  ctx_setUniformMat4x4(s_ctx, handle, mat, num);
}

BCFX_API void bcfx_touch(ViewId id) {
  ctx_touch(s_ctx, id);
}

BCFX_API void bcfx_setVertexBuffer(uint8_t stream, Handle handle) {
  ctx_setVertexBuffer(s_ctx, stream, handle);
}
BCFX_API void bcfx_setIndexBuffer(Handle handle, uint32_t start, uint32_t count) {
  ctx_setIndexBuffer(s_ctx, handle, start, count);
}
BCFX_API void bcfx_setTransform(Mat4x4* mat) {
  ctx_setTransform(s_ctx, mat);
}
BCFX_API void bcfx_setTexture(uint8_t stage, Handle sampler, Handle texture, uint32_t flags) {
  ctx_setTexture(s_ctx, stage, sampler, texture, flags);
}

BCFX_API void bcfx_submit(ViewId id, Handle handle) {
  ctx_submit(s_ctx, id, handle);
}

/* }====================================================== */
