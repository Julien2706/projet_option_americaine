#include <iostream>
#include <ctime>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "PricingResults.hpp"
#include "BlackScholesModel.hpp"
#include "jlparser/parser.hpp"
#include "Option.hpp"
#include "Basket.hpp"
#include "GeometricPut.hpp"
#include "BestOff.hpp"
#include "MonteCarlo.hpp"


using namespace std;

int main(int argc, char **argv)
{
    double T, r, strike, rho;
    PnlVect *spot, *sigma, *divid, *lambda;
    string type;
    int size, nbTimesStep, deg;
    int n_samples;
    char *infile = argv[1];
    Param *P = new Parser(infile);

    P->extract("option type", type);
    P->extract("dates", nbTimesStep);
    P->extract("maturity", T);
    P->extract("model size", size);
    P->extract("spot", spot, size);
    P->extract("correlation", rho);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    P->extract("degree for polynomial regression", deg);
    if (P->extract("dividend rate", divid, size, true) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("MC iterations", n_samples);

    Option* opt;

    if (type == "exchange"){
        P->extract("payoff coefficients", lambda, size);
        opt = new Basket(T, nbTimesStep, size, lambda, strike);
    }
    else if (type == "geometric_put"){
        opt = new GeometricPut(T, nbTimesStep, size, strike);
    }
    else if (type == "bestof"){
        P->extract("payoff coefficients", lambda, size);
        opt = new BestOff(T, nbTimesStep, size, lambda, strike);
    }

    BlackScholesModel *mod = new BlackScholesModel(size, r, rho, sigma, divid, spot);
    MonteCarlo *mc = new MonteCarlo(mod, opt, n_samples, deg);
    double price = mc->price();

    cout << PricingResults(price) << endl;
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    if (type != "geometric_put"){
        pnl_vect_free(&lambda);
    }
    return 0;
}
