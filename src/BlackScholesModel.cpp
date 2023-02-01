#include "BlackScholesModel.hpp"
#include <iostream>
#include <ctime>
using namespace std;

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *divid, PnlVect *spot){
    this->size_=size;
    this->r_=r;
    this->rho_=rho;
    this->sigma_=sigma;
    this->divid=divid;
    this->spot_=spot;
}


void BlackScholesModel::asset(PnlMat *path, double T, int nbTimesStep, PnlRng *rng){
    PnlVect *gaussien = pnl_vect_new();

    // on dit que m est le nombre d'asset
    // et donc n le nombre de de dates
    int numberAsset = this->size_;
    PnlMat *M = pnl_mat_create(numberAsset, numberAsset);
    for (int i=0; i<numberAsset; i++){
        for (int j=0; j< numberAsset; j++){
            if (i==j){
                pnl_mat_set(M, i, j, 1);
            }
            else{
                pnl_mat_set(M, i, j, this->rho_);
            }
        }
    }
    pnl_mat_chol(M);
    PnlVect *lineD = pnl_vect_create(numberAsset);
    for (int i = 0; i<nbTimesStep; i++){
        double timeIntervalle = T/nbTimesStep;
        pnl_vect_rng_normal(gaussien, numberAsset, rng);

        for (int d=0; d<numberAsset; d++){
            
            pnl_mat_get_row(lineD,M,d);
            double arg = (this->r_ - pnl_vect_get(this->divid, d) - (pnl_vect_get(this->sigma_,d)*pnl_vect_get(this->sigma_,d))/2)*(timeIntervalle) + pnl_vect_get(this->sigma_,d)*pnl_vect_scalar_prod(lineD, gaussien)*sqrt(timeIntervalle);
            double expo = exp(arg);
            double out = pnl_mat_get(path,i, d) * expo;
            pnl_mat_set(path, i+1, d, out);

        }
    }
    pnl_vect_free(&lineD);
    pnl_vect_free(&gaussien);
    pnl_mat_free(&M);
}

