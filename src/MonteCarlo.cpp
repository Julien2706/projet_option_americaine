#include "MonteCarlo.hpp"
#include "pnl/pnl_basis.h"
#include <ctime>
#include <iostream>

using namespace std;

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
        for(int j=0; j<opt_->dates_+1; j++){
            pnl_mat_get_row(vect_sub, simul, j);
            pnl_mat_set_row(All, vect_sub, j*nSample_ +i);
        }
        pnl_mat_free(&simul);
        pnl_vect_free(&vect_sub);
    }
    //pnl_mat_print(All);
    PnlBasis *pol = pnl_basis_create_from_degree (PNL_BASIS_HERMITE, deg_, opt_->size_);
    // Vecteur de tps d'arret
    PnlVect* ta = pnl_vect_create_from_scalar(nSample_,opt_->dates_);
    for(int i=opt_->dates_-1; i>0; i--){
        //Matrice x de la methode basis
        PnlMat *M_sub = pnl_mat_create(nSample_, opt_->size_);
        pnl_mat_extract_subblock(M_sub, All, (i+1)*nSample_, nSample_, 0, opt_->size_ );

        //Calcul du payoff
        PnlVect *vectPayoff = pnl_vect_create(nSample_);
        PnlVect* saveRow = pnl_vect_create(opt_->size_);
        for(int j=0; j< nSample_; j++){
            double ta_l = pnl_vect_get(ta,j);
            pnl_mat_get_row(saveRow, All, nSample_*ta_l + j);
            pnl_vect_set(vectPayoff, j, opt_->payoff(saveRow)*exp(-mod_->r_*ta_l*(opt_->T_/opt_->dates_)));
        }

        // Calcul des alphas
        PnlVect* alpha = pnl_vect_create(opt_->size_);
        pnl_basis_fit_ls(pol, alpha, M_sub, vectPayoff);
        PnlVect* saveRow1 = pnl_vect_create(opt_->size_);
        for(int j=0; j< nSample_; j++){
            pnl_mat_get_row(saveRow1, All, nSample_*i + j);
            double payoff_l = opt_->payoff(saveRow1)*exp(-mod_->r_*i*(opt_->T_/opt_->dates_));
            double eval_pol = pnl_basis_eval_vect (pol, alpha,saveRow1);
            //cout << " eval_pol : "<< eval_pol << endl;
            if((payoff_l>=eval_pol) && (eval_pol>=0. )){
                pnl_vect_set(ta,j,i);
            }
        }
        //cout << " date i : "<< i << endl;
        //pnl_vect_print(ta);
        pnl_vect_free(&saveRow);
        pnl_vect_free(&saveRow1);
        pnl_vect_free(&alpha);
        pnl_vect_free(&vectPayoff);
        pnl_mat_free(&M_sub);
    }

    pnl_basis_free(&pol);
    PnlVect* saveRow = pnl_vect_create(opt_->size_);
    pnl_mat_get_row(saveRow, All, 0);
    double payoff_0 = opt_->payoff(saveRow);

    double sum = 0.;
    for(int j=0; j< nSample_; j++){
        double ta_l = pnl_vect_get(ta,j);
        pnl_mat_get_row(saveRow, All, nSample_*ta_l + j);
        double payoff_l = opt_->payoff(saveRow)*exp(-mod_->r_*ta_l*(opt_->T_/opt_->dates_));
        sum += payoff_l;
    }

    pnl_mat_free(&All);
    pnl_rng_free(&rng);
    pnl_vect_free(&ta);

    if(payoff_0>sum/nSample_){
        return payoff_0;
    }
    return sum/nSample_;
}

