#ifndef QUAD_H
#define QUAD_H

class Quad {
    public:
        double h,l;

        Quad(double high, double low);
        Quad(double d);
        Quad();
        Quad toQuad(double d);
        Quad mul(Quad b);
        Quad add(Quad b);
};
#endif
