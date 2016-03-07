#ifndef XAODBTAGGINGEFFICIENCY_XAODBTAGGINGEFFICIENCYDICT_H
#define XAODBTAGGINGEFFICIENCY_XAODBTAGGINGEFFICIENCYDICT_H

// Special handling for Eigen vectorization (relevant because of inclusion of Jet.h)
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif

#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

#endif // XAODBTAGGINGEFFICIENCY_XAODBTAGGINGEFFICIENCYDICT_H
