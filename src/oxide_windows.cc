#include "precompiled.hh"

#include "oxide.hh"

#ifdef _MSC_VER

#define GLCOMPAT

#define GLEW_STATIC
#include "GL/glew.h"

#include "GL/wglew.h"

#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>

#include <gl/gl.h>

const char oxide_window_name[] = "OXIDE_WINDOW";

bool Oxide::init_os_gl() {
    GLuint PixelFormat;

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32;
    pfd.cDepthBits   = 24;
    pfd.cAlphaBits   = 8;
    pfd.cStencilBits = 8;
    pfd.iLayerType   = PFD_MAIN_PLANE;

    dc          = GetDC((HWND)window_handle);
    PixelFormat = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, PixelFormat, &pfd);
    rc = wglCreateContext(dc);
    wglMakeCurrent(dc, rc);

    // calling glewinit NOW because the inside glew, there is mistake to fix...
    // This is the joy of using Core. The query glGetString(GL_EXTENSIONS) is deprecated from the Core profile.
    // You need to use glGetStringi(GL_EXTENSIONS, <index>) instead. Sounds like a "bug" in GLEW.

    if (!wglCreateContextAttribsARB) wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (wglCreateContextAttribsARB) {
        HGLRC atrib_rc = nullptr;
        int   attribList[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                WGL_CONTEXT_MINOR_VERSION_ARB, 2,
#ifdef GLCOMPAT
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
#else
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#endif
                WGL_CONTEXT_FLAGS_ARB,
        //WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB|
#ifndef GLCOMPAT
                WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
#endif
#ifdef _DEBUG
                    WGL_CONTEXT_DEBUG_BIT_ARB
#else
                0
#endif
                ,
                0, 0};
        if (!(atrib_rc = wglCreateContextAttribsARB((HDC)dc, 0, attribList))) {
            return false;
        }
        if (!wglMakeCurrent(dc, atrib_rc)) {
            return false;
        } else {
            wglDeleteContext(rc);
            rc = atrib_rc;
#ifdef _DEBUG
            if (!glDebugMessageCallbackARB) {
                glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
                glDebugMessageControlARB  = (PFNGLDEBUGMESSAGECONTROLARBPROC)wglGetProcAddress("glDebugMessageControlARB");
            }
            if (glDebugMessageCallbackARB) {
                //glDebugMessageCallbackARB(glErrorCallback, NULL);
                glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH_ARB, 0, NULL, GL_TRUE);
            }
#endif
        }
    }

    return true;
}

void *Oxide::overlay_proc(unsigned message, size_t w_param, size_t l_param) {
    switch (message) {
    case WM_CREATE: {
        break;
    }
    case WM_CLOSE: {
        DestroyWindow(window_handle);
        break;
    }
    //release resources
    case WM_DESTROY: {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(rc);
        DeleteDC(dc);
        ReleaseDC(window_handle, dc);
        PostQuitMessage(0);
        UnregisterClass(oxide_window_name, GetModuleHandle(nullptr));
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(window_handle, &ps);
        EndPaint(window_handle, &ps);
        return nullptr;
    }
    }

    return (void *)DefWindowProc(window_handle, message, w_param, l_param);
}

void *__stdcall Oxide::window_proc(HWND window_handle, unsigned message, size_t w_param, size_t l_param) {
    auto instance = reinterpret_cast<Oxide *>(GetWindowLongPtr(window_handle, GWLP_USERDATA));

    if (instance == nullptr) return (void *)DefWindowProc(window_handle, message, w_param, l_param);

    return instance->overlay_proc(message, w_param, l_param);
}

bool Oxide::init_window() {
    auto local_instance = GetModuleHandle(nullptr);

    WNDCLASSEX winClass;
    MSG        uMsg;

    memset(&uMsg, 0, sizeof(uMsg));

    winClass.lpszClassName = oxide_window_name;
    winClass.cbSize        = sizeof(WNDCLASSEX);
    winClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    winClass.lpfnWndProc   = (WNDPROC)&Oxide::window_proc;
    winClass.hInstance     = local_instance,
    winClass.hIcon         = 0x0;
    winClass.hIconSm       = 0x0;
    winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
    winClass.lpszMenuName  = NULL;
    winClass.cbClsExtra    = 0;
    winClass.cbWndExtra    = 0;

    if (!RegisterClassEx(&winClass))
        return false;

    window_handle = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST,
                                   oxide_window_name, "OxIDE",
                                   WS_VISIBLE | WS_POPUP | WS_POPUP,
                                   0,
                                   0,
                                   window_width,
                                   window_height,
                                   NULL,
                                   NULL,
                                   local_instance,
                                   NULL);
    if (window_handle == nullptr)
        return false;

    SetLayeredWindowAttributes(window_handle, 0, 255, LWA_ALPHA);

    DWM_BLURBEHIND bb   = {0};
    HRGN           hRgn = CreateRectRgn(0, 0, -1, -1);
    bb.dwFlags          = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur         = hRgn;
    bb.fEnable          = TRUE;
    DwmEnableBlurBehindWindow(window_handle, &bb);

    //ShowWindow(window_handle, 10);
    //UpdateWindow(window_handle);

    return true;
}

bool Oxide::set_target(const char *target_name) {
    // attempt to find the window now
    target_handle = FindWindowA(target_name, nullptr);
    if (target_handle == nullptr) return false;

    return true;
}

void Oxide::resize() {
    if (target_handle == nullptr || window_handle == nullptr) return; // we cant size the window yet

    RECT window_bounds;
    RECT client_bounds;

    // get the inner and outer bounds of the target
    GetWindowRect(target_handle, &window_bounds);
    GetClientRect(target_handle, &client_bounds);

    auto rect_size_zero = [](const RECT &a) {
        return (a.right - a.left) == 0 || (a.bottom - a.top) == 0;
    };

    if (rect_size_zero(window_bounds) || rect_size_zero(client_bounds)) return;

    //width and height of client rect
    auto width  = client_bounds.right - client_bounds.left;
    auto height = client_bounds.bottom - client_bounds.top;

    auto posx = window_bounds.right - width;
    auto posy = window_bounds.bottom - height;

    SetWindowPos(window_handle, 0, posx, posy, width, height, 0);

    glViewport(0, 0, width, height);
    glOrtho(client_bounds.left, client_bounds.right, client_bounds.bottom, client_bounds.top, 0, 1);

    this->window_width  = width;
    this->window_height = height;
}

void Oxide::begin_os_frame() {
    wglMakeCurrent(dc, rc);
}

void Oxide::end_os_frame() {

    auto active_window = GetForegroundWindow();

    // dont paint if the target is minimised
    if (!IsIconic(target_handle) && active_window == target_handle) {
        ShowWindow(window_handle, SW_SHOWNORMAL);
        SwapBuffers(dc);

        resize();
    } else {
        ShowWindow(window_handle, SW_HIDE);
    }

    // process any new messages
    MSG msg{0};

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return;
}
#endif