/*
 * CONSOLE
 * $Id$
 *
 * Simple console interface
 *
 * This module provides a console driver for the console
 * subsystem on Windows 9X/NT/2K.
 *
 * History:
 * 1999-01-05 fl  Created
 * 1999-01-06 fl  Added Event type and readline hooks
 * 2000-11-15 fl  Added getchar method, default style (attr)
 * 2000-11-22 fl  Added home method
 * 2001-01-03 fl  Minor fixes
 * 2001-05-08 fl  Use DL_EXPORT instead of declspec hack
 *
 * Written by Fredrik Lundh, January 1999.
 *
 * Copyright (c) 1999-2001 by Secret Labs AB.
 * Copyright (c) 1999-2001 by Fredrik Lundh.
 *
 * fredrik@pythonware.com
 * http://www.pythonware.com
 *
 * --------------------------------------------------------------------
 * The Windows Console Driver is
 * 
 * Copyright (c) 1999-2001 by Secret Labs AB
 * Copyright (c) 1999-2001 by Fredrik Lundh
 * 
 * By obtaining, using, and/or copying this software and/or its
 * associated documentation, you agree that you have read, understood,
 * and will comply with the following terms and conditions:
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * associated documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appears in all
 * copies, and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Secret Labs
 * AB or the author not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 * SECRET LABS AB AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS.  IN NO EVENT SHALL SECRET LABS AB OR THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * -------------------------------------------------------------------- */

#include "Python.h"
#include "structmember.h"

#undef UNICODE /* not yet */

#define WITH_READLINE /* readline support */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* default attribute */
#define WHITE 7
#define BLACK 0

/* console type definition */
typedef struct {
    PyObject_HEAD
    HANDLE in;
    HANDLE out;
    DWORD inmode;
    WORD attr;
    /* file emulation stuff */
    int softspace;
    int serial;
} ConsoleObject;

static PyTypeObject Console_Type;

/* event type definition (from uiToolkit) */
typedef struct {
    PyObject_HEAD
    char* type;
    int serial;
    int time;
    int x, y;
    int state;
    int keycode;
    char* keysym;
    char* char_;
    int width;
    int height;
    PyObject* widget;
    char character[2];
} EventObject;

static PyTypeObject Event_Type;

/* ==================================================================== */
/* console instances */

static PyObject*
console_new(PyObject* self_, PyObject* args)
{
    ConsoleObject* self;

    int newbuffer = 1;
    if (!PyArg_ParseTuple(args, "|i", &newbuffer))
        return NULL;

    self = PyObject_NEW(ConsoleObject, &Console_Type);
    if (self == NULL)
        return NULL;
    
    /* FIXME: add error checking! */

    AllocConsole();

    /* setup outbut buffers */
    if (newbuffer) {
        self->out = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE, 0,
            NULL, CONSOLE_TEXTMODE_BUFFER, NULL
            );
        SetConsoleActiveScreenBuffer(self->out);
    } else
        self->out = GetStdHandle(STD_OUTPUT_HANDLE);

    /* setup input buffers */
    self->in = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(self->in, &self->inmode);
    SetConsoleMode(self->in, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    /* default style */
    self->attr = WHITE;

    self->softspace = 0;

    self->serial = 0;

    return (PyObject*) self;
}

static void
console_dealloc(ConsoleObject* self)
{
    SetConsoleMode(self->in, self->inmode);
    FreeConsole();

    PyMem_DEL(self);
}

/* ==================================================================== */
/* event instances */

