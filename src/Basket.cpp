#include "Basket.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


Basket::Basket(double T, int dates, int size, PnlVect *lambda, double strike) {
    this->T_ = T;
    this->dates_ = dates;
    this->size_ = size;
    this->lambda_ = lambda;
    this->strike_ = strike;
}

double Basket::payoff(const PnlMat *path, double time){
    double line_indice = time/(this->T_/this->dates_);
    PnlVect *line_i = pnl_vect_new();
    pnl_mat_get_row(line_i, path, line_indice);
    double payoff = pnl_vect_scalar_prod(lambda_, line_i) - strike_;
    if (payoff > 0 ){
        return payoff;
    }
    return 0;
    
}
