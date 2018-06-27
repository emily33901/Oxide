#include "precompiled.hh"

#include "oxide.hh"

#define GLEW_STATIC
#include "GL/glew.h"

#define NANOVG_GLEW

#include "include/nanovg.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "include/nanovg_gl.h"
#include "include/nanovg_gl_utils.h"

bool Oxide::init_gl() {
    init_os_gl();

    glewInit();

    glClearColor(0, 0, 0, 0);

    context = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

    return true;
}

bool Oxide::init(const char *target_name) {
    bool window_success = init_window();
    if (!window_success) {
        printf("Failed to create window....\n");
        return false;
    }
    bool gl_success = init_gl();
    if (!gl_success) {
        printf("Failed to create gl\n");
        return false;
    }

    if (!set_target(target_name)) {
        printf("Failed to find window\n");
        return false;
    }

    resize();

    return true;
}

void Oxide::begin_frame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(context, window_width, window_height, 1);
}

void Oxide::end_frame() {
    nvgEndFrame(context);
    end_os_frame();
}

void OxideSurface::color(int r, int g, int b, int a) {
    auto new_color = nvgRGBA(r, g, b, a);
    nvgFillColor(window->context, new_color);
    nvgStrokeColor(window->context, new_color);
}

void OxideSurface::filled_rect(int x1, int y1, int x2, int y2) {
    nvgBeginPath(window->context);
    nvgRect(window->context, x1, y1, x2 - x1, y2 - y1);
    nvgFill(window->context);
    nvgStrokeWidth(window->context, 1);
    nvgStroke(window->context);
}

void OxideSurface::outlined_rect(int x1, int y1, int x2, int y2) {
    nvgBeginPath(window->context);
    nvgRect(window->context, x1, y1, x2 - x1, y2 - y1);
    nvgStrokeWidth(window->context, 1);
    nvgStroke(window->context);
}

void OxideSurface::line(int x1, int y1, int x2, int y2) {
    nvgBeginPath(window->context);
    nvgMoveTo(window->context, x1, y1);
    nvgLineTo(window->context, x2, y2);
    nvgStrokeWidth(window->context, 1);
    nvgStroke(window->context);
}

int OxideSurface::create_font(const char *name) {
    return nvgCreateFont(window->context, name, name);
}

void OxideSurface::set_font(int handle) {
    nvgFontFaceId(window->context, handle);
}

void OxideSurface::draw_text(int x, int y, int height, const char *text) {
    nvgFontSize(window->context, height);
    nvgTextAlign(window->context, NVGalign::NVG_ALIGN_LEFT | NVGalign::NVG_ALIGN_TOP);
    nvgText(window->context, x, y, text, nullptr);
}

#include <Windows.h>
int main() {
    Oxide o;
    bool  init_success = o.init("Notepad");

    while (!init_success) {
        printf("Trying to find window...\n");
        Sleep(1000);
        init_success = o.set_target("Notepad");
    }

    int id = nvgCreateFont(o.context, "regular", "C:\\Windows\\Fonts\\Tahoma.ttf");

    OxideSurface s(&o);

    while (true) {
        o.begin_frame();

        nvgBeginPath(o.context);
        nvgCircle(o.context, 100, 100, 15);
        nvgStrokeColor(o.context, {0, 255, 0, 255});
        nvgStrokeWidth(o.context, 15);
        nvgStroke(o.context);

        nvgBeginPath(o.context);
        nvgCircle(o.context, 200, 300, 50);
        nvgStrokeColor(o.context, {0, 255, 0, 255});
        nvgStrokeWidth(o.context, 15);
        nvgStroke(o.context);

        nvgFontSize(o.context, 72);
        nvgTextAlign(o.context, NVGalign::NVG_ALIGN_LEFT | NVGalign::NVG_ALIGN_TOP);
        nvgFillColor(o.context, {0, 0, 255, 255});
        nvgText(o.context, 10, 10, "wtf this is a good meme", nullptr);

        s.color(0, 255, 255, 255);
        s.filled_rect(300, 300, 400, 400);

        s.color(255, 0, 0, 255);
        s.line(0, 0, 300, 300);

        s.color(255, 255, 0, 255);
        s.set_font(id);
        s.draw_text(100, 100, 40, "raspbian cant code");

        s.outlined_rect(50, 50, 200, 300);

        o.end_frame();
    }
}
