#ifndef PRIVATE_H
#define PRIVATE_H

#include <stdint.h>
#include <stdio.h>

#include "sh4.h"
#include "types.h"
#include "aligned_vector.h"

#define MAX_TEXTURE_COUNT 768


#define GL_SCISSOR_TEST     0x0008
#define GL_NEAREST          0x2600
#define GL_LINEAR           0x2601
#define GL_OUT_OF_MEMORY    0x0505

#define GLushort   unsigned short
#define GLuint     unsigned int
#define GLenum     unsigned int
#define GLubyte    unsigned char
#define GLboolean  unsigned char

#define GL_FALSE   0
#define GL_TRUE    1


void glClearDepth(float depth);

GLuint gldcGenTexture(void);
void   gldcDeleteTexture(GLuint texture);
void   gldcBindTexture(GLuint texture);

/* Loads texture from SH4 RAM into PVR VRAM */
int  gldcAllocTexture(int w, int h, int format);
void gldcGetTexture(void** data, int* width, int* height);

void glViewport(int x, int y, int width, int height);
void glScissor( int x, int y, int width, int height);

void glKosInit();
void glKosSwapBuffers();


extern void* memcpy4 (void *dest, const void *src, size_t count);

#define GL_NO_INSTRUMENT inline __attribute__((no_instrument_function))
#define GL_INLINE_DEBUG GL_NO_INSTRUMENT __attribute__((always_inline))
#define GL_FORCE_INLINE static GL_INLINE_DEBUG
#define _GL_UNUSED(x) (void)(x)

#define TRACE_ENABLED 0
#define TRACE() if(TRACE_ENABLED) {fprintf(stderr, "%s\n", __func__);} (void) 0

typedef struct {
    unsigned int flags;      /* Constant PVR_CMD_USERCLIP */
    unsigned int d1, d2, d3; /* Ignored for this type */
    unsigned int sx,         /* Start x */
             sy,         /* Start y */
             ex,         /* End x */
             ey;         /* End y */
} PVRTileClipCommand; /* Tile Clip command for the pvr */

typedef struct {
    unsigned int list_type;
    AlignedVector vector;
} PolyList;

typedef struct {
    float x_plus_hwidth;
    float y_plus_hheight;
    float hwidth;  /* width * 0.5f */
    float hheight; /* height * 0.5f */
} Viewport;

extern Viewport VIEWPORT;

typedef struct {
    //0
    GLuint   index;
    GLuint   color; /* This is the PVR texture format */
    //8
    GLenum minFilter;
    GLenum magFilter;
    //16
    void *data;
    //20
    GLushort width;
    GLushort height;
    // 24
    GLushort  mipmap;  /* Bitmask of supplied mipmap levels */
    // 26
    GLubyte mipmap_bias;
    GLubyte _pad3;
    // 28
    GLushort _pad0;
    // 30
    GLubyte _pad1;
    GLubyte _pad2;
} __attribute__((aligned(32))) TextureObject;


GL_FORCE_INLINE void memcpy_vertex(Vertex *dest, const Vertex *src) {
#ifdef __DREAMCAST__
    _Complex float double_scratch;

    asm volatile (
        "fschg\n\t"
        "clrs\n\t"
        ".align 2\n\t"
        "fmov.d @%[in]+, %[scratch]\n\t"
        "fmov.d %[scratch], @%[out]\n\t"
        "fmov.d @%[in]+, %[scratch]\n\t"
        "add #8, %[out]\n\t"
        "fmov.d %[scratch], @%[out]\n\t"
        "fmov.d @%[in]+, %[scratch]\n\t"
        "add #8, %[out]\n\t"
        "fmov.d %[scratch], @%[out]\n\t"
        "fmov.d @%[in], %[scratch]\n\t"
        "add #8, %[out]\n\t"
        "fmov.d %[scratch], @%[out]\n\t"
        "fschg\n"
        : [in] "+&r" ((uint32_t) src), [scratch] "=&d" (double_scratch), [out] "+&r" ((uint32_t) dest)
        :
        : "t", "memory" // clobbers
    );
#else
    *dest = *src;
#endif
}

void _glInitContext();
void _glInitSubmissionTarget();
void _glInitTextures();

extern TextureObject* TEXTURE_ACTIVE;
extern GLboolean TEXTURES_ENABLED;

extern GLboolean DEPTH_TEST_ENABLED;
extern GLboolean DEPTH_MASK_ENABLED;

extern GLboolean CULLING_ENABLED;

extern GLboolean FOG_ENABLED;
extern GLboolean ALPHA_TEST_ENABLED;
extern GLboolean BLEND_ENABLED;

extern GLboolean SCISSOR_TEST_ENABLED;
extern GLenum SHADE_MODEL;
extern GLboolean AUTOSORT_ENABLED;


extern PolyList OP_LIST;
extern PolyList PT_LIST;
extern PolyList TR_LIST;

GL_FORCE_INLINE PolyList* _glActivePolyList() {
    if(BLEND_ENABLED) {
        return &TR_LIST;
    } else if(ALPHA_TEST_ENABLED) {
        return &PT_LIST;
    } else {
        return &OP_LIST;
    }
}

/* Memory allocation extension (GL_KOS_texture_memory_management) */
void glDefragmentTextureMemory_KOS(void);

GLuint _glFreeTextureMemory(void);
GLuint _glUsedTextureMemory(void);
GLuint _glFreeContiguousTextureMemory(void);

void _glApplyScissor(int force);

extern GLboolean STATE_DIRTY;


/* This is from KOS pvr_buffers.c */
#define PVR_MIN_Z 0.0001f

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CLAMP( X, _MIN, _MAX )  ( (X)<(_MIN) ? (_MIN) : ((X)>(_MAX) ? (_MAX) : (X)) )

#endif // PRIVATE_H
