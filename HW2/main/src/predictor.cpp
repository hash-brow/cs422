/*
 * predictor.cpp
 * CS422 HW2 - Part A
 */

#include "../include/predictor.h"

GAg::GAg() : pht(PHT_SZ, 0) 
{
}

bool GAg::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        bool pred = pht[bht] > (PHT_MAX / 2);

        if (data.taken != pred) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }

        if (data.taken && pht[bht] < PHT_MAX) pht[bht]++;
        else if (!data.taken && pht[bht]) pht[bht]--;

        bht = static_cast<int>( 
                        ((static_cast<long long>(bht) << 1LL) | static_cast<long long>(data.taken)) 
                        % PHT_SZ );

        return pred;
}

SAg::SAg() : bht(BHT_SZ, 0) , pht(PHT_SZ, 0)
{
}

bool SAg::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        int bht_idx = static_cast<int>(data.addr_ins & (BHT_SZ - 1));
        bool pred = pht[bht[bht_idx]] > (PHT_MAX / 2);

        if (data.taken != pred) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }

        if (data.taken && pht[bht[bht_idx]] < PHT_MAX) pht[bht[bht_idx]]++;
        else if (!data.taken && pht[bht[bht_idx]]) pht[bht[bht_idx]]--;

        bht[bht_idx] = static_cast<int>( 
                        ((static_cast<long long>(bht[bht_idx]) << 1LL) | static_cast<int>(data.taken)) 
                        % PHT_SZ);

        return pred;
}

Gshare::Gshare() : pht(PHT_SZ, 0)
{
}

bool Gshare::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        const int idx = bht ^ static_cast<int>(static_cast<long long>(data.addr_ins) % PHT_SZ);
        assert(idx < PHT_SZ);

        bool pred = pht[idx] > (PHT_MAX / 2);

        if (data.taken != pred) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }

        if (data.taken && pht[idx] < PHT_MAX) pht[idx]++;
        else if (!data.taken && pht[idx]) pht[idx]--;

        bht = static_cast<int>( 
                        ((static_cast<long long>(bht) << 1LL) | static_cast<long long>(data.taken)) 
                        % PHT_SZ );

        return pred;
}

Hybrid::Hybrid() : pht(PHT_SZ, 0)
{
}

bool Hybrid::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        bool upd = data.result->GAg != data.result->SAg;

        bool pred;
        if (pht[bht] > (PHT_MAX / 2)) {
                pred = data.result->GAg;
                
                if (upd) {
                        if (pred != data.taken) pht[bht]--;
                        else if (pht[bht] < PHT_MAX) pht[bht]++;
                }
        } else {
                pred = data.result->SAg;

                if (upd) {
                        if (pred != data.taken) pht[bht]++;
                        else if (pht[bht]) pht[bht]--;
                }
        }

        if (pred != data.taken) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }

        bht = static_cast<int>( 
                        ((static_cast<long long>(bht) << 1LL) | static_cast<long long>(data.taken)) 
                        % PHT_SZ );

        return pred;
}

bool Hybrid_majority::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        std::vector<int> cnt(2, 0);
        cnt[static_cast<int>(data.result->SAg)]++;
        cnt[static_cast<int>(data.result->GAg)]++;
        cnt[static_cast<int>(data.result->gshare)]++;

        bool pred = cnt[1] > cnt[0];
        if (pred != data.taken) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }
        
        return pred;
}

Hybrid_tournament::Hybrid_tournament() : pht(3, std::vector<int>(PHT_SZ, 0))
{
} 

bool Hybrid_tournament::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        /* 0 = sag, 1 = gag, 2 = gshare
         * 0 -> sag vs gag
         * 1 -> sag vs gshare
         * 2 -> gag vs gshare
         */ 
        int winner = 0;
        if (pht[0][bht] > (PHT_MAX/2)) winner = 1;
        if (pht[1 + winner][bht] > (PHT_MAX/2)) winner = 2;

        auto get_pred {
                [&data](int idx) -> bool {
                        if (idx == 0) return data.result->SAg;
                        else if (idx == 1) return data.result->GAg;
                        return data.result->gshare;
                }
        };

        auto upd_table {
                [this, &data, &get_pred](int idx, int a, int b) -> void {
                        bool pa = get_pred(a), pb = get_pred(b);

                        bool pred;
                        bool is_b = pht[idx][bht] > (PHT_MAX/2);
                        if (is_b) pred = pb;
                        else pred = pa;

                        if (pred != data.taken) {
                                if (!is_b) pht[idx][bht]++;
                                else pht[idx][bht]--;
                        } else {
                                if (is_b && pht[idx][bht] < PHT_MAX) pht[idx][bht]++;
                                else if (!is_b && pht[idx][bht]) pht[idx][bht]--;
                        }
                }
        };

        upd_table(0, 0, 1);
        upd_table(1, 0, 2);
        upd_table(2, 1, 2);

        bool pred = get_pred(winner);
        if (pred != data.taken) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }

        bht = static_cast<int>( 
                        ((static_cast<long long>(bht) << 1LL) | static_cast<long long>(data.taken)) 
                        % PHT_SZ );

        return pred;
}

Bimodal::Bimodal() : pht(PHT_SZ, 0)
{
}

bool Bimodal::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        int pht_idx = static_cast<int>(static_cast<long long>(data.addr_ins) % (PHT_SZ));

        bool pred = pht[pht_idx] > (PHT_MAX/2);

        if (pred != data.taken) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }
        if (data.taken && pht[pht_idx] < PHT_MAX)
                pht[pht_idx]++;
        else if (!data.taken && pht[pht_idx])
                pht[pht_idx]--;

        return pred;
}

bool FNBT::operator+(const data_t data) {
        bool is_fwd = data.addr_branch > data.addr_ins;

        if (is_fwd) fwd_branches++;
        else bwd_branches++;

        bool pred = !is_fwd;

        if (pred != data.taken) {
                if (is_fwd) fwd_mispred++;
                else bwd_mispred++;
        }
        return pred;
}
                        
