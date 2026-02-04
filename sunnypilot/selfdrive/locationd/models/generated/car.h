#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_5369200598056540630);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6956575797872492127);
void car_H_mod_fun(double *state, double *out_2387770901615532740);
void car_f_fun(double *state, double dt, double *out_8353925704974781694);
void car_F_fun(double *state, double dt, double *out_7402920769861693200);
void car_h_25(double *state, double *unused, double *out_8979882188217307534);
void car_H_25(double *state, double *unused, double *out_2515920923569132100);
void car_h_24(double *state, double *unused, double *out_1624630325334041963);
void car_H_24(double *state, double *unused, double *out_4693135347176282073);
void car_h_30(double *state, double *unused, double *out_5342432659963285184);
void car_H_30(double *state, double *unused, double *out_5034253882076380727);
void car_h_26(double *state, double *unused, double *out_2305646608518952499);
void car_H_26(double *state, double *unused, double *out_1225582395304924124);
void car_h_27(double *state, double *unused, double *out_8523352032590581647);
void car_H_27(double *state, double *unused, double *out_7257847953260323944);
void car_h_29(double *state, double *unused, double *out_6285942512864947619);
void car_H_29(double *state, double *unused, double *out_5544485226390772911);
void car_h_28(double *state, double *unused, double *out_9165541895958151159);
void car_H_28(double *state, double *unused, double *out_462086209321242337);
void car_h_31(double *state, double *unused, double *out_3066149161546559161);
void car_H_31(double *state, double *unused, double *out_2546566885446092528);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}