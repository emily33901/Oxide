#include "precompiled.hh"

#include "oxide.hh"

#include "GL/glew.h"

#define NANOVG_GLEW

#include "include/nanovg.h"

#define NANOVG_GL3_IMPLEMENTATION
#include "include/nanovg_gl.h"
#include "include/nanovg_gl_utils.h"

#include "oxide_imgui_draw.hh"

bool Oxide::init_gl() {
    init_os_gl();

    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor(0, 0, 0, 0);

    context = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);

    {
        // Imgui setup
        ImGui::CreateContext();
        auto &io = ImGui::GetIO();

        io.DisplaySize = ImVec2(window_width, window_height);

        ImGui_ImplOpenGL3_Init();

        ImGui::StyleColorsDark();
    }
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
    begin_os_frame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(context, window_width, window_height, 1);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Oxide::end_frame() {
    nvgEndFrame(context);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

OxideColor OxideNVG::rgba(int r, int g, int b, int a) {
    return nvgRGBA(r, g, b, a);
}

OxideColor OxideNVG::hsla(float h, float s, float l, int a) {
    return nvgHSLA(h, s, l, a);
}

void OxideNVG::save_context() {
    return nvgSave(window->context);
}
void OxideNVG::restore_context() {
    return nvgRestore(window->context);
}
void OxideNVG::reset_context() {
    return nvgReset(window->context);
}

void OxideNVG::set_shape_aa(bool enabled) {
    return nvgShapeAntiAlias(window->context, enabled);
}
void OxideNVG::stroke_color(OxideColor c) {
    return nvgStrokeColor(window->context, c);
}
void OxideNVG::stroke_paint(OxidePaint p) {
    return nvgStrokePaint(window->context, p);
}
void OxideNVG::fill_color(OxideColor c) {
    return nvgFillColor(window->context, c);
}
void OxideNVG::fill_paint(OxidePaint p) {
    return nvgFillPaint(window->context, p);
}
void OxideNVG::miter_limit(float limit) {
    return nvgMiterLimit(window->context, limit);
}
void OxideNVG::stroke_width(float width) {
    return nvgStrokeWidth(window->context, width);
}
void OxideNVG::line_cap(int style) {
    return nvgLineCap(window->context, style);
}
void OxideNVG::line_join(int style) {
    return nvgLineJoin(window->context, style);
}
void OxideNVG::global_alpha(float alpha) {
    return nvgGlobalAlpha(window->context, alpha);
}
void OxideNVG::transform(float a, float b, float c, float d, float e, float f) {
    return nvgTransform(window->context, a, b, c, d, e, f);
}
void OxideNVG::translate(float x, float y) {
    return nvgTranslate(window->context, x, y);
}
void OxideNVG::rotate(float angle) {
    return nvgRotate(window->context, angle);
}
void OxideNVG::skew_x(float angle) {
    return nvgSkewX(window->context, angle);
}
void OxideNVG::skew_y(float angle) {
    return nvgSkewY(window->context, angle);
}
void OxideNVG::scale(float x, float y) {
    return nvgScale(window->context, x, y);
}
void OxideNVG::current_transform(float *xform) {
    return nvgCurrentTransform(window->context, xform);
}

int OxideNVG::create_image(const char *file_name, int flags) {
    return nvgCreateImage(window->context, file_name, flags);
}
int OxideNVG::create_image_mem(int flags, unsigned char *data, int data_count) {
    return nvgCreateImageMem(window->context, flags, data, data_count);
}
int OxideNVG::create_image_rgba(int w, int h, int flags, unsigned char *data) {
    return nvgCreateImageRGBA(window->context, w, h, flags, data);
}
void OxideNVG::update_image(int image, unsigned char *data) {
    return nvgUpdateImage(window->context, image, data);
}
void OxideNVG::image_size(int image, int *w, int *h) {
    return nvgImageSize(window->context, image, w, h);
}
void OxideNVG::delete_image(int image) {
    return nvgDeleteImage(window->context, image);
}

OxidePaint OxideNVG::linear_gradient(float start_x, float start_y,
                                     float end_x, float end_y, OxideColor start_color,
                                     OxideColor end_color) {
    return nvgLinearGradient(window->context, start_x, start_y, end_x, end_y, start_color, end_color);
}

OxidePaint OxideNVG::box_gradient(float x, float y, float w, float h,
                                  float r, float f, OxideColor start_color, OxideColor end_color) {
    return nvgBoxGradient(window->context, x, y, w, h, r, f, start_color, end_color);
}

OxidePaint OxideNVG::radial_gradient(float x, float y, float inner_r, float outer_r,
                                     OxideColor start_color, OxideColor end_color) {
    return nvgRadialGradient(window->context, x, y, inner_r, outer_r, start_color, end_color);
}

OxidePaint OxideNVG::image_pattern(float x, float y, float end_x, float end_y,
                                   float angle, int image, float alpha) {
    return nvgImagePattern(window->context, x, y, end_x, end_y, angle, image, alpha);
}

void OxideNVG::scissor(float x, float y, float w, float h) {
    return nvgScissor(window->context, x, y, w, h);
}
void OxideNVG::intersect_scissor(float x, float y, float w, float h) {
    return nvgIntersectScissor(window->context, x, y, w, h);
}
void OxideNVG::reset_scissor() {
    return nvgResetScissor(window->context);
}

void OxideNVG::begin_path() {
    return nvgBeginPath(window->context);
}
void OxideNVG::move_to(float x, float y) {
    return nvgMoveTo(window->context, x, y);
}
void OxideNVG::line_to(float x, float y) {
    return nvgLineTo(window->context, x, y);
}
void OxideNVG::bezier_to(float c1x, float c1y, float c2x, float c2y, float x, float y) {
    return nvgBezierTo(window->context, c1x, c1y, c2x, c2y, x, y);
}
void OxideNVG::quad_to(float x, float y, float end_x, float end_y) {
    return nvgQuadTo(window->context, x, y, end_x, end_y);
}
void OxideNVG::arc_to(float x1, float y1, float x2, float y2, float r) {
    return nvgArcTo(window->context, x1, y1, x2, y2, r);
}
void OxideNVG::close_path() {
    return nvgClosePath(window->context);
}
void OxideNVG::path_winding(int dir) {
    return nvgPathWinding(window->context, dir);
}

void OxideNVG::arc(float x, float y, float r, float angle_start, float angle_end, int dir) {
    return nvgArc(window->context, x, y, r, angle_start, angle_end, dir);
}
void OxideNVG::rect(float x, float y, float w, float h) {
    return nvgRect(window->context, x, y, w, h);
}

void OxideNVG::rounded_rect(float x, float y, float w, float h, float r) {
    return nvgRoundedRect(window->context, x, y, w, h, r);
}
void OxideNVG::rounded_rect_varying(float x, float y, float w, float h, float top_left, float top_right, float bot_right, float bot_left) {
    return nvgRoundedRectVarying(window->context, x, y, w, h, top_left, top_right, bot_right, bot_left);
}
void OxideNVG::ellipse(float x, float y, float rx, float ry) {
    return nvgEllipse(window->context, x, y, rx, ry);
}
void OxideNVG::circle(float x, float y, float r) {
    return nvgCircle(window->context, x, y, r);
}

void OxideNVG::fill() {
    return nvgFill(window->context);
}
void OxideNVG::stroke() {
    return nvgStroke(window->context);
}

int OxideNVG::create_font(const char *name, const char *file_name) {
    return nvgCreateFont(window->context, name, file_name);
}
int OxideNVG::create_font_mem(const char *name, unsigned char *data, int data_count, int free_data) {
    return nvgCreateFontMem(window->context, name, data, data_count, free_data);
}
int OxideNVG::find_font(const char *name) {
    return nvgFindFont(window->context, name);
}
int OxideNVG::add_fallback_fontId(int base, int fallback) {
    return nvgAddFallbackFontId(window->context, base, fallback);
}
int OxideNVG::add_fallback_font(const char *base, const char *fallback) {
    return nvgAddFallbackFont(window->context, base, fallback);
}

void OxideNVG::font_size(float size) {
    return nvgFontSize(window->context, size);
}
void OxideNVG::font_blur(float blur) {
    return nvgFontBlur(window->context, blur);
}
void OxideNVG::text_letter_spacing(float spacing) {
    return nvgTextLetterSpacing(window->context, spacing);
}
void OxideNVG::text_line_height(float line_height) {
    return nvgTextLineHeight(window->context, line_height);
}
void OxideNVG::text_align(int align) {
    return nvgTextAlign(window->context, align);
}

void OxideNVG::font_face_id(int id) {
    return nvgFontFaceId(window->context, id);
}
void OxideNVG::font_face(const char *name) {
    return nvgFontFace(window->context, name);
}

float OxideNVG::text(float x, float y, const char *string, const char *end) {
    return nvgText(window->context, x, y, string, end);
}

void OxideNVG::text_box(float x, float y, float break_row_width, const char *string, const char *end) {
    return nvgTextBox(window->context, x, y, break_row_width, string, end);
}

float OxideNVG::text_bounds(float x, float y, const char *string, const char *end, float *bounds) {
    return nvgTextBounds(window->context, x, y, string, end, bounds);
}
void OxideNVG::text_box_bounds(float x, float y, float row_width, const char *string, const char *end, float *bounds) {
    return nvgTextBoxBounds(window->context, x, y, row_width, string, end, bounds);
}
int OxideNVG::text_glyph_positions(float x, float y, const char *string, const char *end, NVGglyphPosition *positions, int max_positions) {
    return nvgTextGlyphPositions(window->context, x, y, string, end, positions, max_positions);
}
void OxideNVG::text_metrics(float *ascender, float *descender, float *line_h) {
    return nvgTextMetrics(window->context, ascender, descender, line_h);
}
int OxideNVG::text_break_lines(const char *string, const char *end, float row_width, NVGtextRow *rows, int max_rows) {
    return nvgTextBreakLines(window->context, string, end, row_width, rows, max_rows);
}

NVGparams *OxideNVG::internal_params() {
    return nvgInternalParams(window->context);
}

#if 0
#ifdef _MSC_VER
#include <Windows.h>
const char target[]    = "Notepad";
const char font_name[] = "C:\\Windows\\Fonts\\Tahoma.ttf";
#else
#include <unistd.h>
void Sleep(int amount) {
    usleep(amount * 1000);
}
const char target[] = "pygame window";
//const char target[] = "*Untitled Document 1 - gedit";

const char font_name[] = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif

#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    Oxide o;
    bool  init_success = o.init(target);

    while (!init_success) {
        printf("Trying to find window...\n");
        Sleep(1000);
        init_success = o.set_target(target);
    }

    OxideNVG     n(&o);
    OxideSurface s(&o);

    //int id = nvgCreateFont(o.context, "regular", "C:\\Windows\\Fonts\\Tahoma.ttf");
    int id = n.create_font("regular", font_name);

    duration<double> frame_length;
    char             fps[128];

    while (true) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        o.begin_frame();

#if 0
        nvgBeginPath(o.context);
        nvgCircle(o.context, 100, 100, 15);
        nvgStrokeColor(o.context, {0, 255, 0, 255});
        nvgStrokeWidth(o.context, 15);
        nvgStroke(o.context);
#endif
        n.begin_path();
        n.circle(100, 100, 15);
        n.stroke_color({0, 255, 0, 255});
        n.stroke_width(15);
        n.stroke();
#if 0
        nvgBeginPath(o.context);
        nvgCircle(o.context, 200, 300, 50);
        nvgStrokeColor(o.context, {0, 255, 0, 255});
        nvgStrokeWidth(o.context, 15);
        nvgStroke(o.context);
#endif
        n.begin_path();
        n.circle(200, 300, 50);
        n.stroke_color({0, 255, 0, 255});
        n.stroke_width(15);
        n.stroke();

#if 0
        nvgFontSize(o.context, 72);
        nvgTextAlign(o.context, NVGalign::NVG_ALIGN_LEFT | NVGalign::NVG_ALIGN_TOP);
        nvgFillColor(o.context, {0, 0, 255, 255});
        nvgText(o.context, 10, 10, "wtf this is a good meme", nullptr);
#endif
        n.font_size(72);
        n.text_align(OxideAlign::NVG_ALIGN_LEFT | OxideAlign::NVG_ALIGN_TOP);
        n.fill_color({0, 0, 255, 255});
        sprintf(fps, "%f", 1 / frame_length.count());
        n.text(10, 10, fps);

        printf("%f\n", 1 / frame_length.count());

        s.color(0, 255, 255, 255);
        s.filled_rect(300, 300, 400, 400);

        s.color(255, 0, 0, 255);
        s.line(0, 0, 300, 300);

        s.color(255, 255, 0, 255);
        s.set_font(id);
        s.draw_text(100, 100, 48, "raspbian cant code");

        s.outlined_rect(50, 50, 200, 300);

        o.end_frame();

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        frame_length                         = duration_cast<duration<double>>(t2 - t1);
    }
}
#endif