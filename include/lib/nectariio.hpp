#ifndef NECTAR_IIO_H
#define NECTAR_IIO_H

namespace nectariio
{
/**
 * @brief Converts an rtd impedence to a temperature value. Valid for temperature zero and above
 * @param r0 resistance at zero degrees celcius
 * @param r1 resistance of the rtd at unknow temperature
 * @param alpha the alpha/A coeffecient of the RTD specification (ignoring precision)
 * @param beta the beta/B coeffecient of the RTD specification (ignoring precision)
 * @return Returns the temperature in degrees celcius for the corresponding resistance value
*/
double impTempConversion(double r0, double r1, double alpha, double beta);

/**
 * @brief Converts a raw ADC output code to a real world value
 * @param fullScale The value of the full scale output code (max value)
 * @param gain The gain of the amplifier, set to 1 if there is none
 * @param raw The raw output code of the ADC
 * @param ref Either the referece voltage or impedence of the ADC measurement
 * @return Returns the real world value of the ADC (unspecified units)
*/
double rawRealConverstion(unsigned int fullScale, unsigned int gain, unsigned int raw, unsigned int ref);

/**
 * @brief Calculates the resultant value of a linear model 
 * @param gradient The gradient of the linear model
 * @param x0 Starting value 
 * @param x1 Point of interest to calculate result at
 * @return The result at point x1
*/ 
double linearResult(double gradient, double x0, double x1);

/**
 * @brief Calculates the inverse gradient of a linear model (i.e 1 / m) 
 * @param x0 Starting point on x axis
 * @param x1 End point on x axis
 * @param y0 Result at point x0
 * @param y1 Result at point x1 
 */
double gradient(double x0, double x1, double y0, double y1);

}
#endif