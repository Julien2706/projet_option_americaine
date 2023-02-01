#include "MonteCarlo.hpp"
#include "pnl/pnl_basis.h"
#include <ctime>

MonteCarlo::MonteCarlo(BlackScholesModel *mod, Option *opt, int nSample, int deg){
    this->mod_ = mod;
    this->opt_ = opt;
    this->nSample_ = nSample;
    this->deg_ = deg;
}

double MonteCarlo::price(){
    PnlMat *All = pnl_mat_create(nSample_ * (opt_->dates_+1), mod_->size_);
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    for (int i=0; i < nSample_; i++){
        PnlMat *simul = pnl_mat_create(opt_->dates_ +1, mod_->size_);
        pnl_mat_set_row(simul, mod_->spot_, 0);
        mod_->asset(simul, opt_->T_, opt_->dates_, rng);
        PnlVect *vect_sub = pnl_vect_create(opt_->size_);
        for(int j=0; j< opt_->dates_+1; j++){
            pnl_mat_get_row(vect_sub, simul, j);
            pnl_mat_set_row(All, vect_sub, j*nSample_ +i);
        }
    }
    PnlBasis *pol = pnl_basis_create_from_degree (PNL_BASIS_HERMITE, deg_, opt_->size_);
    for(int i=opt_->dates_; i>0; i--){
        PnlMat *M_sub = pnl_mat_create(nSample_, opt_->size_);
        pnl_mat_extract_subblock(M_sub, All, i*nSample_, nSample_, 0, opt_->size_ );
        PnlVect *vectPayoff = pnl_vect_create(nSample_);
        for(int j=0; j< nSample_; j++){
            pnl_vect_set(vectPayoff, j, opt_->payoff())
        }
    }

}

