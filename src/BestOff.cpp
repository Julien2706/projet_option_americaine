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

double BestOff::payoff(const PnlVect *values){
    double maxi = 0.;
    double current;
    for(int i=0; i<values->size; i++){
        current =  pnl_vect_get(values, i) * pnl_vect_get(lambda_, i) ;
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