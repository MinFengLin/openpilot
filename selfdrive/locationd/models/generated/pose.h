#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_1520136949545670133);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_5028598293345485084);
void pose_H_mod_fun(double *state, double *out_1187816111513252734);
void pose_f_fun(double *state, double dt, double *out_7179860701368214742);
void pose_F_fun(double *state, double dt, double *out_6410555765576752725);
void pose_h_4(double *state, double *unused, double *out_2896490222522275789);
void pose_H_4(double *state, double *unused, double *out_1964100429216498701);
void pose_h_10(double *state, double *unused, double *out_4680354251909638720);
void pose_H_10(double *state, double *unused, double *out_7907331316519793486);
void pose_h_13(double *state, double *unused, double *out_2876827965017568376);
void pose_H_13(double *state, double *unused, double *out_1248173396115834100);
void pose_h_14(double *state, double *unused, double *out_1032771649440202970);
void pose_H_14(double *state, double *unused, double *out_5046888861511870997);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}