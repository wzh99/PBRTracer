#ifndef MEDIUM_H
#define MEDIUM_H

#include "ray.h"
#include "spectrum.h"
#include "memory.h"
#include "stringprint.h"

class MediumInteraction;
class Sampler;

class PhaseFunction {
public:
    virtual ~PhaseFunction();
    virtual Float p(const Vector3f &wo, const Vector3f &wi) const = 0;
    virtual Float Sample_p(const Vector3f &wo, Vector3f *wi,
                           const Point2f &u) const = 0;
    virtual string toString() const = 0;
};

inline ostream & operator << (ostream &os, const PhaseFunction &p) {
    os << p.toString();
    return os;
}

class Medium {
public:
    virtual ~Medium() {}
    virtual Spectrum Tr(const Ray &ray, Sampler &sampler) const = 0;
    virtual Spectrum Sample(const Ray &ray, Sampler &sampler, MemoryArena &arena,
                            MediumInteraction *mi) const = 0;

    static bool getMediumScatteringProperties(const string &name, Spectrum *sigma_a, Spectrum *sigma_s);

    inline Float phaseHG(Float cosTheta, Float g) {
        Float denom = 1 + g * g + 2 * g * cosTheta;
        return INV_PI * (1 - g * g) / (denom * sqrt(denom));
    }
};

class HenyeyGreenstein : public PhaseFunction {
public:
    HenyeyGreenstein(Float g) : g(g) {}
    Float p(const Vector3f &wo, const Vector3f &wi) const;
    Float Sample_p(const Vector3f &wo, Vector3f *wi,
                   const Point2f &sample) const;
    string toString() const {
        return StringPrint::printf("[ HenyeyGreenstein g: %f ]", g);
    }

private:
    const Float g;
};

struct MediumInterface {
    MediumInterface() : inside(nullptr), outside(nullptr) {}
    MediumInterface(const Medium *medium) : inside(medium), outside(medium) {}
    MediumInterface(const Medium *inside, const Medium *outside)
        : inside(inside), outside(outside) {}
    bool isMediumTransition() const { return inside != outside; }

    const Medium *inside, *outside;
};

#endif // MEDIUM_H