#ifndef ANT_TYPES_H
#define ANT_TYPES_H

#include <ostream>
#include <printable.h>


namespace ant {

typedef double radian_t;
typedef double mev_t;
typedef double ns_t;
typedef unsigned int index_t;
typedef unsigned int clustersize_t;
typedef int element_index_t;

class detector_t : public printable_traits {
private:
    unsigned int v;
    detector_t(unsigned int _v) : v(_v) {}
public:
    detector_t() : v(0) {}

    const static detector_t None;
    const static detector_t NaI;
    const static detector_t PID;
    const static detector_t MWPC;
    const static detector_t BaF2;
    const static detector_t PbWO4;
    const static detector_t Veto;
    const static detector_t anyCB;
    const static detector_t anyTAPS;
    const static detector_t anyVeto;

    bool operator==(const detector_t& o) const {
        return v==o.v;
    }

    bool operator!=(const detector_t& o) const {
        return v!=o.v;
    }

    operator bool() const {
        return v;
    }

    detector_t operator&(const detector_t& o) const {
        return v & o.v;
    }

    detector_t operator^(const detector_t& o) const {
        return v ^ o.v;
    }

    detector_t operator^=(const detector_t&o) {
        v ^= o.v;
        return *this;
    }

    bool operator<(const detector_t& o) const {
        return v < o.v;
    }

    bool operator>(const detector_t& o) const {
        return v > o.v;
    }

    detector_t& operator|=(const detector_t& o) {
        v |= o.v;
        return *this;
    }

    detector_t operator|(const detector_t& o) const {
        return  detector_t(v | o.v);
    }

    std::ostream& Print(std::ostream &stream) const;
};

}

#endif
