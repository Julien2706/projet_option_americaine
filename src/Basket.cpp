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

double Basket::payoff(const PnlVect *values){
    double payoff = pnl_vect_scalar_prod(lambda_, values) - strike_;
    if (payoff > 0 ){
        return payoff;
    }
    return 0;
    
}
