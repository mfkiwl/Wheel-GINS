/*
 * OB_GINS: An Optimization-Based GNSS/INS Integrated Navigation System
 *
 * Copyright (C) 2022 i2Nav Group, Wuhan University
 *
 *     Author : Hailiang Tang
 *    Contact : thl@whu.edu.cn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <Eigen/Geometry>

class Rotation {
 public:
  static Eigen::Quaterniond matrix2quaternion(const Eigen::Matrix3d &matrix) {
    return Eigen::Quaterniond(matrix);
  }

  static Eigen::Matrix3d quaternion2matrix(
      const Eigen::Quaterniond &quaternion) {
    return quaternion.toRotationMatrix();
  }

  static Eigen::Vector3d matrix2euler(const Eigen::Matrix3d &dcm) {
    Eigen::Vector3d euler;

    euler[1] =
        atan(-dcm(2, 0) / sqrt(dcm(2, 1) * dcm(2, 1) + dcm(2, 2) * dcm(2, 2)));

    if (dcm(2, 0) <= -0.999) {
      euler[0] = atan2(dcm(2, 1), dcm(2, 2));
      euler[2] = atan2((dcm(1, 2) - dcm(0, 1)), (dcm(0, 2) + dcm(1, 1)));
    } else if (dcm(2, 0) >= 0.999) {
      euler[0] = atan2(dcm(2, 1), dcm(2, 2));
      euler[2] = M_PI + atan2((dcm(1, 2) + dcm(0, 1)), (dcm(0, 2) - dcm(1, 1)));
    } else {
      euler[0] = atan2(dcm(2, 1), dcm(2, 2));
      euler[2] = atan2(dcm(1, 0), dcm(0, 0));
    }

    // heading 0~2PI
    if (euler[2] < 0) {
      euler[2] = M_PI * 2 + euler[2];
    }

    return euler;
  }

  static Eigen::Vector3d quaternion2euler(
      const Eigen::Quaterniond &quaternion) {
    return matrix2euler(quaternion.toRotationMatrix());
  }

  static Eigen::Quaterniond rotvec2quaternion(const Eigen::Vector3d &rotvec) {
    double angle = rotvec.norm();
    Eigen::Vector3d vec = rotvec.normalized();
    return Eigen::Quaterniond(Eigen::AngleAxisd(angle, vec));
  }

  static Eigen::Vector3d quaternion2vector(
      const Eigen::Quaterniond &quaternion) {
    Eigen::AngleAxisd axisd(quaternion);
    return axisd.angle() * axisd.axis();
  }

  // RPY --> C_b^n, ZYX
  static Eigen::Matrix3d euler2matrix(const Eigen::Vector3d &euler) {
    return Eigen::Matrix3d(
        Eigen::AngleAxisd(euler[2], Eigen::Vector3d::UnitZ()) *
        Eigen::AngleAxisd(euler[1], Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(euler[0], Eigen::Vector3d::UnitX()));
  }

  static Eigen::Quaterniond euler2quaternion(const Eigen::Vector3d &euler) {
    return Eigen::Quaterniond(
        Eigen::AngleAxisd(euler[2], Eigen::Vector3d::UnitZ()) *
        Eigen::AngleAxisd(euler[1], Eigen::Vector3d::UnitY()) *
        Eigen::AngleAxisd(euler[0], Eigen::Vector3d::UnitX()));
  }

  static Eigen::Matrix3d skewSymmetric(const Eigen::Vector3d &vector) {
    Eigen::Matrix3d mat;
    mat << 0, -vector(2), vector(1), vector(2), 0, -vector(0), -vector(1),
        vector(0), 0;
    return mat;
  }

  static Eigen::Matrix4d quaternionleft(const Eigen::Quaterniond &q) {
    Eigen::Matrix4d ans;
    ans(0, 0) = q.w();
    ans.block<1, 3>(0, 1) = -q.vec().transpose();
    ans.block<3, 1>(1, 0) = q.vec();
    ans.block<3, 3>(1, 1) =
        q.w() * Eigen::Matrix3d::Identity() + skewSymmetric(q.vec());
    return ans;
  }

  static Eigen::Matrix4d quaternionright(const Eigen::Quaterniond &p) {
    Eigen::Matrix4d ans;
    ans(0, 0) = p.w();
    ans.block<1, 3>(0, 1) = -p.vec().transpose();
    ans.block<3, 1>(1, 0) = p.vec();
    ans.block<3, 3>(1, 1) =
        p.w() * Eigen::Matrix3d::Identity() - skewSymmetric(p.vec());
    return ans;
  }

  static double heading(double &heading) {
    if (heading < -M_PI) {
      heading += 2 * M_PI;
    } else if (heading > M_PI) {
      heading -= 2 * M_PI;
    }
    return heading;
  }
};