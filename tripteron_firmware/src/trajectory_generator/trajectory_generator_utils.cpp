#include "trajectory_generator_utils.h"

double CalculatePartialProfileDistance(double initVel, double finalVel, TrajectoryConfig config, TrayectoryProfileType profileType) {

    if(static_cast<int>(profileType) > 2 || initVel < -eps || finalVel < -eps)
        return -1.0; // Error

    // Aceleración final siempre 0
    double distance = 0.0;
    double sectionTime = 0.0;
    double sign = finalVel > initVel ? 1.0 : -1.0;
    double vel = initVel;
    double aux_vel = 0.0;
    double maxAcc = config.accMax;
    double jerk = config.jerk;

    // Jerk > 0
    if(profileType == TrayectoryProfileType::S_CURVE_PARCIAL) {
        sectionTime = maxAcc / jerk;
        distance += (vel * sectionTime + sign * (1.0/6.0) * jerk * sectionTime * sectionTime * sectionTime);
        vel += sign * (0.5 * jerk * sectionTime * sectionTime);
    }

    aux_vel = (profileType == TrayectoryProfileType::TRAPEZOIDAL_PARCIAL ? finalVel :
        finalVel - sign * (maxAcc * sectionTime - 0.5 * jerk * sectionTime * sectionTime));
    if(sign * (aux_vel - vel) < -eps)
        return -1.0; // Error
    sectionTime = sign * (aux_vel - vel) / maxAcc;
    distance += (vel * sectionTime + sign * 0.5 * maxAcc * sectionTime * sectionTime);
    vel = aux_vel;

    // Jerk < 0
    if(profileType != TrayectoryProfileType::TRAPEZOIDAL_PARCIAL) {
        sectionTime = maxAcc / jerk;
        distance += (vel * sectionTime + sign * ((0.5 * maxAcc * sectionTime * sectionTime) - (1.0/6.0) * jerk * sectionTime * sectionTime * sectionTime));
    }

    return distance;
}