static char*
keysym(int keycode)
{
    /* map virtual keys to X windows key symbols (from uiToolkit) */

    switch (keycode) {

    case VK_CANCEL: return "Cancel";

    case VK_BACK: return "BackSpace";
    case VK_TAB: return "Tab";

    case VK_CLEAR: return "Clear";
    case VK_RETURN: return "Return";

    case VK_SHIFT: return "Shift_L";
    case VK_CONTROL: return "Control_L";
    case VK_MENU: return "Alt_L";
    case VK_PAUSE: return "Pause";
    case VK_CAPITAL: return "Caps_Lock";

    case VK_ESCAPE: return "Escape";

    case VK_SPACE: return "space";
    case VK_PRIOR: return "Prior";
    case VK_NEXT: return "Next";
    case VK_END: return "End";
    case VK_HOME: return "Home";
    case VK_LEFT: return "Left";
    case VK_UP: return "Up";
    case VK_RIGHT: return "Right";
    case VK_DOWN: return "Down";
    case VK_SELECT: return "Select";
    case VK_PRINT: return "Print";
    case VK_EXECUTE: return "Execute";
    case VK_SNAPSHOT: return "Snapshot";
    case VK_INSERT: return "Insert";
    case VK_DELETE: return "Delete";
    case VK_HELP: return "Help";

   /* FIXME: should the numerical keyboard keys be included in this
    * list?  if not, there's no way to distinguish them from the
    * corresponding ASCII characters */

    case VK_F1: return "F1";
    case VK_F2: return "F2";
    case VK_F3: return "F3";
    case VK_F4: return "F4";
    case VK_F5: return "F5";
    case VK_F6: return "F6";
    case VK_F7: return "F7";
    case VK_F8: return "F8";
    case VK_F9: return "F9";
    case VK_F10: return "F10";
    case VK_F11: return "F11";
    case VK_F12: return "F12";
    case VK_F13: return "F13";
    case VK_F14: return "F14";
    case VK_F15: return "F15";
    case VK_F16: return "F16";
    case VK_F17: return "F17";
    case VK_F18: return "F18";
    case VK_F19: return "F19";
    case VK_F20: return "F20";
    case VK_F21: return "F21";
    case VK_F22: return "F22";
    case VK_F23: return "F23";
    case VK_F24: return "F24";

    case VK_NUMLOCK: return "Num_Lock,";
    case VK_SCROLL: return "Scroll_Lock";

    /* FIXME: remove the following? */

#if defined(VK_APPS)
    case VK_APPS: return "VK_APPS";
#endif
#if defined(VK_PROCESSKEY)
    case VK_PROCESSKEY: return "VK_PROCESSKEY";
#endif
#if defined(VK_ATTN)
    case VK_ATTN: return "VK_ATTN";
#endif
#if defined(VK_CRSEL)
    case VK_CRSEL: return "VK_CRSEL";
#endif
#if defined(VK_EXSEL)
    case VK_EXSEL: return "VK_EXSEL";
#endif
#if defined(VK_EREOF)
    case VK_EREOF: return "VK_EREOF";
#endif
#if defined(VK_PLAY)
    case VK_PLAY: return "VK_PLAY";
#endif
#if defined(VK_ZOOM)
    case VK_ZOOM: return "VK_ZOOM";
#endif
#if defined(VK_NONAME)
    case VK_NONAME: return "VK_NONAME";
#endif
#if defined(VK_PA1)
    case VK_PA1: return "VK_PA1";
#endif
#if defined(VK_OEM_CLEAR)
    case VK_OEM_CLEAR: return "VK_OEM_CLEAR";
#endif
    }

    return "";
};

