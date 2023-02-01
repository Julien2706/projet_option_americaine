#include "BestOff.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


BestOff::BestOff(double T, int dates, int size, PnlVect *lambda, double strike) {
    this->T_ = T;
    this->dates_ = dates;
    this->size_ = size;
    this->lambda_ = lambda;
    this->strike_ = strike;
}

double BestOff::payoff(const PnlMat *path, double time){
    int line_indice = time/(this->T_/this->dates_);
    PnlVect *line_i = pnl_vect_new();
    pnl_mat_get_row(line_i, path, line_indice);
    double maxi = 0.;
    double current;
    for(int i=0; i<path->n; i++){
        current =  pnl_mat_get(path, line_indice, i) * pnl_vect_get(lambda_, i) ;
        if(current>maxi){
            maxi=current;
        }
    }
    double payoff = maxi - strike_;
    if (payoff > 0 ){
        return payoff;
    }
    return 0;
    
}