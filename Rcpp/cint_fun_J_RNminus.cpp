#include <Rcpp.h>
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//



NumericVector LinearSpacedArray(double a, double b, double N)
{
  double h = (b - a) / (N-1);
  NumericVector xs(N);
  int x;
  for (x = 0; x < N; x++) {
    
    if (x == 0) {
      xs[x] = a;
    } else {
      xs[x] = xs[x-1] + h;
    }
  }
  return xs;
  
}


NumericVector allK(NumericVector kSmall, NumericVector kLarge){
  
  NumericVector allK(500);
  int x;
  
  for (x = 0; x < 500; x++){
    
    if (x < 250){
      allK[x] = kSmall[x];
    } else {
      allK[x] = kLarge[x-250];
    }
    
  }
  
  return(allK);
}


NumericVector kSmall = LinearSpacedArray(0.001,10,250);
NumericVector kLarge = LinearSpacedArray(10.001,1e4,250);
NumericVector Kmap = allK(kSmall,kLarge);


NumericVector allJ(NumericVector Kmap){
  
  NumericVector allJ(500);
  int x;
  
  for (x = 0; x < 500; x++){
    
    allJ[x] = Kmap[x] * R::bessel_i(Kmap[x],1,2)/R::bessel_i(Kmap[x],0,2);
    
  }
  
  return(allJ);
}

NumericVector Jmap = allJ(Kmap);

// approxfun linear approximation steal
double cKappaFromJ(double J){
  
  double v = J;
  NumericVector y = Kmap;
  NumericVector x = Jmap;
  
  
  /* Approximate  y(v),  given (x,y)[i], i = 0,..,n-1 */
  int n = Jmap.size();

    int i, j, ij;
    
    //if(!n) return R_NaN;
    
    i = 0;
    j = n - 1;
    
    /* handle out-of-domain points */
    //if(v < x[i]) return Meth->ylow;
    //if(v > x[j]) return Meth->yhigh;
    
    /* find the correct interval by bisection */
    while(i < j - 1) { /* x[i] <= v <= x[j] */
    ij = (i + j)/2; /* i+1 <= ij <= j-1 */
    if(v < x[ij]) j = ij; else i = ij;
    /* still i < j */
    }
    /* provably have i == j-1 */
    
    /* interpolation */
    
    if(v == x[j]) return y[j];
    if(v == x[i]) return y[i];
    /* impossible: if(x[j] == x[i]) return y[i]; */
    
    //if(Meth->kind == 1) /* linear */
    return y[i] + (y[j] - y[i]) * ((v - x[i])/(x[j] - x[i]));
    // else /* 2 : constant */
    //return (Meth->f1 != 0.0 ? y[i] * Meth->f1 : 0.0)
    //  + (Meth->f2 != 0.0 ? y[j] * Meth->f2 : 0.0);
  
}


// [[Rcpp::export]]
NumericVector cint_fun_J_RNminus(
    NumericVector x,
    NumericVector pars, 
    double radian) {
  
  int i;
  int lx = x.size();
  double kappa;
  NumericVector out(lx);
  
  for (i = 0; i < lx; i++) {
    
    if (i > 1e4) {
      kappa = x[i];
    } else {
    kappa = cKappaFromJ(x[i]);
    }
    
    out[i] = R::dgamma(x[i], pars[0]/pars[1], pars[1], 0) * 
      (1 / (2*PI*R::bessel_i(kappa,0,2)) * 
      pow(exp(cos(radian) - 1),kappa));
    
  }
  return(out);
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//


