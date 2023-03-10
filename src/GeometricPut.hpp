#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"

/// \brief Classe Option abstraite
class GeometricPut : public Option
{
public:
  // double T_;        /// maturité
  // int nbTimeSteps_; /// nombre de pas de temps de discrétisation
  // int size_;        /// dimension du modèle, redondant avec BlackScholesModel::size_
  PnlVect *lambda_;
  double strike_;

  GeometricPut(double T, int dates, int size, double strike);

  ~GeometricPut();
  /**
   * Calcule la valeur du payoff sur la trajectoire
   *
   * @param[in] path est une matrice de taille (N+1) x d
   * contenant une trajectoire du modèle telle que créée
   * par la fonction asset.
   * @return phi(trajectoire)
   */
  double payoff(const PnlVect *values) override;
};