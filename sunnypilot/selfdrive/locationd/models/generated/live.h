#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_8708802279090543733);
void live_err_fun(double *nom_x, double *delta_x, double *out_7046499695008007335);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_8082825287356561563);
void live_H_mod_fun(double *state, double *out_2599535844452018864);
void live_f_fun(double *state, double dt, double *out_8001330115430582838);
void live_F_fun(double *state, double dt, double *out_7857288662862943921);
void live_h_4(double *state, double *unused, double *out_6242355224180129088);
void live_H_4(double *state, double *unused, double *out_8084367645049918016);
void live_h_9(double *state, double *unused, double *out_8165897520044603453);
void live_H_9(double *state, double *unused, double *out_797148709785470546);
void live_h_10(double *state, double *unused, double *out_8403486933126307940);
void live_H_10(double *state, double *unused, double *out_1273190197375986465);
void live_h_12(double *state, double *unused, double *out_7665010112949499972);
void live_H_12(double *state, double *unused, double *out_3981118051616900604);
void live_h_35(double *state, double *unused, double *out_4945996794835031644);
void live_H_35(double *state, double *unused, double *out_4717705587677310640);
void live_h_32(double *state, double *unused, double *out_6241817077783397003);
void live_H_32(double *state, double *unused, double *out_8936404825349219710);
void live_h_13(double *state, double *unused, double *out_4242903670006425170);
void live_H_13(double *state, double *unused, double *out_7675443502002152439);
void live_h_14(double *state, double *unused, double *out_8165897520044603453);
void live_H_14(double *state, double *unused, double *out_797148709785470546);
void live_h_33(double *state, double *unused, double *out_838738127361146128);
void live_H_33(double *state, double *unused, double *out_5478880705596403789);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}