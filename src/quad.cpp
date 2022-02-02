#include "quad.h"

static double split = 536870913.;

Quad::Quad(double high, double low): h(high), l(low) {}

Quad::Quad(double d): h(d), l(0.) {}

Quad::Quad(): h(0.), l(0.) {}

Quad Quad::toQuad (double d) {
    return Quad(d);
}

Quad Quad::add(Quad b) {
    Quad c;
    double t1, t2, e;

    t1 = h + b.h;
    e = t1 - h;
    t2 = ((b.h - e) + (h - (t1 - e))) + l + b.l;

    c.h = t1 + t2;
    c.l = t2 - (c.h - t1);
    return c;
}

Quad Quad::mul(Quad b) {
    Quad c;
    double c11, c21, c2, e, t1, t2;
    double a1, a2, b1, b2, cona, conb;

    cona = h * split;
    conb = b.h * split;
    a1 = cona - (cona - h);
    b1 = conb - (conb - b.h);
    a2 = h - a1;
    b2 = b.h - b1;

    c11 = h * b.h;
    c21 = a2 * b2 + (a2 * b1 + (a1 * b2 + (a1 * b1 - c11)));

    c2 = h * b.l + l * b.h;

    t1 = c11 + c2;
    e = t1 - c11;
    t2 = l * b.l + ((c2 - e) + (c11 - (t1 - e))) + c21;

    c.h = t1 + t2;
    c.l = t2 - (c.h - t1);

    return c;
}
