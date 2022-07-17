#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#define BCFX_LIB

#include <bcfx.h>

// max bits is 12
#define BCFX_CONFIG_VIEW_ID_BITS 8
#define BCFX_CONFIG_PROGRAM_BITS 9

#define BCFX_UINT64_BITS (6) // 2^6 is 64bit
#define BCFX_VIEW_CAPTURE_UINT64S (1 << (BCFX_CONFIG_VIEW_ID_BITS - BCFX_UINT64_BITS))
#define BCFX_VIEW_CAPTURE_UINT64_MASK (BCFX_VIEW_CAPTURE_UINT64S - 1)
#define VIEW_UINT64_INDEX(id) ((id >> BCFX_UINT64_BITS) & BCFX_VIEW_CAPTURE_UINT64_MASK)
#define VIEW_OFFSET_BIT(id) (1 << (id & ((1 << BCFX_UINT64_BITS) - 1)))

// max number of render resource is 4096
// because current implementation using 12 bits for handle index
#define BCFX_CONFIG_MAX_VERTEX_BUFFER 4096
#define BCFX_CONFIG_MAX_INDEX_BUFFER 4096
#define BCFX_CONFIG_MAX_SHADER 512
#define BCFX_CONFIG_MAX_PROGRAM (1 << BCFX_CONFIG_PROGRAM_BITS)
#define BCFX_CONFIG_MAX_UNIFORM 512
#define BCFX_CONFIG_MAX_SAMPLER 512
#define BCFX_CONFIG_MAX_TEXTURE 4096
#define BCFX_CONFIG_MAX_FRAME_BUFFER 128
#define BCFX_CONFIG_MAX_INSTANCE_DATA_BUFFER 1024
#define BCFX_CONFIG_MAX_TEXTURE_BUFFER 1024

#define BCFX_CONFIG_MAX_VIEWS (1 << BCFX_CONFIG_VIEW_ID_BITS)
#define BCFX_CONFIG_MAX_DRAW_CALLS 65536
#define BCFX_CONFIG_MAX_VERTEX_STREAMS 4

#define BCFX_CONFIG_MAX_WINDOW 32

#define BCFX_CONFIG_MAX_UNIFORM_PER_PROGRAM 32

#define BCFX_CONFIG_MAX_TEXTURE_UNIT 8

#define BCFX_CONFIG_MAX_INSTANCE_DATA 5

#define BCFX_CONFIG_MAX_FRAME_BUFFER_ATTACHMENTS 8

#define BCFX_CONFIG_MAX_VIEW_CAPTURE 16

#define IS_VIEWID_VALID(id) (id < BCFX_CONFIG_MAX_VIEWS)

#define CHECK_VIEWID(id) assert(id < BCFX_CONFIG_MAX_VIEWS)
#define CHECK_STREAMID(id) assert(id < BCFX_CONFIG_MAX_VERTEX_STREAMS)
#define CHECK_TEXTURE_UNIT(stage) assert(stage < BCFX_CONFIG_MAX_TEXTURE_UNIT)

#define printf_err(...) (fprintf(stderr, ##__VA_ARGS__), fflush(stderr))

typedef enum {
  UB_ViewRect,
  UB_ViewTexel,
  UB_View,
  UB_InvView,
  UB_Proj,
  UB_InvProj,
  UB_ViewProj,
  UB_InvViewProj,
  UB_Model,
  UB_ModelView,
  UB_ModelViewProj,
  UB_AlphaRef,
  UB_Count,
} bcfx_EUniformBuiltin;

typedef struct {
  bcfx_EUniformType type;
  uint16_t num;
} UniformBase;
void uniform_initBase(UniformBase* u, bcfx_EUniformType type, uint16_t num);
size_t uniform_getSize(UniformBase* u);

uint8_t* uniform_writeData(luaL_ByteBuffer* b, bcfx_Handle handle, size_t sz);
uint8_t* uniform_readData(luaL_ByteBuffer* b, bcfx_Handle* phandle, size_t* psize, size_t* pread);

// WARNING: Change bcfx_EIndexType must Update texture_ToUniformType
typedef enum {
  TT_Texture1D,
  TT_Texture1DArray,
  TT_Texture2D,
  TT_Texture2DArray,
  TT_Texture3D,
  TT_TextureCubeMap,
  TT_Texture2DMipmap,
} bcfx_ETextureType;

typedef struct {
  bcfx_ETextureType type;
} TextureBase;

#endif /* _COMMON_H_ */
