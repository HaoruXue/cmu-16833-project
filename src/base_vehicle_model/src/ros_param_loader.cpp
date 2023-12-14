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

#include <string>
#include <memory>
#include <vector>

#include <lmpc_utils/ros_param_helper.hpp>

#include "base_vehicle_model/ros_param_loader.hpp"

namespace lmpc
{
namespace vehicle_model
{
namespace base_vehicle_model
{
BaseVehicleModelConfig::SharedPtr load_parameters(rclcpp::Node * node)
{
  auto declare_double = [&](const char * name) {
      return lmpc::utils::declare_parameter<double>(node, name);
    };
  auto declare_vec = [&](const char * name) {
      return lmpc::utils::declare_parameter<std::vector<double>>(node, name);
    };
  auto declare_bool = [&](const char * name) {
      return lmpc::utils::declare_parameter<bool>(node, name);
    };
  auto declare_string = [&](const char * name) {
      return lmpc::utils::declare_parameter<std::string>(node, name);
    };

  auto front_tyre_config = std::make_shared<TyreConfig>(
    TyreConfig{
          declare_double("front_tyre.radius"),
          declare_double("front_tyre.width"),
          declare_double("front_tyre.mass"),
          declare_double("front_tyre.moi"),
          declare_double("front_tyre.pacejka_b"),
          declare_double("front_tyre.pacejka_c"),
          declare_double("front_tyre.pacejka_e"),
          declare_double("front_tyre.pacejka_fz0"),
          declare_double("front_tyre.pacejka_eps")
        }
  );

  auto rear_tyre_config = std::make_shared<TyreConfig>(
    TyreConfig{
          declare_double("rear_tyre.radius"),
          declare_double("rear_tyre.width"),
          declare_double("rear_tyre.mass"),
          declare_double("rear_tyre.rot"),
          declare_double("rear_tyre.pacejka_b"),
          declare_double("rear_tyre.pacejka_c"),
          declare_double("rear_tyre.pacejka_e"),
          declare_double("rear_tyre.pacejka_fz0"),
          declare_double("rear_tyre.pacejka_eps")
        }
  );

  auto front_brake_config = std::make_shared<BrakeConfig>(
    BrakeConfig{
          declare_double("front_brake.max_brake"),
          declare_double("front_brake.brake_pad_out_r"),
          declare_double("front_brake.brake_pad_in_r"),
          declare_double("front_brake.brake_pad_friction_coeff"),
          declare_double("front_brake.piston_area"),
          declare_double("front_brake.bias")
        }
  );

  auto rear_brake_config = std::make_shared<BrakeConfig>(
    BrakeConfig{
          declare_double("rear_brake.max_brake"),
          declare_double("rear_brake.brake_pad_out_r"),
          declare_double("rear_brake.brake_pad_in_r"),
          declare_double("rear_brake.brake_pad_friction_coeff"),
          declare_double("rear_brake.piston_area"),
          declare_double("rear_brake.bias")
        }
  );

  auto steer_config = std::make_shared<SteerConfig>(
    SteerConfig{
          declare_double("steer.max_steer_rate"),
          declare_double("steer.max_steer"),
          declare_double("steer.turn_left_bias")
        }
  );

  auto chassis_config = std::make_shared<ChassisConfig>(
    ChassisConfig{
          declare_double("chassis.total_mass"),
          declare_double("chassis.sprung_mass"),
          declare_double("chassis.unsprung_mass"),
          declare_double("chassis.cg_ratio"),
          declare_double("chassis.cg_height"),
          declare_double("chassis.wheel_base"),
          declare_double("chassis.tw_f"),
          declare_double("chassis.tw_r"),
          declare_double("chassis.moi"),
          declare_double("chassis.b"),
          declare_double("chassis.fr")
        }
  );

  auto aero_config = std::make_shared<AeroConfig>(
    AeroConfig{
          declare_double("aero.air_density"),
          declare_double("aero.drag_coeff"),
          declare_double("aero.frontal_area"),
          declare_double("aero.cl_f"),
          declare_double("aero.cl_r")
        }
  );

  auto powertrain_config = std::make_shared<PowerTrainConfig>(
    PowerTrainConfig{
          lmpc::utils::Lookup3D{
            declare_vec("powertrain.rpm"),
            declare_vec("powertrain.throttle"),
            declare_vec("powertrain.torque")
          },
          declare_vec("powertrain.gear_ratio"),
          declare_double("powertrain.final_drive_ratio"),
          declare_double("powertrain.kd"),
          declare_double("powertrain.mechanical_efficiency"),
        }
  );

  std::string integratory_name = declare_string("modeling.integrator_type");
  IntegratorType integrator_type;
  if (integratory_name == "rk4") {
    integrator_type = IntegratorType::RK4;
  } else if (integratory_name == "euler") {
    integrator_type = IntegratorType::EULER;
  } else {
    throw std::runtime_error("Unknown integrator type: " + integratory_name);
  }

  auto modeling_config = std::make_shared<ModelingConfig>(
    ModelingConfig{
          declare_bool("modeling.use_frenet"),
          integrator_type,
          declare_double("modeling.sample_throttle"),
        }
  );

  return std::make_shared<BaseVehicleModelConfig>(
    BaseVehicleModelConfig{
          front_tyre_config,
          rear_tyre_config,
          front_brake_config,
          rear_brake_config,
          steer_config,
          chassis_config,
          aero_config,
          powertrain_config,
          modeling_config
        }
  );
}
}  // namespace base_vehicle_model
}  // namespace vehicle_model
}  // namespace lmpc
