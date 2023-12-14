// Copyright 2023 Haoru Xue
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef BASE_VEHICLE_MODEL__BASE_VEHICLE_MODEL_HPP_
#define BASE_VEHICLE_MODEL__BASE_VEHICLE_MODEL_HPP_

#include <memory>

#include <casadi/casadi.hpp>

#include "base_vehicle_model/base_vehicle_model_config.hpp"
#include "base_vehicle_model/base_vehicle_model_state.hpp"

namespace lmpc
{
namespace vehicle_model
{
namespace base_vehicle_model
{
enum XIndex : casadi_int
{
  PX = 0,  // global or frenet x position
  PY = 1,  // global or frenet y position
  YAW = 2,  // global or frenet yaw
  VX = 3,  // body longitudinal velocity
  VY = 4,  // body lateral velocity
  VYAW = 5  // body yaw rate
};

enum UIndex : casadi_int
{
  FD = 0,
  FB = 1,
  STEER = 2
};

// TODO(haoru): the base vehicle model should be an abstract class.
class BaseVehicleModel
{
public:
  typedef std::shared_ptr<BaseVehicleModel> SharedPtr;
  typedef std::unique_ptr<BaseVehicleModel> UniquePtr;

  explicit BaseVehicleModel(BaseVehicleModelConfig::SharedPtr config);

  void set_base_config(BaseVehicleModelConfig::SharedPtr config);
  const BaseVehicleModelConfig & get_base_config() const;

  BaseVehicleModelState & get_state();
  const BaseVehicleModelState & get_const_state() const;

  /**
   * @brief Get the size of the state variable.
   *
   * @return state variable size.
   */
  virtual size_t nx() const;

  /**
   * @brief Get the size of the control variable.
   *
   * @return control variable size.
   */
  virtual size_t nu() const;

  /**
   * @brief Returns the continuious dynamics.
   *  In the input, "x" (state) and "u" (control) are usually required. additional inputs are optional.
   *  In the output, "x_dot" (time derivative of state) is usually required. additional outputs are optional.
   */
  virtual const casadi::Function & dynamics() const;

  /**
   * @brief Returns the jacobian of the continuious dynamics.
   * In the function, the inputs typically include "x" (state) and "u" (control).
   * The function outputs Jacobian of "x_dot" with respect to x (A matrix) and u (B matrix) evaulated at their given point.
   */
  virtual const casadi::Function & dynamics_jacobian() const;

  /**
   * @brief Returns the discrete dynamics.
   *  In the input, "x" (state) and "u" (control) are usually required. additional inputs are optional.
   *  In the output, "xip1" (next state) is usually required. additional outputs are optional.
   *  The discretization method specified should be as in the config.
   * @return const casadi::Function& discrete dynamics function.
   */
  virtual const casadi::Function & discrete_dynamics() const;

  /**
   * @brief Returns the discrete dynamics Jacobian.
   * In the function, the inputs typically include "x" (state) and "u" (control).
   * The function outputs Jacobian of "xip1" with respect to x (A matrix) and u (B matrix) evaulated at their given point.
   */
  virtual const casadi::Function & discrete_dynamics_jacobian() const;

  /**
   * @brief If the subclassed VD model uses a different state representation,
   *        this function should take "x" and "u",
   *        and return a function that converts them to the base state.
   *
   * @return const casadi::Function& function that converts "x" and "u" to the base state variable.
   */
  virtual const casadi::Function & to_base_state() const;
  virtual const casadi::Function & from_base_state() const;

  /**
   * @brief If the subclassed VD model uses a different control representation,
   *        this function should take "x" and "u",
   *        and return a function that converts them to the base control variable.
   *
   * @return const casadi::Function& function that converts "x" and "u" to the base control variable.
   */
  virtual const casadi::Function & to_base_control() const;
  virtual const casadi::Function & from_base_control() const;

  /**
   * @brief Add constraints to the optimal control problem.
   *
   * @param opti Casadi NLP optimizer
   * @param in "x" (state), "u" (control), "t"(delta t), "xip1" (next state), "uip1" (next control).
   */
  virtual void add_nlp_constraints(casadi::Opti & opti, const casadi::MXDict & in);

  /**
   * @brief calculate longitudinal control based on control variable.
   *
   * @param in "u" (control) required, the rest is optional.
   * @param throttle output throttle in 1-100.
   * @param brake_kpa output brake in kpa.
   */
  virtual void calc_lon_control(
    const casadi::DMDict & in, double & throttle,
    double & brake_kpa) const;

  /**
   * @brief calculate lateral control based on control variable.
   *
   * @param in "u" (control) required, the rest is optional.
   * @param steering_rad output front wheel angle in radian.
   */
  virtual void calc_lat_control(const casadi::DMDict & in, double & steering_rad) const;

  /**
   * @brief calculate throttle based on drive force.
   *        torque vs. RPM needs to monotonically and linearly decrease.
   *        not designed for gas engine car.
   *        override to customize.
   *        `base_state_` needs to be populated beforehand.
   *
   * @param fd total driving force.
   * @return target throttle 1-100.
   */
  virtual double calc_throttle(const double & fd) const;

  /**
   * @brief calculate brake line pressure based on braking force.
   *        pressure is taken at the master cylinder.
   *        this sums front and rear pressures.
   *        override to customize.
   *        `base_state_` needs to be populated beforehand.
   *
   * @param fb total braking force.
   * @return target brake line pressure in kpa
   */
  virtual double calc_brake(const double & fb) const;

  /**
   * @brief the inverse of calc_throttle.
   * calculate drive force given throttle.
   *
   * @param throttle
   * @return double drive force
   */
  virtual double calc_drive_force(const double & throttle);

  /**
   * @brief the inverse of calc_brake.
   * calculate brake force given brake pressure.
   *
   * @param brake_kpa brake master cylinder pressure in kpa
   * @return double brake force
   */
  virtual double calc_brake_force(const double & brake_kpa);

protected:
  BaseVehicleModelConfig::SharedPtr base_config_ {};
  BaseVehicleModelState base_state_;

  casadi::Function dynamics_ {};
  casadi::Function dynamics_jacobian_ {};
  casadi::Function discrete_dynamics_ {};
  casadi::Function discrete_dynamics_jacobian_ {};
  casadi::Function to_base_state_ {};
  casadi::Function to_base_control_ {};
  casadi::Function from_base_state_ {};
  casadi::Function from_base_control_ {};
};
}  // namespace base_vehicle_model
}  // namespace vehicle_model
}  // namespace lmpc
#endif  // BASE_VEHICLE_MODEL__BASE_VEHICLE_MODEL_HPP_
