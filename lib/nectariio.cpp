#include "lib/nectariio.hpp"
#include <math.h>
#include <bits/stdc++.h>

double nectariio::impTempConversion(double r0, double r1, double alpha, double beta)
{
    double a = alpha * pow(10.0, -3.0);
    double b = beta * pow(10.0, -7.0);

    return ((-r0 * a) + sqrt(
                            pow((r0 * a), 2) -
                            4 * r0 * b * (r0 - r1)
                            )) / 
                            (2 * r0 * b);
}

double nectariio::rawRealConverstion(unsigned int fullScale, unsigned int gain, unsigned int raw, unsigned int ref)
{
    return (double)raw / ((double)fullScale * (double)gain) * (double)ref;
}

double nectariio::linearResult(double gradient, double x0, double x1)
{
    return (x1 - x0) / gradient;
}

double nectariio::gradient(double x0, double x1, double y0, double y1)
{
    return (x1 - x0) / (y1 - y0);
}