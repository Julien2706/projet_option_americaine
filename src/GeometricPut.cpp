#include "GeometricPut.hpp"
#include <math.h>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


GeometricPut::GeometricPut(double T, int dates, int size, double strike) {
    this->T_ = T;
    this->dates_ = dates;
    this->size_ = size;
    this->strike_ = strike;
}

double GeometricPut::payoff(const PnlVect *values){
    double prod =1;
    for(int i=0; i<values->size; i++){
        prod = prod * pnl_vect_get(values, i);
    }
    prod = pow(prod,1.0/values->size);
    double payoff = strike_ - prod;
    if (payoff > 0 ){
        return payoff;
    }
    return 0;
}