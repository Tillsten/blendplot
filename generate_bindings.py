"""
This script generates bybind11 boilerplate by loading the header file
with cppyy. There is properly a smarter way to do it but it works.
"""

import cppyy
cppyy.add_include_path(
    r'C:\Users\tills\OneDrive\Dokumente\blend2d-beta12-all\blend2d\src')
cppyy.include('blend2d.h')
g = cppyy.gbl

code = """
#include <blend2d.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#define ULong64_t size_t
#define Long64_t intptr_t

namespace py = pybind11;

PYBIND11_MODULE(pyblend2d, m) {
    m.doc() = "Blend2D bindings";
"""


def w(i, s):
    intend = '    '
    global code
    code += i * intend + s + '\n'


def argsplit(s):
    pos = [-1]
    pstack = []
    out = []
    for i, c in enumerate(s):
        if c == '(':
            pstack.append(i)
        elif c == ')':
            pstack.pop()
        if len(pstack) > 0:
            pass
        else:
            if c == ',':
                pos.append(i)
                out.append(s[1 + pos[-2]:i])
    out.append(s[pos[-1] + 1:])
    return out


def ga(s: str):

    args = argsplit(s)
    out = []

    for a in args:
        if "=" in a:
            print(s)
            print(a)
            tmp = a[:a.find('=') - 1]
            tmp.rfind(' ')
            tmp = tmp[:tmp.rfind(' ')]
            
        else:
            w = a.split(' ')
            if not w[-1].endswith('&'):
                w.pop(-1)
            tmp = ' '.join(w)
        out.append(tmp)
    out = ','.join(out)
    return out


stdov = ['==', '!=']
clist = [
    ("BLBox", ["contains"], stdov),
    ("BLMatrix2D", ["mapPoint", "mapVector"], stdov), ("BLImage", [], stdov),
    ("BLPoint", [], stdov), ("BLPointI", [], stdov),
    ("BLContext", ['getStyle', 'getStrokeStyle', 'getFillStyle'], stdov),
    ("BLRgba64", [], stdov), ("BLRgba32", [], stdov), ("BLPattern", [], stdov),
    ("BLPath", ['getClosestVertex'], stdov), ("BLRect", [], stdov),
    ("BLFont", ['getGlyphOutlines', 'getGlyphRunOutlines',
                'mapTextToGlyphs'], stdov), ("BLFontFace", [], stdov),
    ("BLCircle", [], stdov), ("BLGlyphRun", [], []), ("BLGlyphInfo", [], []),
    ("BLArc", [], stdov)
]

flist = [("operator+", [], '')]

for klass, const_fcn, overload in clist:
    c = getattr(g, klass)

    w(1, ('py::class_<%s>(m, "%s")' % (klass, klass)))
    for l in c.__init__.func_doc.splitlines():
        args = l[l.find('(') + 1:-1]
        if '&&' in args:
            continue
        w(2, ".def(py::init<%s>())" % (ga(args)))

    for op in overload:
        w(2, ".def(py::self %s py::self)" % op)

    for m in dir(c):
        if m in ("__assign__", "__init__", "__bool__", "__len__", "modifyOp"):
            continue
        try:
            attr = getattr(c, m)
            if hasattr(attr, 'func_doc'):

                decl = attr.func_doc.splitlines()
                for d in decl:
                    print(d)
                if len(decl) > 1:
                    for l in decl:
                        l = l.replace('__attribute__((cdecl))', '')

                        args = l[l.find('(') + 1:l.rfind(')')]
                        if '&&' in args:
                            continue
                        if m in const_fcn:
                            extra = ', py::const_'
                        else:
                            extra = ''
                        w(
                            2,
                            '.def("%s", py::overload_cast<%s> (&%s::%s%s))' %
                            (m, ga(args), klass, m, extra))
                else:
                    w(2, '.def("%s", (&%s::%s))' % (m, klass, m))

        except:
            pass
    code += (';\n')

for f, c, ov in flist:
    c = getattr(g, f)

code += "}"
with open('src/pyblend2d.cpp', "w") as f:
    f.write(code)