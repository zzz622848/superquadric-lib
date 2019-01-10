#ifndef SUPERQMODEL_H
#define SUPERQMODEL_H

#include <Eigen/Dense>

namespace SuperqModel {

/**
* \class SuperqModel::Superquadric
* \headerfile superquadric.h <SuperquadricModel/include/superquadric.h>
*
* \brief A class from SuperqModel namespace.
*
* This class implements a generic Superquadric with 11 parameters.
*/
class Superquadric
{
public:

    int n_params;
    Eigen::VectorXd params;
    Eigen::Vector3d dim;
    Eigen::Vector2d exp;
    Eigen::Vector4d axisangle;
    Eigen::Vector3d ea;
    Eigen::Vector3d center;
    Eigen::Matrix3d axes;

    /**
    * Constructor
    */
    Superquadric();

    /**
    * Destructory
    */
    ~Superquadric() { }

    /**
     * Set all superquadric parameters
     * @param p is a eigen vector of dimension num_params
     * containing the superquadric parameters
     * @return true/false if the parameters are consistent
     */
    bool setSuperqParams(Eigen::VectorXd &p);

    /**
     * Set  superquadric dimensions
     * @param d is a eigen vector of dimension 3
     * containing the superquadric dimensions
     * @return true/false if the parameters are consistent
     */
    bool setSuperqDims(Eigen::Vector3d &d);

    /**
     * Set  superquadric exponents
     * @param e is a eigen vector of dimension 2
     * containing the superquadric exponents
     * @return true/false if the parameters are consistent
     */
    bool setSuperqExps(Eigen::Vector2d &e);

    /**
     * Set  superquadric center coordinates
     * @param c is a eigen vector of dimension 3
     * containing the superquadric center
     * @return true/false if the parameters are consistent
     */
    bool setSuperqCenter(Eigen::Vector3d &c);

    /**
     * Set  superquadric orientation
     * @param o is a eigen vector of dimension 4
     * containing the superquadric orientation as axis angle
     * @return true/false if the parameters are consistent
     */
    bool setSuperqOrientation(Eigen::Vector4d &o);

    /**
     * Set  superquadric orientation
     * @param o is a eigen vector of dimension 3
     * containing the superquadric orientation as euler angles zyz
     * @return true/false if the parameters are consistent
     */
    bool setSuperqOrientation(Eigen::Vector3d &a);

    /**
     * Get all superquadric parameters
     * @return a 11D vector containing the parameters
     */
    Eigen::VectorXd getSuperqParams();

    /**
     * Get  superquadric dimensions
     * @return a 3D vector containing the dimensions
     */
    Eigen::Vector3d getSuperqDims();

    /**
     * Get  superquadric exponents
     * @return a 2D vector containing the exponents
     */
    Eigen::Vector2d getSuperqExps();

    /**
     * Get  superquadric center coordinates
     * @return a 3D vector containing the  enter
     */
    Eigen::Vector3d getSuperqCenter();

    /**
     * Get  superquadric orientation
     * return a 4D vector containing the axis angle
     */
    Eigen::Vector4d getSuperqAxisAngle();

    /**
     * Get  superquadric orientation
     * @return a 3D vector containing the Euler angles
     */
    Eigen::Vector3d getSuperqEulerZYZ();

    /**
     * Get superquadric axes
     * @return a 3x3 Matrix containing the axes
     */
    Eigen::Matrix3d getSuperqAxes();

    /**
     * Compute the inside-outside function of the superquadric
     * @param pose is the pose of the superquadric
     * @param point is a point where we want to evaluate the inside-ouside function
     * @return the value of the inside-outside function
     */
    double insideOutsideF(Eigen::VectorXd &pose, Eigen::Vector3d &point);
};

}

#endif
