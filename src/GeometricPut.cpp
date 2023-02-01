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

double GeometricPut::payoff(const PnlMat *path, double time){
    int line_indice = time/(this->T_/this->dates_);
    PnlVect *line_i = pnl_vect_new();
    pnl_mat_get_row(line_i, path, line_indice);
    double prod =1;
    for(int i=0; i<path->n; i++){
        prod = prod * pnl_mat_get(path, line_indice, i);
    }
    
    double denominateur = 1/path->n;
    prod = pow(prod,1.0/path->n);
    double payoff = strike_ - prod;
    if (payoff > 0 ){
        return payoff;
    }
    return 0;
}