static PyObject*
event_new(ConsoleObject* console, INPUT_RECORD* input)
{
    EventObject* self;

    self = PyObject_NEW(EventObject, &Event_Type);
    if (self == NULL)
        return NULL;

    self->type = "??";

    self->serial = ++console->serial;

    self->width = 0;
    self->height = 0;

    self->x = self->y = 0;

    self->character[0] = 0;

    self->keycode = 0;
    self->keysym = "??";

    self->char_ = self->character;
    
    Py_INCREF(Py_None);
    self->widget = Py_None;

    switch (input->EventType) {
    case KEY_EVENT:
        if (input->Event.KeyEvent.bKeyDown)
            self->type = "KeyPress";
        else
            self->type = "KeyRelease";
        self->character[0] = input->Event.KeyEvent.uChar.AsciiChar;
        self->character[1] = 0;
        self->keysym = keysym(input->Event.KeyEvent.wVirtualKeyCode);
        self->keycode = input->Event.KeyEvent.wVirtualKeyCode;
        self->state = input->Event.KeyEvent.dwControlKeyState;
        /* FIXME: map state to Tkinter compatible state */
        break;
    case MOUSE_EVENT:
        if (input->Event.MouseEvent.dwEventFlags & MOUSE_MOVED)
            self->type = "Motion";
        else
            self->type = "Button";
        /* FIXME: map state change to Tkinter button event */
        self->x = input->Event.MouseEvent.dwMousePosition.X;
        self->y = input->Event.MouseEvent.dwMousePosition.Y;
        self->state = input->Event.MouseEvent.dwButtonState;
        /* FIXME: map state to Tkinter compatible state */
        break;
    case WINDOW_BUFFER_SIZE_EVENT:
        self->type = "Configure";
        self->width = input->Event.WindowBufferSizeEvent.dwSize.X;
        self->height = input->Event.WindowBufferSizeEvent.dwSize.Y;
        break;
    case FOCUS_EVENT:
        if (input->Event.FocusEvent.bSetFocus)
            self->type = "FocusIn";
        else
            self->type = "FocusOut";
        break;
    case MENU_EVENT:
        self->type = "Menu";
        self->state = input->Event.MenuEvent.dwCommandId;
        break;
    default:
        self->type = "??";
    }

    return (PyObject*) self;
}

static void
event_dealloc(EventObject* self)
{
    PyMem_DEL(self);
}

/* utility functions */

