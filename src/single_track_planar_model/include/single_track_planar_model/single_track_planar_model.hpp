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

#ifndef SINGLE_TRACK_PLANAR_MODEL__SINGLE_TRACK_PLANAR_MODEL_HPP_
#define SINGLE_TRACK_PLANAR_MODEL__SINGLE_TRACK_PLANAR_MODEL_HPP_

#include <memory>

#include <casadi/casadi.hpp>

#include "base_vehicle_model/base_vehicle_model.hpp"

namespace lmpc
{
namespace vehicle_model
{
namespace single_track_planar_model
{
struct SingleTrackPlanarModelConfig
{
  typedef std::shared_ptr<SingleTrackPlanarModelConfig> SharedPtr;

  double Fd_max;
  double Fb_max;
  double Td;
  double Tb;
  double v_max;
  double P_max;
  double mu;
  bool simplify_lon_control;
};

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

enum UIndexSimple : casadi_int
{
  LON = 0,
  STEER_SIMPLE = 1
};

class SingleTrackPlanarModel final : public base_vehicle_model::BaseVehicleModel
{
public:
  typedef std::shared_ptr<SingleTrackPlanarModel> SharedPtr;
  typedef std::unique_ptr<SingleTrackPlanarModel> UniquePtr;

  SingleTrackPlanarModel(
    base_vehicle_model::BaseVehicleModelConfig::SharedPtr base_config,
    SingleTrackPlanarModelConfig::SharedPtr config);

  const SingleTrackPlanarModelConfig & get_config() const;

  size_t nx() const override;
  size_t nu() const override;

  void add_nlp_constraints(casadi::Opti & opti, const casadi::MXDict & in) override;
  void calc_lon_control(
    const casadi::DMDict & in, double & throttle,
    double & brake_kpa) const override;
  void calc_lat_control(const casadi::DMDict & in, double & steering_rad) const override;

private:
  void compile_dynamics();

  SingleTrackPlanarModelConfig::SharedPtr config_ {};
};
}  // namespace single_track_planar_model
}  // namespace vehicle_model
}  // namespace lmpc
#endif  // SINGLE_TRACK_PLANAR_MODEL__SINGLE_TRACK_PLANAR_MODEL_HPP_
