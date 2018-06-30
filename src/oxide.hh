#pragma once

#include "include/nanovgdef.h"

#ifdef _MSC_VER
struct HDC__;
using HDC = HDC__ *;
struct HGLRC__;
using HGLRC = HGLRC__ *;

struct HWND__;
using HWND = HWND__ *;
#else
using HWND = void *;
#define __stdcall
#endif

class Oxide {
    friend class OxideSurface;
    friend class OxideNVG;

#ifdef _MSC_VER
    HDC   dc;
    HGLRC rc;
#endif
    HWND window_handle;
    HWND target_handle;

#ifdef _MSC_VER
    static void *__stdcall window_proc(HWND hwnd, unsigned message, size_t w_param, size_t l_param);
    void *overlay_proc(unsigned message, size_t w_param, size_t l_param);
#endif

    NVGcontext *context;

    bool init_gl();
    bool init_os_gl();
    bool init_window();

    float window_width  = 800;
    float window_height = 600;

    void end_os_frame();

public:
    bool init(const char *target_name);
    void shutdown();
    bool set_target(const char *target_name);

    void begin_frame();
    void end_frame();

    void resize();
};

struct NVColor;

// State based renderer, similar to ISurface
class OxideSurface {
    Oxide *window;

public:
    OxideSurface(Oxide *window) : window(window) {}

    void color(int r, int g, int b, int a);

    void filled_rect(int x1, int y1, int x2, int y2);
    void outlined_rect(int x1, int y1, int x2, int y2);

    void line(int x1, int y1, int x2, int y2);

    int  create_font(const char *font_name);
    void set_font(int handle);

    void draw_text(int x, int y, int height, const char *text);
};

using OxideColor = NVGcolor;
using OxidePaint = NVGpaint;
using OxideAlign = NVGalign;

// Wraps nvg c api
class OxideNVG {
    Oxide *window;

public:
    OxideNVG(Oxide *window) : window(window) {}
    static OxideColor rgba(int r, int g, int b, int a);
    static OxideColor hsla(float h, float s, float l, int a);

    void save_context();
    void restore_context();
    void reset_context();

    // Render styles
    void set_shape_aa(bool enabled);

    void stroke_color(OxideColor c);
    void stroke_paint(OxidePaint p);

    void fill_color(OxideColor c);
    void fill_paint(OxidePaint p);

    void miter_limit(float limit);
    void stroke_width(float width);

    void line_cap(int style);
    void line_join(int style);

    void global_alpha(float alpha);

    // Transformations
    void transform(float a, float b, float c, float d, float e, float f);
    void translate(float x, float y);
    void rotate(float angle);
    void skew_x(float angle);
    void skew_y(float angle);
    void scale(float x, float y);

    void current_transform(float *xform);

    // image
    int  create_image(const char *file_name, int flags);
    int  create_image_mem(int flags, unsigned char *data, int data_count);
    int  create_image_rgba(int w, int h, int flags, unsigned char *data);
    void update_image(int image, unsigned char *data);
    void image_size(int image, int *w, int *h);
    void delete_image(int image);

    // paints
    OxidePaint linear_gradient(float start_x, float start_y,
                               float eend_x, float end_y, OxideColor start_color,
                               OxideColor end_color);

    OxidePaint box_gradient(float x, float y, float w, float h,
                            float r, float f, OxideColor start_color, OxideColor end_color);

    OxidePaint radial_gradient(float x, float y, float inner_r, float outer_r,
                               OxideColor start_color, OxideColor end_color);

    OxidePaint image_pattern(float x, float y, float end_x, float end_y,
                             float angle, int image, float alpha);

    // Scissoring
    void scissor(float x, float y, float w, float h);
    void intersect_scissor(float x, float y, float w, float h);
    void reset_scissor();

    // Paths
    void begin_path();
    void move_to(float x, float y);
    void line_to(float x, float y);
    void bezier_to(float c1x, float c1y, float c2x, float c2y, float x, float y);
    void quad_to(float x, float y, float end_x, float end_y);
    void arc_to(float x1, float y1, float x2, float y2, float r);
    void close_path();
    void path_winding(int dir);

    void arc(float x, float y, float r, float angle_start, float angle_end, int dir);
    void rect(float x, float y, float w, float h);

    void rounded_rect(float x, float y, float w, float h, float r);
    void rounded_rect_varying(float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft);
    void ellipse(float x, float y, float rx, float ry);
    void circle(float x, float y, float r);

    void fill();
    void stroke();

    int create_font(const char *name, const char *file_name);
    int create_font_mem(const char *name, unsigned char *data, int data_count, int free_data);
    int find_font(const char *name);
    int add_fallback_fontId(int base, int fallback);
    int add_fallback_font(const char *base, const char *fallback);

    void font_size(float size);
    void font_blur(float blur);
    void text_letter_spacing(float spacing);
    void text_line_height(float line_height);
    void text_align(int align);

    void font_face_id(int id);
    void font_face(const char *name);

    float text(float x, float y, const char *string, const char *end = nullptr);

    void  text_box(float x, float y, float break_row_width, const char *string, const char *end = nullptr);
    float text_bounds(float x, float y, const char *string, const char *end, float *bounds);
    void  text_box_bounds(float x, float y, float row_width, const char *string, const char *end, float *bounds);
    int   text_glyph_positions(float x, float y, const char *string, const char *end, NVGglyphPosition *positions, int max_positions);
    void  text_metrics(float *ascender, float *descender, float *line_h);
    int   text_break_lines(const char *string, const char *end, float row_width, NVGtextRow *rows, int max_rows);

    NVGparams *internal_params();
};