static __inline PyObject*
fixstatus(int flag)
{
    if (flag) {
        Py_INCREF(Py_True);
        return Py_True;
    } else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static __inline COORD
fixcoord(ConsoleObject* self, int x, int y)
{
    COORD coord;
    CONSOLE_SCREEN_BUFFER_INFO info;

    if (x < 0 || y < 0) {
        /* wrap coordinates */
        GetConsoleScreenBufferInfo(self->out, &info);
        if (x < 0)
            x = info.srWindow.Right - x;
        if (y < 0)
            y = info.srWindow.Bottom + y;
    }

    coord.X = x;
    coord.Y = y;

    return coord;
}

/* ==================================================================== */
/* console methods */

/* file interface (this also includes the softspace attribute) */ 

static PyObject*
console_pos(ConsoleObject* self, PyObject* args)
{
    /* move or query the window cursor */

    int x, y;

    if (!PyTuple_Size(args)) {
        /* get current position */
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(self->out, &info);
        return Py_BuildValue("ii", info.dwCursorPosition.X,
                             info.dwCursorPosition.Y);
    }

    if (!PyArg_ParseTuple(args, "ii", &x, &y))
        return NULL;

    return fixstatus(SetConsoleCursorPosition(
        self->out, fixcoord(self, x, y)
        ));
}

static PyObject*
console_home(ConsoleObject* self, PyObject* args)
{
    /* convenience: same as pos(0, 0) */

    if (!PyArg_ParseTuple(args,""))
        return NULL;

    return fixstatus(SetConsoleCursorPosition(
        self->out, fixcoord(self, 0, 0)
        ));
}

static PyObject*
console_write(ConsoleObject* self, PyObject* args)
{
    /* write text at current cursor position */

    DWORD n;

    char* text;
    int length;
    int attr = self->attr;
    if (!PyArg_ParseTuple(args, "s#|i", &text, &length, &attr))
        return NULL;

    SetConsoleTextAttribute(self->out, (WORD) attr);

    WriteConsole(self->out, text, length, &n, NULL);

    return fixstatus(0);
}

/* graphics api */

static PyObject*
console_page(ConsoleObject* self, PyObject* args)
{
    /* fill the entire screen */

    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD pos;
    DWORD n;
    int w;

    int attr = self->attr;
    char fill = ' ';
    if (!PyArg_ParseTuple(args, "|ic", &attr, &fill))
        return NULL;

    GetConsoleScreenBufferInfo(self->out, &info);

    pos.X = pos.Y = 0;

    if (info.dwCursorPosition.X != 0 || info.dwCursorPosition.Y != 0)
        /* setting the cursor is many times slower than clearing
           the entire console... */
        SetConsoleCursorPosition(self->out, pos);

    w = info.dwSize.X;

    for (; pos.Y < info.dwSize.Y; pos.Y++) {
        FillConsoleOutputAttribute(self->out, (WORD) attr, w, pos, &n);
        FillConsoleOutputCharacter(self->out, fill, w, pos, &n);
    }

    self->attr = attr;

    return fixstatus(0);
}

static PyObject*
console_text(ConsoleObject* self, PyObject* args)
{
    /* write text at given position */

    COORD pos;
    DWORD n;

    int x, y;
    char* text;
    int length;
    int attr = self->attr;
    if (!PyArg_ParseTuple(args, "iis#|i", &x, &y, &text, &length, &attr))
        return NULL;

    pos = fixcoord(self, x, y);

    WriteConsoleOutputCharacter(self->out, text, length, pos, &n);
    FillConsoleOutputAttribute(self->out, (WORD) attr, n, pos, &n);

    return fixstatus(0);
}

static PyObject*
console_rectangle(ConsoleObject* self, PyObject* args)
{
    /* fill rectangle */

    COORD pos;
    DWORD n;

    int x0, y0, x1, y1;
    int attr = self->attr;
    char fill = ' ';
    if (!PyArg_ParseTuple(args, "(iiii)|ic", &x0, &y0, &x1, &y1, &attr, &fill))
        return NULL;

    pos.X = x0; 
    pos.Y = y0;

    while (pos.Y < y1) {
        FillConsoleOutputAttribute(self->out, (WORD) attr, x1-x0, pos, &n);
        FillConsoleOutputCharacter(self->out, fill, x1-x0, pos, &n);
        pos.Y++;
    }

    return fixstatus(0);
}

static PyObject*
console_scroll(ConsoleObject* self, PyObject* args)
{
    /* scroll a region */

    SMALL_RECT source;
    CHAR_INFO style;
    COORD dest;

    int x0, y0, x1, y1;
    int dx, dy;
    int attr = self->attr;
    char fill = ' ';
    if (!PyArg_ParseTuple(args, "(iiii)ii|ic", &x0, &y0, &x1, &y1,
                          &dx, &dy, &attr, &fill))
        return NULL;

    source.Left = x0;
    source.Top = y0;
    source.Right = x1 - 1;
    source.Bottom = y1 - 1;

    dest.X = x0 + dx;
    dest.Y = y0 + dy;

    style.Attributes = attr;
    style.Char.AsciiChar = fill;

    return fixstatus(ScrollConsoleScreenBuffer(
        self->out,
        &source,&source,
        dest, &style
        ));
}

/* event management */

static PyObject*
console_get(ConsoleObject* self, PyObject* args)
{
    /* get next event from queue */

    INPUT_RECORD event;
    DWORD count = 0;
    int status;

    if (!PyArg_ParseTuple(args,""))
        return NULL;

    status = ReadConsoleInput(self->in, &event, 1, &count);
    if (status && count == 1)
        return event_new(self, &event);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
console_getchar(ConsoleObject* self, PyObject* args)
{
    /* get next character from queue */

    INPUT_RECORD event;
    DWORD count = 0;
    int status;

    if (!PyArg_ParseTuple(args,""))
        return NULL;

    for (;;) {
        status = ReadConsoleInput(self->in, &event, 1, &count);
        if (status && count == 1 && event.EventType == KEY_EVENT &&
            event.Event.KeyEvent.bKeyDown) {
            char* sym = keysym(event.Event.KeyEvent.wVirtualKeyCode);
            if (sym && sym[0])
                return Py_BuildValue("s", sym);
            else
                return Py_BuildValue(
                    "c", event.Event.KeyEvent.uChar.AsciiChar
                    );
        }
    }

    return NULL;
}

static PyObject*
console_peek(ConsoleObject* self, PyObject* args)
{
    /* check event queue */

    INPUT_RECORD event;
    DWORD count = 0;
    int status;

    if (!PyArg_ParseTuple(args,""))
        return NULL;

    status = PeekConsoleInput(self->in, &event, 1, &count);
    if (status && count == 1)
        return event_new(self, &event);

    Py_INCREF(Py_None);
    return Py_None;
}

/* configuration */

static PyObject*
console_cursor(ConsoleObject* self, PyObject* args)
{
    /* set cursor on or off */

    CONSOLE_CURSOR_INFO info;

    int visible;
    if (!PyArg_ParseTuple(args, "i", &visible))
        return NULL;

    if (GetConsoleCursorInfo(self->out, &info)) {
        info.bVisible = (visible) ? TRUE : FALSE;
        SetConsoleCursorInfo(self->out, &info);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
console_size(ConsoleObject* self, PyObject* args)
{
    /* set/get window size */

    CONSOLE_SCREEN_BUFFER_INFO info;

    int width = -1;
    int height = -1;
    if (!PyArg_ParseTuple(args, "|ii", &width, &height))
        return NULL;

    if (width >= 0 && height >= 0)
        /* FIXME */;
    else {
        if (GetConsoleScreenBufferInfo(self->out, &info))
            return Py_BuildValue("ii", info.dwSize.X, info.dwSize.Y);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
console_title(ConsoleObject* self, PyObject* args)
{
    /* set/get title */
    char* title = NULL;
    if (!PyArg_ParseTuple(args, "|s", &title))
        return NULL;

    if (title)
        SetConsoleTitle(title);
    else {
        char buffer[200];
        if (GetConsoleTitle(buffer, sizeof buffer))
            return Py_BuildValue("s", buffer);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef
console_methods[] = {

    /* graphics */
    {"text", (PyCFunction) console_text, 1},
    {"rectangle", (PyCFunction) console_rectangle, 1},

    {"scroll", (PyCFunction) console_scroll, 1},

    {"page", (PyCFunction) console_page, 1},

/*    {"save", (PyCFunction) console_save, 1}, */
/*    {"restore", (PyCFunction) console_restore, 1},*/

    /* stream output */
    {"write", (PyCFunction) console_write, 1},
    {"pos", (PyCFunction) console_pos, 1},
    {"home", (PyCFunction) console_home, 1},

    /* events */
    {"get", (PyCFunction) console_get, 1},
    {"getchar", (PyCFunction) console_getchar, 1},
    {"peek", (PyCFunction) console_peek, 1},

    /* misc. settings */
    {"cursor", (PyCFunction) console_cursor, 1},
    {"size", (PyCFunction) console_size, 1},
    {"title", (PyCFunction) console_title, 1},

    {NULL, NULL}
};

static PyObject*  
console_getattr(ConsoleObject* self, char* name)
{
    if (name[0] == 's' && strcmp(name, "softspace") == 0)
      return PyLong_FromLong(self->softspace);

    return PyObject_GenericGetAttr((PyObject*) self,(PyObject*)  name);
}

static int
console_setattr(ConsoleObject* self, char* name, PyObject* value)
{
    if (!value) {
        PyErr_SetString(
            PyExc_AttributeError,
            "can't delete Console attributes"
            );
        return -1;
    }

    if (!strcmp(name, "softspace")) {
    long v;
    v = PyLong_AsLong(value);
    if (v == -1 && PyErr_Occurred())
            return -1;
    self->softspace = v;
    return 0;
    }

    PyErr_SetString(PyExc_AttributeError, name);
    return -1;
}

static PyTypeObject Console_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "Console", /* tp_name */
    sizeof(ConsoleObject), /* tp_size */
    0,
    /* methods */
    (destructor)console_dealloc, /* tp_dealloc */
    NULL, /* tp_print */
    (getattrfunc)console_getattr, /* tp_getattr */
    (setattrfunc)console_setattr, /* tp_setattr */
    NULL, /* tp_compare */
    0, /* tp_repr */
    0,                    /* tp_as_number */
    0,                    /* tp_as_sequence */
    0,                    /* tp_as_mapping */
    0,                    /* tp_hash */
    0,        /* tp_call */
    0,                    /* tp_str */
    PyObject_GenericGetAttr,        /* tp_getattro */
    0,                    /* tp_setattro */
    0,                    /* tp_as_buffer */
    0, /* tp_flags */
    0,                    /* tp_doc */
    0,                /* tp_traverse */
    0,                    /* tp_clear */
    0,                    /* tp_richcompare */
    0,                    /* tp_weaklistoffset */
    0,                    /* tp_iter */
    0,                    /* tp_iternext */
    console_methods,                    /* tp_methods */
    0,                /* tp_members */
    0,                /* tp_getset */
    0,                    /* tp_base */
    0,                    /* tp_dict */
    0,        /* tp_descr_get */
    0,                    /* tp_descr_set */
};

/* ==================================================================== */
/* event attributes */

#define EVENT_OFF(x) offsetof(EventObject, x)

struct PyMemberDef
event_members[] = {
    {"type", T_STRING, EVENT_OFF(type), READONLY,"type"},
    {"keycode", T_INT, EVENT_OFF(keycode), READONLY,"keycode"},
    {"keysym", T_STRING, EVENT_OFF(keysym), READONLY,"keysym"},
    {"char", T_STRING, EVENT_OFF(char_), READONLY,"char"},
    {"state", T_INT, EVENT_OFF(state), READONLY,"state"},
    {"x", T_INT, EVENT_OFF(x), READONLY,"x"},
    {"y", T_INT, EVENT_OFF(y), READONLY,"y"},
    {"width", T_INT, EVENT_OFF(width), READONLY,"width"},
    {"height", T_INT, EVENT_OFF(height), READONLY,"height"},
    {"widget", T_OBJECT, EVENT_OFF(widget)},
    {"serial", T_INT, EVENT_OFF(serial), READONLY,"serial"},
    {"time", T_INT, EVENT_OFF(time), READONLY,"time"},
    {NULL}
};

static PyObject*  
event_getattr(EventObject* self, char* name)
{
    //return PyMember_GetOne((char*)self, event_members, name);
    return PyMember_GetOne((const char *) self, event_members);
}

static int
event_setattr(EventObject* self, char* name, PyObject* value)
{
    return PyMember_SetOne((char *) self, event_members, value);
}

static PyObject *
event_repr(EventObject* self)
{
    char buffer[300];

    sprintf(
        buffer, "<%s Event at %lx>",
        self->type,
        (long) self
        );

    return PyUnicode_FromString(buffer);
}

static PyTypeObject Event_Type = {
    PyVarObject_HEAD_INIT(NULL,0)
    "Event",
    sizeof(PyMethodDescrObject),
    0,
    (destructor)event_dealloc, /* tp_dealloc */
    NULL, /* tp_print */
    (getattrfunc)event_getattr, /* tp_getattr */
    (setattrfunc)event_setattr, /* tp_setattr */
    NULL, /* tp_compare */
    (reprfunc)event_repr, /* tp_repr */
    0,                    /* tp_as_number */
    0,                    /* tp_as_sequence */
    0,                    /* tp_as_mapping */
    0,                    /* tp_hash */
    0,        /* tp_call */
    0,                    /* tp_str */
    PyObject_GenericGetAttr,        /* tp_getattro */
    0,                    /* tp_setattro */
    0,                    /* tp_as_buffer */
    0, /* tp_flags */
    0,                    /* tp_doc */
    0,                /* tp_traverse */
    0,                    /* tp_clear */
    0,                    /* tp_richcompare */
    0,                    /* tp_weaklistoffset */
    0,                    /* tp_iter */
    0,                    /* tp_iternext */
    0,                    /* tp_methods */
    event_members,                /* tp_members */
    0,                /* tp_getset */
    0,                    /* tp_base */
    0,                    /* tp_dict */
    0,        /* tp_descr_get */
    0,                    /* tp_descr_set */
};

/* ==================================================================== */
/* readline support */

#ifdef WITH_READLINE

static PyObject* rl_write;
static PyObject* rl_readline;
static PyThreadState *rl_threadstate;

static char*
rl_function(FILE *sys_stdin, FILE *sys_stdout, const char* prompt)
{
    PyObject* res;
    char* p;

    /* switch to original thread state */
    PyThreadState *threadstate;
    threadstate = PyThreadState_Swap(NULL);
    PyEval_RestoreThread(rl_threadstate);

    res = PyObject_CallFunction(rl_write, "s", prompt);
    if (res)
        Py_DECREF(res);
    else {
        /* better than nothing... */
        PyErr_Print();
        PyErr_Clear();
    }

    res = PyObject_CallFunction(rl_readline, NULL);
    if (!res) {
        PyErr_Print();
        goto done;
    }

    if (res == Py_None) {
        p = NULL; /* genereate KeyboardInterrupt */
        goto done;
    }

    if (!PyUnicode_Check(res)) {
        PyErr_SetString(PyExc_ValueError, "readline must return string");
        PyErr_Print();
        goto done;
    }

    p = strdup(PyUnicode_AsUTF8String(res));
    if (!p) {
        PyErr_NoMemory();
        PyErr_Print();
        goto done;
    }

    Py_DECREF(res);

done:
    PyEval_SaveThread();
    PyThreadState_Swap(threadstate);

    return p;
}

static PyObject*
install_readline(ConsoleObject* self, PyObject* args)
{
    /* install readline hook */

    PyObject* handler;
    if (!PyArg_ParseTuple(args, "O", &handler))
        return NULL;

    rl_write = PyObject_GetAttrString(handler, "write");
    if (!rl_write)
        return NULL;

    rl_readline = PyObject_GetAttrString(handler, "readline");
    if (!rl_readline) {
        Py_DECREF(rl_write);
        return NULL;
    }

    rl_threadstate = PyThreadState_Get();

    PyOS_ReadlineFunctionPointer = rl_function;

    Py_INCREF(Py_None);
    return Py_None;
}
#endif

/* ==================================================================== */
/* module stuff */

static PyMethodDef _functions[] = {
    {"Console", (PyCFunction) console_new, 1},
#ifdef WITH_READLINE
    {"install_readline", (PyCFunction) install_readline, 1},
#endif
    {NULL, NULL}
};

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_wincon",     /* m_name */
        "Console for Python3+",  /* m_doc */
        -1,                  /* m_size */
        _functions,    /* m_methods */
        NULL,                /* m_reload */
        NULL,                /* m_traverse */
        NULL,                /* m_clear */
        NULL,                /* m_free */
};

//DL_EXPORT(void)
PyMODINIT_FUNC
PyInit__wincon(void)
{
    /* Patch object type 
    Py_TYPE(Console_Type) = &PyType_Type;
    Py_TYPE(Event_Type) = &PyType_Type; */

    PyObject *m;

    m = PyModule_Create(&moduledef);

    if (m == NULL)
        return NULL;
        
  return m;
}