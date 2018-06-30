#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define NVG_PI 3.14159265358979323846264338327f

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#endif

typedef struct NVGcontext NVGcontext;

struct NVGcolor {
    union {
        float rgba[4];
        struct {
            float r, g, b, a;
        };
    };
};
typedef struct NVGcolor NVGcolor;

struct NVGpaint {
    float    xform[6];
    float    extent[2];
    float    radius;
    float    feather;
    NVGcolor innerColor;
    NVGcolor outerColor;
    int      image;
};
typedef struct NVGpaint NVGpaint;

enum NVGwinding {
    NVG_CCW = 1, // Winding for solid shapes
    NVG_CW  = 2, // Winding for holes
};

enum NVGsolidity {
    NVG_SOLID = 1, // CCW
    NVG_HOLE  = 2, // CW
};

enum NVGlineCap {
    NVG_BUTT,
    NVG_ROUND,
    NVG_SQUARE,
    NVG_BEVEL,
    NVG_MITER,
};

enum NVGalign {
    // Horizontal align
    NVG_ALIGN_LEFT   = 1 << 0, // Default, align text horizontally to left.
    NVG_ALIGN_CENTER = 1 << 1, // Align text horizontally to center.
    NVG_ALIGN_RIGHT  = 1 << 2, // Align text horizontally to right.
    // Vertical align
    NVG_ALIGN_TOP      = 1 << 3, // Align text vertically to top.
    NVG_ALIGN_MIDDLE   = 1 << 4, // Align text vertically to middle.
    NVG_ALIGN_BOTTOM   = 1 << 5, // Align text vertically to bottom.
    NVG_ALIGN_BASELINE = 1 << 6, // Default, align text vertically to baseline.
};

enum NVGblendFactor {
    NVG_ZERO                = 1 << 0,
    NVG_ONE                 = 1 << 1,
    NVG_SRC_COLOR           = 1 << 2,
    NVG_ONE_MINUS_SRC_COLOR = 1 << 3,
    NVG_DST_COLOR           = 1 << 4,
    NVG_ONE_MINUS_DST_COLOR = 1 << 5,
    NVG_SRC_ALPHA           = 1 << 6,
    NVG_ONE_MINUS_SRC_ALPHA = 1 << 7,
    NVG_DST_ALPHA           = 1 << 8,
    NVG_ONE_MINUS_DST_ALPHA = 1 << 9,
    NVG_SRC_ALPHA_SATURATE  = 1 << 10,
};

enum NVGcompositeOperation {
    NVG_SOURCE_OVER,
    NVG_SOURCE_IN,
    NVG_SOURCE_OUT,
    NVG_ATOP,
    NVG_DESTINATION_OVER,
    NVG_DESTINATION_IN,
    NVG_DESTINATION_OUT,
    NVG_DESTINATION_ATOP,
    NVG_LIGHTER,
    NVG_COPY,
    NVG_XOR,
};

struct NVGcompositeOperationState {
    int srcRGB;
    int dstRGB;
    int srcAlpha;
    int dstAlpha;
};
typedef struct NVGcompositeOperationState NVGcompositeOperationState;

struct NVGglyphPosition {
    const char *str;        // Position of the glyph in the input string.
    float       x;          // The x-coordinate of the logical glyph position.
    float       minx, maxx; // The bounds of the glyph shape.
};
typedef struct NVGglyphPosition NVGglyphPosition;

struct NVGtextRow {
    const char *start;      // Pointer to the input text where the row starts.
    const char *end;        // Pointer to the input text where the row ends (one past the last character).
    const char *next;       // Pointer to the beginning of the next row.
    float       width;      // Logical width of the row.
    float       minx, maxx; // Actual bounds of the row. Logical with and bounds can differ because of kerning and some parts over extending.
};
typedef struct NVGtextRow NVGtextRow;

enum NVGimageFlags {
    NVG_IMAGE_GENERATE_MIPMAPS = 1 << 0, // Generate mipmaps during creation of the image.
    NVG_IMAGE_REPEATX          = 1 << 1, // Repeat image in X direction.
    NVG_IMAGE_REPEATY          = 1 << 2, // Repeat image in Y direction.
    NVG_IMAGE_FLIPY            = 1 << 3, // Flips (inverses) image in Y direction when rendered.
    NVG_IMAGE_PREMULTIPLIED    = 1 << 4, // Image data has premultiplied alpha.
    NVG_IMAGE_NEAREST          = 1 << 5, // Image interpolation is Nearest instead Linear
};

//
// Internal Render API
//
enum NVGtexture {
    NVG_TEXTURE_ALPHA = 0x01,
    NVG_TEXTURE_RGBA  = 0x02,
};

struct NVGscissor {
    float xform[6];
    float extent[2];
};
typedef struct NVGscissor NVGscissor;

struct NVGvertex {
    float x, y, u, v;
};
typedef struct NVGvertex NVGvertex;

struct NVGpath {
    int           first;
    int           count;
    unsigned char closed;
    int           nbevel;
    NVGvertex *   fill;
    int           nfill;
    NVGvertex *   stroke;
    int           nstroke;
    int           winding;
    int           convex;
};
typedef struct NVGpath NVGpath;

struct NVGparams {
    void *userPtr;
    int   edgeAntiAlias;
    int (*renderCreate)(void *uptr);
    int (*renderCreateTexture)(void *uptr, int type, int w, int h, int imageFlags, const unsigned char *data);
    int (*renderDeleteTexture)(void *uptr, int image);
    int (*renderUpdateTexture)(void *uptr, int image, int x, int y, int w, int h, const unsigned char *data);
    int (*renderGetTextureSize)(void *uptr, int image, int *w, int *h);
    void (*renderViewport)(void *uptr, float width, float height, float devicePixelRatio);
    void (*renderCancel)(void *uptr);
    void (*renderFlush)(void *uptr);
    void (*renderFill)(void *uptr, NVGpaint *paint, NVGcompositeOperationState compositeOperation, NVGscissor *scissor, float fringe, const float *bounds, const NVGpath *paths, int npaths);
    void (*renderStroke)(void *uptr, NVGpaint *paint, NVGcompositeOperationState compositeOperation, NVGscissor *scissor, float fringe, float strokeWidth, const NVGpath *paths, int npaths);
    void (*renderTriangles)(void *uptr, NVGpaint *paint, NVGcompositeOperationState compositeOperation, NVGscissor *scissor, const NVGvertex *verts, int nverts);
    void (*renderDelete)(void *uptr);
};
typedef struct NVGparams NVGparams;

#ifdef __cplusplus
}
#endif