#ifndef PID_H
#define PID_H

#include <stdio.h>
#include <functional>
#include <atomic>

#define A(kp, ki, kd, ts) (kp + ki * ts / 2 + kd / ts)
#define B(kp, ki, kd, ts) ((-kp) + ki * ts / 2 - 2 * kd / ts)
#define C(kd, ts) (kd / ts)

namespace pid
{   

    class PidParameter
    {
        #define PID_MAX_ERROR 3

        private:
            std::atomic<double> setPoint{0};
            std::atomic<int> kp{0};
            std::atomic<int> ki{0};
            std::atomic<int> kd{0};
            std::atomic<int> ts{0};
            std::atomic<double> output{0};
            std::atomic<double> err[PID_MAX_ERROR];

        public:
            PidParameter();

            PidParameter(double SP, int KP, int KI, int KD, int TS);

            void setSetPoint(double SP);

            void setKP(int KP);

            void setKI(int KI);

            void setKD(int KD);

            void setTS(int TS);

            void setOutput(double _output);

            void pushError(double err);

            double getSetPoint();

            int getKP();

            int getKI();

            int getKD();

            int getTS();

            double getOutput();

            double getError(int pos);

    };

    /**
     * @brief Incremental PID control equation
     * @param u1 previous output signal 
     * @param kp Proportial coeffecient
     * @param ki Intergral coeffecient
     * @param kd Differential coeffecient
     * @param ts Time step in milli seconds
     * @param err0 The current error e(k)
     * @param err1 The previous error e(k-1)
     * @param err2 The previous previous error e(k-2)
     * @return The new output signal
     */
    double outputSignal(double u1,
                        int kp, int ki, int kd, 
                        int ts, 
                        double err0, double err1, double err2);

    /**
     * @brief Clips the output signal to within specified limits
     * @param signal Control signal to clip
     * @param min The minimum allowed output signal
     * @param max The maximum allowed outpu signal
     * @return The clipped output signal
     */
    double clipSignal(double signal, double min, double max);

    /**
     * @brief Calculates the errors and shifts the errors down a spot
     * @param setPoint The set point of the PID
     * @param read The sensor reading
     * @param errors An array representing the history of the error values, zero is most current
     * @return void
     */
    void calcErrors(PidParameter &parameters, double read);

    /**
     * @brief The PID control algorithm. Run in a loop for continuous updating.
     * @param parameters The control process parameters data structure. 
     * @param readFunc Function to read the sensor data
     * @param writeFunction Function to update the plant
     * @return void
     */
    double doControl(PidParameter &parameters);
}

#endif