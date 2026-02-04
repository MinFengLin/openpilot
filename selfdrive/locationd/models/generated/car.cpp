#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_3640875552329470390) {
   out_3640875552329470390[0] = delta_x[0] + nom_x[0];
   out_3640875552329470390[1] = delta_x[1] + nom_x[1];
   out_3640875552329470390[2] = delta_x[2] + nom_x[2];
   out_3640875552329470390[3] = delta_x[3] + nom_x[3];
   out_3640875552329470390[4] = delta_x[4] + nom_x[4];
   out_3640875552329470390[5] = delta_x[5] + nom_x[5];
   out_3640875552329470390[6] = delta_x[6] + nom_x[6];
   out_3640875552329470390[7] = delta_x[7] + nom_x[7];
   out_3640875552329470390[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_3870765577239243527) {
   out_3870765577239243527[0] = -nom_x[0] + true_x[0];
   out_3870765577239243527[1] = -nom_x[1] + true_x[1];
   out_3870765577239243527[2] = -nom_x[2] + true_x[2];
   out_3870765577239243527[3] = -nom_x[3] + true_x[3];
   out_3870765577239243527[4] = -nom_x[4] + true_x[4];
   out_3870765577239243527[5] = -nom_x[5] + true_x[5];
   out_3870765577239243527[6] = -nom_x[6] + true_x[6];
   out_3870765577239243527[7] = -nom_x[7] + true_x[7];
   out_3870765577239243527[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_668996414300044099) {
   out_668996414300044099[0] = 1.0;
   out_668996414300044099[1] = 0.0;
   out_668996414300044099[2] = 0.0;
   out_668996414300044099[3] = 0.0;
   out_668996414300044099[4] = 0.0;
   out_668996414300044099[5] = 0.0;
   out_668996414300044099[6] = 0.0;
   out_668996414300044099[7] = 0.0;
   out_668996414300044099[8] = 0.0;
   out_668996414300044099[9] = 0.0;
   out_668996414300044099[10] = 1.0;
   out_668996414300044099[11] = 0.0;
   out_668996414300044099[12] = 0.0;
   out_668996414300044099[13] = 0.0;
   out_668996414300044099[14] = 0.0;
   out_668996414300044099[15] = 0.0;
   out_668996414300044099[16] = 0.0;
   out_668996414300044099[17] = 0.0;
   out_668996414300044099[18] = 0.0;
   out_668996414300044099[19] = 0.0;
   out_668996414300044099[20] = 1.0;
   out_668996414300044099[21] = 0.0;
   out_668996414300044099[22] = 0.0;
   out_668996414300044099[23] = 0.0;
   out_668996414300044099[24] = 0.0;
   out_668996414300044099[25] = 0.0;
   out_668996414300044099[26] = 0.0;
   out_668996414300044099[27] = 0.0;
   out_668996414300044099[28] = 0.0;
   out_668996414300044099[29] = 0.0;
   out_668996414300044099[30] = 1.0;
   out_668996414300044099[31] = 0.0;
   out_668996414300044099[32] = 0.0;
   out_668996414300044099[33] = 0.0;
   out_668996414300044099[34] = 0.0;
   out_668996414300044099[35] = 0.0;
   out_668996414300044099[36] = 0.0;
   out_668996414300044099[37] = 0.0;
   out_668996414300044099[38] = 0.0;
   out_668996414300044099[39] = 0.0;
   out_668996414300044099[40] = 1.0;
   out_668996414300044099[41] = 0.0;
   out_668996414300044099[42] = 0.0;
   out_668996414300044099[43] = 0.0;
   out_668996414300044099[44] = 0.0;
   out_668996414300044099[45] = 0.0;
   out_668996414300044099[46] = 0.0;
   out_668996414300044099[47] = 0.0;
   out_668996414300044099[48] = 0.0;
   out_668996414300044099[49] = 0.0;
   out_668996414300044099[50] = 1.0;
   out_668996414300044099[51] = 0.0;
   out_668996414300044099[52] = 0.0;
   out_668996414300044099[53] = 0.0;
   out_668996414300044099[54] = 0.0;
   out_668996414300044099[55] = 0.0;
   out_668996414300044099[56] = 0.0;
   out_668996414300044099[57] = 0.0;
   out_668996414300044099[58] = 0.0;
   out_668996414300044099[59] = 0.0;
   out_668996414300044099[60] = 1.0;
   out_668996414300044099[61] = 0.0;
   out_668996414300044099[62] = 0.0;
   out_668996414300044099[63] = 0.0;
   out_668996414300044099[64] = 0.0;
   out_668996414300044099[65] = 0.0;
   out_668996414300044099[66] = 0.0;
   out_668996414300044099[67] = 0.0;
   out_668996414300044099[68] = 0.0;
   out_668996414300044099[69] = 0.0;
   out_668996414300044099[70] = 1.0;
   out_668996414300044099[71] = 0.0;
   out_668996414300044099[72] = 0.0;
   out_668996414300044099[73] = 0.0;
   out_668996414300044099[74] = 0.0;
   out_668996414300044099[75] = 0.0;
   out_668996414300044099[76] = 0.0;
   out_668996414300044099[77] = 0.0;
   out_668996414300044099[78] = 0.0;
   out_668996414300044099[79] = 0.0;
   out_668996414300044099[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_8299356038284067441) {
   out_8299356038284067441[0] = state[0];
   out_8299356038284067441[1] = state[1];
   out_8299356038284067441[2] = state[2];
   out_8299356038284067441[3] = state[3];
   out_8299356038284067441[4] = state[4];
   out_8299356038284067441[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_8299356038284067441[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_8299356038284067441[7] = state[7];
   out_8299356038284067441[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3194678011425204153) {
   out_3194678011425204153[0] = 1;
   out_3194678011425204153[1] = 0;
   out_3194678011425204153[2] = 0;
   out_3194678011425204153[3] = 0;
   out_3194678011425204153[4] = 0;
   out_3194678011425204153[5] = 0;
   out_3194678011425204153[6] = 0;
   out_3194678011425204153[7] = 0;
   out_3194678011425204153[8] = 0;
   out_3194678011425204153[9] = 0;
   out_3194678011425204153[10] = 1;
   out_3194678011425204153[11] = 0;
   out_3194678011425204153[12] = 0;
   out_3194678011425204153[13] = 0;
   out_3194678011425204153[14] = 0;
   out_3194678011425204153[15] = 0;
   out_3194678011425204153[16] = 0;
   out_3194678011425204153[17] = 0;
   out_3194678011425204153[18] = 0;
   out_3194678011425204153[19] = 0;
   out_3194678011425204153[20] = 1;
   out_3194678011425204153[21] = 0;
   out_3194678011425204153[22] = 0;
   out_3194678011425204153[23] = 0;
   out_3194678011425204153[24] = 0;
   out_3194678011425204153[25] = 0;
   out_3194678011425204153[26] = 0;
   out_3194678011425204153[27] = 0;
   out_3194678011425204153[28] = 0;
   out_3194678011425204153[29] = 0;
   out_3194678011425204153[30] = 1;
   out_3194678011425204153[31] = 0;
   out_3194678011425204153[32] = 0;
   out_3194678011425204153[33] = 0;
   out_3194678011425204153[34] = 0;
   out_3194678011425204153[35] = 0;
   out_3194678011425204153[36] = 0;
   out_3194678011425204153[37] = 0;
   out_3194678011425204153[38] = 0;
   out_3194678011425204153[39] = 0;
   out_3194678011425204153[40] = 1;
   out_3194678011425204153[41] = 0;
   out_3194678011425204153[42] = 0;
   out_3194678011425204153[43] = 0;
   out_3194678011425204153[44] = 0;
   out_3194678011425204153[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3194678011425204153[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3194678011425204153[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3194678011425204153[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3194678011425204153[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3194678011425204153[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3194678011425204153[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3194678011425204153[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3194678011425204153[53] = -9.8100000000000005*dt;
   out_3194678011425204153[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3194678011425204153[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3194678011425204153[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3194678011425204153[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3194678011425204153[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3194678011425204153[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3194678011425204153[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3194678011425204153[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3194678011425204153[62] = 0;
   out_3194678011425204153[63] = 0;
   out_3194678011425204153[64] = 0;
   out_3194678011425204153[65] = 0;
   out_3194678011425204153[66] = 0;
   out_3194678011425204153[67] = 0;
   out_3194678011425204153[68] = 0;
   out_3194678011425204153[69] = 0;
   out_3194678011425204153[70] = 1;
   out_3194678011425204153[71] = 0;
   out_3194678011425204153[72] = 0;
   out_3194678011425204153[73] = 0;
   out_3194678011425204153[74] = 0;
   out_3194678011425204153[75] = 0;
   out_3194678011425204153[76] = 0;
   out_3194678011425204153[77] = 0;
   out_3194678011425204153[78] = 0;
   out_3194678011425204153[79] = 0;
   out_3194678011425204153[80] = 1;
}
void h_25(double *state, double *unused, double *out_3284272869868547751) {
   out_3284272869868547751[0] = state[6];
}
void H_25(double *state, double *unused, double *out_7252341618673815706) {
   out_7252341618673815706[0] = 0;
   out_7252341618673815706[1] = 0;
   out_7252341618673815706[2] = 0;
   out_7252341618673815706[3] = 0;
   out_7252341618673815706[4] = 0;
   out_7252341618673815706[5] = 0;
   out_7252341618673815706[6] = 1;
   out_7252341618673815706[7] = 0;
   out_7252341618673815706[8] = 0;
}
void h_24(double *state, double *unused, double *out_4907226623136626829) {
   out_4907226623136626829[0] = state[4];
   out_4907226623136626829[1] = state[5];
}
void H_24(double *state, double *unused, double *out_5079692019668316140) {
   out_5079692019668316140[0] = 0;
   out_5079692019668316140[1] = 0;
   out_5079692019668316140[2] = 0;
   out_5079692019668316140[3] = 0;
   out_5079692019668316140[4] = 1;
   out_5079692019668316140[5] = 0;
   out_5079692019668316140[6] = 0;
   out_5079692019668316140[7] = 0;
   out_5079692019668316140[8] = 0;
   out_5079692019668316140[9] = 0;
   out_5079692019668316140[10] = 0;
   out_5079692019668316140[11] = 0;
   out_5079692019668316140[12] = 0;
   out_5079692019668316140[13] = 0;
   out_5079692019668316140[14] = 1;
   out_5079692019668316140[15] = 0;
   out_5079692019668316140[16] = 0;
   out_5079692019668316140[17] = 0;
}
void h_30(double *state, double *unused, double *out_6921722398122570101) {
   out_6921722398122570101[0] = state[4];
}
void H_30(double *state, double *unused, double *out_8676069496528487283) {
   out_8676069496528487283[0] = 0;
   out_8676069496528487283[1] = 0;
   out_8676069496528487283[2] = 0;
   out_8676069496528487283[3] = 0;
   out_8676069496528487283[4] = 1;
   out_8676069496528487283[5] = 0;
   out_8676069496528487283[6] = 0;
   out_8676069496528487283[7] = 0;
   out_8676069496528487283[8] = 0;
}
void h_26(double *state, double *unused, double *out_8089466199198711838) {
   out_8089466199198711838[0] = state[7];
}
void H_26(double *state, double *unused, double *out_3510838299799759482) {
   out_3510838299799759482[0] = 0;
   out_3510838299799759482[1] = 0;
   out_3510838299799759482[2] = 0;
   out_3510838299799759482[3] = 0;
   out_3510838299799759482[4] = 0;
   out_3510838299799759482[5] = 0;
   out_3510838299799759482[6] = 0;
   out_3510838299799759482[7] = 1;
   out_3510838299799759482[8] = 0;
}
void h_27(double *state, double *unused, double *out_6444634283972311539) {
   out_6444634283972311539[0] = state[3];
}
void H_27(double *state, double *unused, double *out_7595911265380639422) {
   out_7595911265380639422[0] = 0;
   out_7595911265380639422[1] = 0;
   out_7595911265380639422[2] = 0;
   out_7595911265380639422[3] = 1;
   out_7595911265380639422[4] = 0;
   out_7595911265380639422[5] = 0;
   out_7595911265380639422[6] = 0;
   out_7595911265380639422[7] = 0;
   out_7595911265380639422[8] = 0;
}
void h_29(double *state, double *unused, double *out_4541551025488351752) {
   out_4541551025488351752[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8165838152214095099) {
   out_8165838152214095099[0] = 0;
   out_8165838152214095099[1] = 1;
   out_8165838152214095099[2] = 0;
   out_8165838152214095099[3] = 0;
   out_8165838152214095099[4] = 0;
   out_8165838152214095099[5] = 0;
   out_8165838152214095099[6] = 0;
   out_8165838152214095099[7] = 0;
   out_8165838152214095099[8] = 0;
}
void h_28(double *state, double *unused, double *out_1442961016311465894) {
   out_1442961016311465894[0] = state[0];
}
void H_28(double *state, double *unused, double *out_5198506904425925943) {
   out_5198506904425925943[0] = 1;
   out_5198506904425925943[1] = 0;
   out_5198506904425925943[2] = 0;
   out_5198506904425925943[3] = 0;
   out_5198506904425925943[4] = 0;
   out_5198506904425925943[5] = 0;
   out_5198506904425925943[6] = 0;
   out_5198506904425925943[7] = 0;
   out_5198506904425925943[8] = 0;
}
void h_31(double *state, double *unused, double *out_3683042294812484743) {
   out_3683042294812484743[0] = state[8];
}
void H_31(double *state, double *unused, double *out_2884630197566408006) {
   out_2884630197566408006[0] = 0;
   out_2884630197566408006[1] = 0;
   out_2884630197566408006[2] = 0;
   out_2884630197566408006[3] = 0;
   out_2884630197566408006[4] = 0;
   out_2884630197566408006[5] = 0;
   out_2884630197566408006[6] = 0;
   out_2884630197566408006[7] = 0;
   out_2884630197566408006[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_3640875552329470390) {
  err_fun(nom_x, delta_x, out_3640875552329470390);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_3870765577239243527) {
  inv_err_fun(nom_x, true_x, out_3870765577239243527);
}
void car_H_mod_fun(double *state, double *out_668996414300044099) {
  H_mod_fun(state, out_668996414300044099);
}
void car_f_fun(double *state, double dt, double *out_8299356038284067441) {
  f_fun(state,  dt, out_8299356038284067441);
}
void car_F_fun(double *state, double dt, double *out_3194678011425204153) {
  F_fun(state,  dt, out_3194678011425204153);
}
void car_h_25(double *state, double *unused, double *out_3284272869868547751) {
  h_25(state, unused, out_3284272869868547751);
}
void car_H_25(double *state, double *unused, double *out_7252341618673815706) {
  H_25(state, unused, out_7252341618673815706);
}
void car_h_24(double *state, double *unused, double *out_4907226623136626829) {
  h_24(state, unused, out_4907226623136626829);
}
void car_H_24(double *state, double *unused, double *out_5079692019668316140) {
  H_24(state, unused, out_5079692019668316140);
}
void car_h_30(double *state, double *unused, double *out_6921722398122570101) {
  h_30(state, unused, out_6921722398122570101);
}
void car_H_30(double *state, double *unused, double *out_8676069496528487283) {
  H_30(state, unused, out_8676069496528487283);
}
void car_h_26(double *state, double *unused, double *out_8089466199198711838) {
  h_26(state, unused, out_8089466199198711838);
}
void car_H_26(double *state, double *unused, double *out_3510838299799759482) {
  H_26(state, unused, out_3510838299799759482);
}
void car_h_27(double *state, double *unused, double *out_6444634283972311539) {
  h_27(state, unused, out_6444634283972311539);
}
void car_H_27(double *state, double *unused, double *out_7595911265380639422) {
  H_27(state, unused, out_7595911265380639422);
}
void car_h_29(double *state, double *unused, double *out_4541551025488351752) {
  h_29(state, unused, out_4541551025488351752);
}
void car_H_29(double *state, double *unused, double *out_8165838152214095099) {
  H_29(state, unused, out_8165838152214095099);
}
void car_h_28(double *state, double *unused, double *out_1442961016311465894) {
  h_28(state, unused, out_1442961016311465894);
}
void car_H_28(double *state, double *unused, double *out_5198506904425925943) {
  H_28(state, unused, out_5198506904425925943);
}
void car_h_31(double *state, double *unused, double *out_3683042294812484743) {
  h_31(state, unused, out_3683042294812484743);
}
void car_H_31(double *state, double *unused, double *out_2884630197566408006) {
  H_31(state, unused, out_2884630197566408006);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
