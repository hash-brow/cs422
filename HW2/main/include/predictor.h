#ifndef _PREDICTOR_H
#define _PREDICTOR_H

#include "pin.H"

typedef struct _result_t {
        bool gshare { false };
        bool GAg { false };
        bool SAg { false };
} result_t;

typedef struct _data_t {
        ADDRINT addr_ins;
        ADDRINT addr_branch;
        bool taken;

        std::optional<result_t> result;
} data_t;

class Predictor {
protected:
        long long fwd_branches { 0 };
        long long fwd_mispred { 0 };
        long long bwd_branches { 0 };
        long long bwd_mispred { 0 };
public:
        friend inline std::ostream& operator<< (std::ostream& out, const Predictor &pred);

        virtual bool operator+(const data_t data) = 0;
};

inline std::ostream& operator<< (std::ostream& out, const Predictor &pred) {
        auto prnt {
                [&out](long long a, long long b) -> std::string 
                {
                        out << a << " (" << static_cast<double>(a)/b << ")";
                        return "";
                }
        };

        const long long tot = pred.fwd_branches + pred.bwd_branches;
        const long long mispred = pred.fwd_mispred + pred.bwd_mispred;
        out << "Accesses " << tot << ", Mispredictions " << prnt(mispred, tot) <<
                ", Forward Branches " << pred.fwd_branches << ", Forward Mispredictions " << prnt(pred.fwd_mispred, pred.fwd_branches) <<
                ", Backward Branches " << pred.bwd_branches << ", Backward Mispredictions " << prnt(pred.bwd_mispred, pred.bwd_branches);
        return out;
}

class GAg : public Predictor {
private:
        const int PHT_MAX = 7;
        const long long PHT_SZ = 512;

        int bht { 0 };
        std::vector<int> pht;
public: 
        GAg();
        bool operator+(const data_t data) override;
};

class SAg : public Predictor {
private:
        const int PHT_MAX = 3;
        const long long BHT_SZ = 1024;
        const long long PHT_SZ = 512;

        std::vector<int> bht;
        std::vector<int> pht;
public:
        SAg();
        bool operator+(const data_t data) override;
};

class Gshare : public Predictor {
private:
        const int PHT_MAX = 7; 
        const long long PHT_SZ = 512; 
        
        int bht { 0 };
        std::vector<int> pht;
public:
        Gshare();
        bool operator+(const data_t data) override;
};

// hybrid of SAg and GAg
class Hybrid : public Predictor {
private:
        const int PHT_MAX = 3;
        const long long PHT_SZ = 512;

        int bht { 0 };
        std::vector<int> pht;
public:
        Hybrid();
        bool operator+(const data_t data) override;
};

// majority from SAg, GAg, gshare
class Hybrid_majority : public Predictor {
public:
        bool operator+(const data_t data) override;
};

// tournament b/w SAg, GAg, gshare
class Hybrid_tournament : public Predictor {
private:
        const int PHT_MAX = 3;
        const long long PHT_SZ = 512;

        int bht { 0 };
        // 3x512
        std::vector<std::vector<int>> pht;
public:
        Hybrid_tournament();
        bool operator+(const data_t data) override;
};

class Bimodal : public Predictor {
private:
        const int PHT_MAX = 3;
        const long long PHT_SZ = 512;

        std::vector<int> pht;
public:
        Bimodal();
        bool operator+(const data_t data) override;
};

class FNBT : public Predictor {
public:
        bool operator+(const data_t data) override;
};

#endif // _PREDICTOR_H
