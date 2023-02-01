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
    int size, nbTimesStep;
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


    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    BlackScholesModel *mod = new BlackScholesModel(size, r, rho, sigma, divid, spot);
    //PnlMat *path = pnl_mat_create(nbTimesStep +1, size);
    //pnl_mat_set_row(path, spot, 0);
    //mod->asset(path, T, nbTimesStep, rng);
    MonteCarlo *mc = new MonteCarlo(mod, opt, 5);
    mc->price();
    //pnl_mat_print(path);
    PnlVect *G = pnl_vect_new();
    //double price = opt->payoff(path, 2*T/nbTimesStep);
    //cout << price << endl;

    int M = 1E5;
    int dim = 2;

    double acc = 0.;

    for (int i = 0; i < M; i++)
    {
        pnl_vect_rng_normal(G, dim, rng);
        double tmp = pnl_vect_norm_two(G);
        acc += tmp;
    }

    acc /= M;

    cout << PricingResults(acc) << endl;


    pnl_vect_free(&G);
    //pnl_mat_free(&path);
    pnl_rng_free(&rng);
    return 0;
}
