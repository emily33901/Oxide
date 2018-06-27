#pragma once

class NVGcontext;

#ifdef _MSC_VER
struct HDC__;
using HDC = HDC__ *;
struct HGLRC__;
using HGLRC = HGLRC__ *;

struct HWND__;
using HWND = HWND__ *;
#else
#define __stdcall
#endif

class Oxide {
    friend class OxideDraw;

#ifdef _MSC_VER
    HDC   dc;
    HGLRC rc;
#endif
    HWND window_handle;

    HWND target_handle;

    static void *__stdcall window_proc(HWND hwnd, unsigned message, size_t w_param, size_t l_param);
    void *overlay_proc(unsigned message, size_t w_param, size_t l_param);

    bool init_gl();
    bool init_os_gl();
    bool init_window();

    float window_width  = 800;
    float window_height = 600;

    void end_os_frame();

public:
    NVGcontext *context;

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
