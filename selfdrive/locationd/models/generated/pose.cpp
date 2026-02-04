#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_1520136949545670133) {
   out_1520136949545670133[0] = delta_x[0] + nom_x[0];
   out_1520136949545670133[1] = delta_x[1] + nom_x[1];
   out_1520136949545670133[2] = delta_x[2] + nom_x[2];
   out_1520136949545670133[3] = delta_x[3] + nom_x[3];
   out_1520136949545670133[4] = delta_x[4] + nom_x[4];
   out_1520136949545670133[5] = delta_x[5] + nom_x[5];
   out_1520136949545670133[6] = delta_x[6] + nom_x[6];
   out_1520136949545670133[7] = delta_x[7] + nom_x[7];
   out_1520136949545670133[8] = delta_x[8] + nom_x[8];
   out_1520136949545670133[9] = delta_x[9] + nom_x[9];
   out_1520136949545670133[10] = delta_x[10] + nom_x[10];
   out_1520136949545670133[11] = delta_x[11] + nom_x[11];
   out_1520136949545670133[12] = delta_x[12] + nom_x[12];
   out_1520136949545670133[13] = delta_x[13] + nom_x[13];
   out_1520136949545670133[14] = delta_x[14] + nom_x[14];
   out_1520136949545670133[15] = delta_x[15] + nom_x[15];
   out_1520136949545670133[16] = delta_x[16] + nom_x[16];
   out_1520136949545670133[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5028598293345485084) {
   out_5028598293345485084[0] = -nom_x[0] + true_x[0];
   out_5028598293345485084[1] = -nom_x[1] + true_x[1];
   out_5028598293345485084[2] = -nom_x[2] + true_x[2];
   out_5028598293345485084[3] = -nom_x[3] + true_x[3];
   out_5028598293345485084[4] = -nom_x[4] + true_x[4];
   out_5028598293345485084[5] = -nom_x[5] + true_x[5];
   out_5028598293345485084[6] = -nom_x[6] + true_x[6];
   out_5028598293345485084[7] = -nom_x[7] + true_x[7];
   out_5028598293345485084[8] = -nom_x[8] + true_x[8];
   out_5028598293345485084[9] = -nom_x[9] + true_x[9];
   out_5028598293345485084[10] = -nom_x[10] + true_x[10];
   out_5028598293345485084[11] = -nom_x[11] + true_x[11];
   out_5028598293345485084[12] = -nom_x[12] + true_x[12];
   out_5028598293345485084[13] = -nom_x[13] + true_x[13];
   out_5028598293345485084[14] = -nom_x[14] + true_x[14];
   out_5028598293345485084[15] = -nom_x[15] + true_x[15];
   out_5028598293345485084[16] = -nom_x[16] + true_x[16];
   out_5028598293345485084[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_1187816111513252734) {
   out_1187816111513252734[0] = 1.0;
   out_1187816111513252734[1] = 0.0;
   out_1187816111513252734[2] = 0.0;
   out_1187816111513252734[3] = 0.0;
   out_1187816111513252734[4] = 0.0;
   out_1187816111513252734[5] = 0.0;
   out_1187816111513252734[6] = 0.0;
   out_1187816111513252734[7] = 0.0;
   out_1187816111513252734[8] = 0.0;
   out_1187816111513252734[9] = 0.0;
   out_1187816111513252734[10] = 0.0;
   out_1187816111513252734[11] = 0.0;
   out_1187816111513252734[12] = 0.0;
   out_1187816111513252734[13] = 0.0;
   out_1187816111513252734[14] = 0.0;
   out_1187816111513252734[15] = 0.0;
   out_1187816111513252734[16] = 0.0;
   out_1187816111513252734[17] = 0.0;
   out_1187816111513252734[18] = 0.0;
   out_1187816111513252734[19] = 1.0;
   out_1187816111513252734[20] = 0.0;
   out_1187816111513252734[21] = 0.0;
   out_1187816111513252734[22] = 0.0;
   out_1187816111513252734[23] = 0.0;
   out_1187816111513252734[24] = 0.0;
   out_1187816111513252734[25] = 0.0;
   out_1187816111513252734[26] = 0.0;
   out_1187816111513252734[27] = 0.0;
   out_1187816111513252734[28] = 0.0;
   out_1187816111513252734[29] = 0.0;
   out_1187816111513252734[30] = 0.0;
   out_1187816111513252734[31] = 0.0;
   out_1187816111513252734[32] = 0.0;
   out_1187816111513252734[33] = 0.0;
   out_1187816111513252734[34] = 0.0;
   out_1187816111513252734[35] = 0.0;
   out_1187816111513252734[36] = 0.0;
   out_1187816111513252734[37] = 0.0;
   out_1187816111513252734[38] = 1.0;
   out_1187816111513252734[39] = 0.0;
   out_1187816111513252734[40] = 0.0;
   out_1187816111513252734[41] = 0.0;
   out_1187816111513252734[42] = 0.0;
   out_1187816111513252734[43] = 0.0;
   out_1187816111513252734[44] = 0.0;
   out_1187816111513252734[45] = 0.0;
   out_1187816111513252734[46] = 0.0;
   out_1187816111513252734[47] = 0.0;
   out_1187816111513252734[48] = 0.0;
   out_1187816111513252734[49] = 0.0;
   out_1187816111513252734[50] = 0.0;
   out_1187816111513252734[51] = 0.0;
   out_1187816111513252734[52] = 0.0;
   out_1187816111513252734[53] = 0.0;
   out_1187816111513252734[54] = 0.0;
   out_1187816111513252734[55] = 0.0;
   out_1187816111513252734[56] = 0.0;
   out_1187816111513252734[57] = 1.0;
   out_1187816111513252734[58] = 0.0;
   out_1187816111513252734[59] = 0.0;
   out_1187816111513252734[60] = 0.0;
   out_1187816111513252734[61] = 0.0;
   out_1187816111513252734[62] = 0.0;
   out_1187816111513252734[63] = 0.0;
   out_1187816111513252734[64] = 0.0;
   out_1187816111513252734[65] = 0.0;
   out_1187816111513252734[66] = 0.0;
   out_1187816111513252734[67] = 0.0;
   out_1187816111513252734[68] = 0.0;
   out_1187816111513252734[69] = 0.0;
   out_1187816111513252734[70] = 0.0;
   out_1187816111513252734[71] = 0.0;
   out_1187816111513252734[72] = 0.0;
   out_1187816111513252734[73] = 0.0;
   out_1187816111513252734[74] = 0.0;
   out_1187816111513252734[75] = 0.0;
   out_1187816111513252734[76] = 1.0;
   out_1187816111513252734[77] = 0.0;
   out_1187816111513252734[78] = 0.0;
   out_1187816111513252734[79] = 0.0;
   out_1187816111513252734[80] = 0.0;
   out_1187816111513252734[81] = 0.0;
   out_1187816111513252734[82] = 0.0;
   out_1187816111513252734[83] = 0.0;
   out_1187816111513252734[84] = 0.0;
   out_1187816111513252734[85] = 0.0;
   out_1187816111513252734[86] = 0.0;
   out_1187816111513252734[87] = 0.0;
   out_1187816111513252734[88] = 0.0;
   out_1187816111513252734[89] = 0.0;
   out_1187816111513252734[90] = 0.0;
   out_1187816111513252734[91] = 0.0;
   out_1187816111513252734[92] = 0.0;
   out_1187816111513252734[93] = 0.0;
   out_1187816111513252734[94] = 0.0;
   out_1187816111513252734[95] = 1.0;
   out_1187816111513252734[96] = 0.0;
   out_1187816111513252734[97] = 0.0;
   out_1187816111513252734[98] = 0.0;
   out_1187816111513252734[99] = 0.0;
   out_1187816111513252734[100] = 0.0;
   out_1187816111513252734[101] = 0.0;
   out_1187816111513252734[102] = 0.0;
   out_1187816111513252734[103] = 0.0;
   out_1187816111513252734[104] = 0.0;
   out_1187816111513252734[105] = 0.0;
   out_1187816111513252734[106] = 0.0;
   out_1187816111513252734[107] = 0.0;
   out_1187816111513252734[108] = 0.0;
   out_1187816111513252734[109] = 0.0;
   out_1187816111513252734[110] = 0.0;
   out_1187816111513252734[111] = 0.0;
   out_1187816111513252734[112] = 0.0;
   out_1187816111513252734[113] = 0.0;
   out_1187816111513252734[114] = 1.0;
   out_1187816111513252734[115] = 0.0;
   out_1187816111513252734[116] = 0.0;
   out_1187816111513252734[117] = 0.0;
   out_1187816111513252734[118] = 0.0;
   out_1187816111513252734[119] = 0.0;
   out_1187816111513252734[120] = 0.0;
   out_1187816111513252734[121] = 0.0;
   out_1187816111513252734[122] = 0.0;
   out_1187816111513252734[123] = 0.0;
   out_1187816111513252734[124] = 0.0;
   out_1187816111513252734[125] = 0.0;
   out_1187816111513252734[126] = 0.0;
   out_1187816111513252734[127] = 0.0;
   out_1187816111513252734[128] = 0.0;
   out_1187816111513252734[129] = 0.0;
   out_1187816111513252734[130] = 0.0;
   out_1187816111513252734[131] = 0.0;
   out_1187816111513252734[132] = 0.0;
   out_1187816111513252734[133] = 1.0;
   out_1187816111513252734[134] = 0.0;
   out_1187816111513252734[135] = 0.0;
   out_1187816111513252734[136] = 0.0;
   out_1187816111513252734[137] = 0.0;
   out_1187816111513252734[138] = 0.0;
   out_1187816111513252734[139] = 0.0;
   out_1187816111513252734[140] = 0.0;
   out_1187816111513252734[141] = 0.0;
   out_1187816111513252734[142] = 0.0;
   out_1187816111513252734[143] = 0.0;
   out_1187816111513252734[144] = 0.0;
   out_1187816111513252734[145] = 0.0;
   out_1187816111513252734[146] = 0.0;
   out_1187816111513252734[147] = 0.0;
   out_1187816111513252734[148] = 0.0;
   out_1187816111513252734[149] = 0.0;
   out_1187816111513252734[150] = 0.0;
   out_1187816111513252734[151] = 0.0;
   out_1187816111513252734[152] = 1.0;
   out_1187816111513252734[153] = 0.0;
   out_1187816111513252734[154] = 0.0;
   out_1187816111513252734[155] = 0.0;
   out_1187816111513252734[156] = 0.0;
   out_1187816111513252734[157] = 0.0;
   out_1187816111513252734[158] = 0.0;
   out_1187816111513252734[159] = 0.0;
   out_1187816111513252734[160] = 0.0;
   out_1187816111513252734[161] = 0.0;
   out_1187816111513252734[162] = 0.0;
   out_1187816111513252734[163] = 0.0;
   out_1187816111513252734[164] = 0.0;
   out_1187816111513252734[165] = 0.0;
   out_1187816111513252734[166] = 0.0;
   out_1187816111513252734[167] = 0.0;
   out_1187816111513252734[168] = 0.0;
   out_1187816111513252734[169] = 0.0;
   out_1187816111513252734[170] = 0.0;
   out_1187816111513252734[171] = 1.0;
   out_1187816111513252734[172] = 0.0;
   out_1187816111513252734[173] = 0.0;
   out_1187816111513252734[174] = 0.0;
   out_1187816111513252734[175] = 0.0;
   out_1187816111513252734[176] = 0.0;
   out_1187816111513252734[177] = 0.0;
   out_1187816111513252734[178] = 0.0;
   out_1187816111513252734[179] = 0.0;
   out_1187816111513252734[180] = 0.0;
   out_1187816111513252734[181] = 0.0;
   out_1187816111513252734[182] = 0.0;
   out_1187816111513252734[183] = 0.0;
   out_1187816111513252734[184] = 0.0;
   out_1187816111513252734[185] = 0.0;
   out_1187816111513252734[186] = 0.0;
   out_1187816111513252734[187] = 0.0;
   out_1187816111513252734[188] = 0.0;
   out_1187816111513252734[189] = 0.0;
   out_1187816111513252734[190] = 1.0;
   out_1187816111513252734[191] = 0.0;
   out_1187816111513252734[192] = 0.0;
   out_1187816111513252734[193] = 0.0;
   out_1187816111513252734[194] = 0.0;
   out_1187816111513252734[195] = 0.0;
   out_1187816111513252734[196] = 0.0;
   out_1187816111513252734[197] = 0.0;
   out_1187816111513252734[198] = 0.0;
   out_1187816111513252734[199] = 0.0;
   out_1187816111513252734[200] = 0.0;
   out_1187816111513252734[201] = 0.0;
   out_1187816111513252734[202] = 0.0;
   out_1187816111513252734[203] = 0.0;
   out_1187816111513252734[204] = 0.0;
   out_1187816111513252734[205] = 0.0;
   out_1187816111513252734[206] = 0.0;
   out_1187816111513252734[207] = 0.0;
   out_1187816111513252734[208] = 0.0;
   out_1187816111513252734[209] = 1.0;
   out_1187816111513252734[210] = 0.0;
   out_1187816111513252734[211] = 0.0;
   out_1187816111513252734[212] = 0.0;
   out_1187816111513252734[213] = 0.0;
   out_1187816111513252734[214] = 0.0;
   out_1187816111513252734[215] = 0.0;
   out_1187816111513252734[216] = 0.0;
   out_1187816111513252734[217] = 0.0;
   out_1187816111513252734[218] = 0.0;
   out_1187816111513252734[219] = 0.0;
   out_1187816111513252734[220] = 0.0;
   out_1187816111513252734[221] = 0.0;
   out_1187816111513252734[222] = 0.0;
   out_1187816111513252734[223] = 0.0;
   out_1187816111513252734[224] = 0.0;
   out_1187816111513252734[225] = 0.0;
   out_1187816111513252734[226] = 0.0;
   out_1187816111513252734[227] = 0.0;
   out_1187816111513252734[228] = 1.0;
   out_1187816111513252734[229] = 0.0;
   out_1187816111513252734[230] = 0.0;
   out_1187816111513252734[231] = 0.0;
   out_1187816111513252734[232] = 0.0;
   out_1187816111513252734[233] = 0.0;
   out_1187816111513252734[234] = 0.0;
   out_1187816111513252734[235] = 0.0;
   out_1187816111513252734[236] = 0.0;
   out_1187816111513252734[237] = 0.0;
   out_1187816111513252734[238] = 0.0;
   out_1187816111513252734[239] = 0.0;
   out_1187816111513252734[240] = 0.0;
   out_1187816111513252734[241] = 0.0;
   out_1187816111513252734[242] = 0.0;
   out_1187816111513252734[243] = 0.0;
   out_1187816111513252734[244] = 0.0;
   out_1187816111513252734[245] = 0.0;
   out_1187816111513252734[246] = 0.0;
   out_1187816111513252734[247] = 1.0;
   out_1187816111513252734[248] = 0.0;
   out_1187816111513252734[249] = 0.0;
   out_1187816111513252734[250] = 0.0;
   out_1187816111513252734[251] = 0.0;
   out_1187816111513252734[252] = 0.0;
   out_1187816111513252734[253] = 0.0;
   out_1187816111513252734[254] = 0.0;
   out_1187816111513252734[255] = 0.0;
   out_1187816111513252734[256] = 0.0;
   out_1187816111513252734[257] = 0.0;
   out_1187816111513252734[258] = 0.0;
   out_1187816111513252734[259] = 0.0;
   out_1187816111513252734[260] = 0.0;
   out_1187816111513252734[261] = 0.0;
   out_1187816111513252734[262] = 0.0;
   out_1187816111513252734[263] = 0.0;
   out_1187816111513252734[264] = 0.0;
   out_1187816111513252734[265] = 0.0;
   out_1187816111513252734[266] = 1.0;
   out_1187816111513252734[267] = 0.0;
   out_1187816111513252734[268] = 0.0;
   out_1187816111513252734[269] = 0.0;
   out_1187816111513252734[270] = 0.0;
   out_1187816111513252734[271] = 0.0;
   out_1187816111513252734[272] = 0.0;
   out_1187816111513252734[273] = 0.0;
   out_1187816111513252734[274] = 0.0;
   out_1187816111513252734[275] = 0.0;
   out_1187816111513252734[276] = 0.0;
   out_1187816111513252734[277] = 0.0;
   out_1187816111513252734[278] = 0.0;
   out_1187816111513252734[279] = 0.0;
   out_1187816111513252734[280] = 0.0;
   out_1187816111513252734[281] = 0.0;
   out_1187816111513252734[282] = 0.0;
   out_1187816111513252734[283] = 0.0;
   out_1187816111513252734[284] = 0.0;
   out_1187816111513252734[285] = 1.0;
   out_1187816111513252734[286] = 0.0;
   out_1187816111513252734[287] = 0.0;
   out_1187816111513252734[288] = 0.0;
   out_1187816111513252734[289] = 0.0;
   out_1187816111513252734[290] = 0.0;
   out_1187816111513252734[291] = 0.0;
   out_1187816111513252734[292] = 0.0;
   out_1187816111513252734[293] = 0.0;
   out_1187816111513252734[294] = 0.0;
   out_1187816111513252734[295] = 0.0;
   out_1187816111513252734[296] = 0.0;
   out_1187816111513252734[297] = 0.0;
   out_1187816111513252734[298] = 0.0;
   out_1187816111513252734[299] = 0.0;
   out_1187816111513252734[300] = 0.0;
   out_1187816111513252734[301] = 0.0;
   out_1187816111513252734[302] = 0.0;
   out_1187816111513252734[303] = 0.0;
   out_1187816111513252734[304] = 1.0;
   out_1187816111513252734[305] = 0.0;
   out_1187816111513252734[306] = 0.0;
   out_1187816111513252734[307] = 0.0;
   out_1187816111513252734[308] = 0.0;
   out_1187816111513252734[309] = 0.0;
   out_1187816111513252734[310] = 0.0;
   out_1187816111513252734[311] = 0.0;
   out_1187816111513252734[312] = 0.0;
   out_1187816111513252734[313] = 0.0;
   out_1187816111513252734[314] = 0.0;
   out_1187816111513252734[315] = 0.0;
   out_1187816111513252734[316] = 0.0;
   out_1187816111513252734[317] = 0.0;
   out_1187816111513252734[318] = 0.0;
   out_1187816111513252734[319] = 0.0;
   out_1187816111513252734[320] = 0.0;
   out_1187816111513252734[321] = 0.0;
   out_1187816111513252734[322] = 0.0;
   out_1187816111513252734[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_7179860701368214742) {
   out_7179860701368214742[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_7179860701368214742[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_7179860701368214742[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_7179860701368214742[3] = dt*state[12] + state[3];
   out_7179860701368214742[4] = dt*state[13] + state[4];
   out_7179860701368214742[5] = dt*state[14] + state[5];
   out_7179860701368214742[6] = state[6];
   out_7179860701368214742[7] = state[7];
   out_7179860701368214742[8] = state[8];
   out_7179860701368214742[9] = state[9];
   out_7179860701368214742[10] = state[10];
   out_7179860701368214742[11] = state[11];
   out_7179860701368214742[12] = state[12];
   out_7179860701368214742[13] = state[13];
   out_7179860701368214742[14] = state[14];
   out_7179860701368214742[15] = state[15];
   out_7179860701368214742[16] = state[16];
   out_7179860701368214742[17] = state[17];
}
void F_fun(double *state, double dt, double *out_6410555765576752725) {
   out_6410555765576752725[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6410555765576752725[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6410555765576752725[2] = 0;
   out_6410555765576752725[3] = 0;
   out_6410555765576752725[4] = 0;
   out_6410555765576752725[5] = 0;
   out_6410555765576752725[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6410555765576752725[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6410555765576752725[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6410555765576752725[9] = 0;
   out_6410555765576752725[10] = 0;
   out_6410555765576752725[11] = 0;
   out_6410555765576752725[12] = 0;
   out_6410555765576752725[13] = 0;
   out_6410555765576752725[14] = 0;
   out_6410555765576752725[15] = 0;
   out_6410555765576752725[16] = 0;
   out_6410555765576752725[17] = 0;
   out_6410555765576752725[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6410555765576752725[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6410555765576752725[20] = 0;
   out_6410555765576752725[21] = 0;
   out_6410555765576752725[22] = 0;
   out_6410555765576752725[23] = 0;
   out_6410555765576752725[24] = 0;
   out_6410555765576752725[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6410555765576752725[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6410555765576752725[27] = 0;
   out_6410555765576752725[28] = 0;
   out_6410555765576752725[29] = 0;
   out_6410555765576752725[30] = 0;
   out_6410555765576752725[31] = 0;
   out_6410555765576752725[32] = 0;
   out_6410555765576752725[33] = 0;
   out_6410555765576752725[34] = 0;
   out_6410555765576752725[35] = 0;
   out_6410555765576752725[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6410555765576752725[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6410555765576752725[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6410555765576752725[39] = 0;
   out_6410555765576752725[40] = 0;
   out_6410555765576752725[41] = 0;
   out_6410555765576752725[42] = 0;
   out_6410555765576752725[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6410555765576752725[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6410555765576752725[45] = 0;
   out_6410555765576752725[46] = 0;
   out_6410555765576752725[47] = 0;
   out_6410555765576752725[48] = 0;
   out_6410555765576752725[49] = 0;
   out_6410555765576752725[50] = 0;
   out_6410555765576752725[51] = 0;
   out_6410555765576752725[52] = 0;
   out_6410555765576752725[53] = 0;
   out_6410555765576752725[54] = 0;
   out_6410555765576752725[55] = 0;
   out_6410555765576752725[56] = 0;
   out_6410555765576752725[57] = 1;
   out_6410555765576752725[58] = 0;
   out_6410555765576752725[59] = 0;
   out_6410555765576752725[60] = 0;
   out_6410555765576752725[61] = 0;
   out_6410555765576752725[62] = 0;
   out_6410555765576752725[63] = 0;
   out_6410555765576752725[64] = 0;
   out_6410555765576752725[65] = 0;
   out_6410555765576752725[66] = dt;
   out_6410555765576752725[67] = 0;
   out_6410555765576752725[68] = 0;
   out_6410555765576752725[69] = 0;
   out_6410555765576752725[70] = 0;
   out_6410555765576752725[71] = 0;
   out_6410555765576752725[72] = 0;
   out_6410555765576752725[73] = 0;
   out_6410555765576752725[74] = 0;
   out_6410555765576752725[75] = 0;
   out_6410555765576752725[76] = 1;
   out_6410555765576752725[77] = 0;
   out_6410555765576752725[78] = 0;
   out_6410555765576752725[79] = 0;
   out_6410555765576752725[80] = 0;
   out_6410555765576752725[81] = 0;
   out_6410555765576752725[82] = 0;
   out_6410555765576752725[83] = 0;
   out_6410555765576752725[84] = 0;
   out_6410555765576752725[85] = dt;
   out_6410555765576752725[86] = 0;
   out_6410555765576752725[87] = 0;
   out_6410555765576752725[88] = 0;
   out_6410555765576752725[89] = 0;
   out_6410555765576752725[90] = 0;
   out_6410555765576752725[91] = 0;
   out_6410555765576752725[92] = 0;
   out_6410555765576752725[93] = 0;
   out_6410555765576752725[94] = 0;
   out_6410555765576752725[95] = 1;
   out_6410555765576752725[96] = 0;
   out_6410555765576752725[97] = 0;
   out_6410555765576752725[98] = 0;
   out_6410555765576752725[99] = 0;
   out_6410555765576752725[100] = 0;
   out_6410555765576752725[101] = 0;
   out_6410555765576752725[102] = 0;
   out_6410555765576752725[103] = 0;
   out_6410555765576752725[104] = dt;
   out_6410555765576752725[105] = 0;
   out_6410555765576752725[106] = 0;
   out_6410555765576752725[107] = 0;
   out_6410555765576752725[108] = 0;
   out_6410555765576752725[109] = 0;
   out_6410555765576752725[110] = 0;
   out_6410555765576752725[111] = 0;
   out_6410555765576752725[112] = 0;
   out_6410555765576752725[113] = 0;
   out_6410555765576752725[114] = 1;
   out_6410555765576752725[115] = 0;
   out_6410555765576752725[116] = 0;
   out_6410555765576752725[117] = 0;
   out_6410555765576752725[118] = 0;
   out_6410555765576752725[119] = 0;
   out_6410555765576752725[120] = 0;
   out_6410555765576752725[121] = 0;
   out_6410555765576752725[122] = 0;
   out_6410555765576752725[123] = 0;
   out_6410555765576752725[124] = 0;
   out_6410555765576752725[125] = 0;
   out_6410555765576752725[126] = 0;
   out_6410555765576752725[127] = 0;
   out_6410555765576752725[128] = 0;
   out_6410555765576752725[129] = 0;
   out_6410555765576752725[130] = 0;
   out_6410555765576752725[131] = 0;
   out_6410555765576752725[132] = 0;
   out_6410555765576752725[133] = 1;
   out_6410555765576752725[134] = 0;
   out_6410555765576752725[135] = 0;
   out_6410555765576752725[136] = 0;
   out_6410555765576752725[137] = 0;
   out_6410555765576752725[138] = 0;
   out_6410555765576752725[139] = 0;
   out_6410555765576752725[140] = 0;
   out_6410555765576752725[141] = 0;
   out_6410555765576752725[142] = 0;
   out_6410555765576752725[143] = 0;
   out_6410555765576752725[144] = 0;
   out_6410555765576752725[145] = 0;
   out_6410555765576752725[146] = 0;
   out_6410555765576752725[147] = 0;
   out_6410555765576752725[148] = 0;
   out_6410555765576752725[149] = 0;
   out_6410555765576752725[150] = 0;
   out_6410555765576752725[151] = 0;
   out_6410555765576752725[152] = 1;
   out_6410555765576752725[153] = 0;
   out_6410555765576752725[154] = 0;
   out_6410555765576752725[155] = 0;
   out_6410555765576752725[156] = 0;
   out_6410555765576752725[157] = 0;
   out_6410555765576752725[158] = 0;
   out_6410555765576752725[159] = 0;
   out_6410555765576752725[160] = 0;
   out_6410555765576752725[161] = 0;
   out_6410555765576752725[162] = 0;
   out_6410555765576752725[163] = 0;
   out_6410555765576752725[164] = 0;
   out_6410555765576752725[165] = 0;
   out_6410555765576752725[166] = 0;
   out_6410555765576752725[167] = 0;
   out_6410555765576752725[168] = 0;
   out_6410555765576752725[169] = 0;
   out_6410555765576752725[170] = 0;
   out_6410555765576752725[171] = 1;
   out_6410555765576752725[172] = 0;
   out_6410555765576752725[173] = 0;
   out_6410555765576752725[174] = 0;
   out_6410555765576752725[175] = 0;
   out_6410555765576752725[176] = 0;
   out_6410555765576752725[177] = 0;
   out_6410555765576752725[178] = 0;
   out_6410555765576752725[179] = 0;
   out_6410555765576752725[180] = 0;
   out_6410555765576752725[181] = 0;
   out_6410555765576752725[182] = 0;
   out_6410555765576752725[183] = 0;
   out_6410555765576752725[184] = 0;
   out_6410555765576752725[185] = 0;
   out_6410555765576752725[186] = 0;
   out_6410555765576752725[187] = 0;
   out_6410555765576752725[188] = 0;
   out_6410555765576752725[189] = 0;
   out_6410555765576752725[190] = 1;
   out_6410555765576752725[191] = 0;
   out_6410555765576752725[192] = 0;
   out_6410555765576752725[193] = 0;
   out_6410555765576752725[194] = 0;
   out_6410555765576752725[195] = 0;
   out_6410555765576752725[196] = 0;
   out_6410555765576752725[197] = 0;
   out_6410555765576752725[198] = 0;
   out_6410555765576752725[199] = 0;
   out_6410555765576752725[200] = 0;
   out_6410555765576752725[201] = 0;
   out_6410555765576752725[202] = 0;
   out_6410555765576752725[203] = 0;
   out_6410555765576752725[204] = 0;
   out_6410555765576752725[205] = 0;
   out_6410555765576752725[206] = 0;
   out_6410555765576752725[207] = 0;
   out_6410555765576752725[208] = 0;
   out_6410555765576752725[209] = 1;
   out_6410555765576752725[210] = 0;
   out_6410555765576752725[211] = 0;
   out_6410555765576752725[212] = 0;
   out_6410555765576752725[213] = 0;
   out_6410555765576752725[214] = 0;
   out_6410555765576752725[215] = 0;
   out_6410555765576752725[216] = 0;
   out_6410555765576752725[217] = 0;
   out_6410555765576752725[218] = 0;
   out_6410555765576752725[219] = 0;
   out_6410555765576752725[220] = 0;
   out_6410555765576752725[221] = 0;
   out_6410555765576752725[222] = 0;
   out_6410555765576752725[223] = 0;
   out_6410555765576752725[224] = 0;
   out_6410555765576752725[225] = 0;
   out_6410555765576752725[226] = 0;
   out_6410555765576752725[227] = 0;
   out_6410555765576752725[228] = 1;
   out_6410555765576752725[229] = 0;
   out_6410555765576752725[230] = 0;
   out_6410555765576752725[231] = 0;
   out_6410555765576752725[232] = 0;
   out_6410555765576752725[233] = 0;
   out_6410555765576752725[234] = 0;
   out_6410555765576752725[235] = 0;
   out_6410555765576752725[236] = 0;
   out_6410555765576752725[237] = 0;
   out_6410555765576752725[238] = 0;
   out_6410555765576752725[239] = 0;
   out_6410555765576752725[240] = 0;
   out_6410555765576752725[241] = 0;
   out_6410555765576752725[242] = 0;
   out_6410555765576752725[243] = 0;
   out_6410555765576752725[244] = 0;
   out_6410555765576752725[245] = 0;
   out_6410555765576752725[246] = 0;
   out_6410555765576752725[247] = 1;
   out_6410555765576752725[248] = 0;
   out_6410555765576752725[249] = 0;
   out_6410555765576752725[250] = 0;
   out_6410555765576752725[251] = 0;
   out_6410555765576752725[252] = 0;
   out_6410555765576752725[253] = 0;
   out_6410555765576752725[254] = 0;
   out_6410555765576752725[255] = 0;
   out_6410555765576752725[256] = 0;
   out_6410555765576752725[257] = 0;
   out_6410555765576752725[258] = 0;
   out_6410555765576752725[259] = 0;
   out_6410555765576752725[260] = 0;
   out_6410555765576752725[261] = 0;
   out_6410555765576752725[262] = 0;
   out_6410555765576752725[263] = 0;
   out_6410555765576752725[264] = 0;
   out_6410555765576752725[265] = 0;
   out_6410555765576752725[266] = 1;
   out_6410555765576752725[267] = 0;
   out_6410555765576752725[268] = 0;
   out_6410555765576752725[269] = 0;
   out_6410555765576752725[270] = 0;
   out_6410555765576752725[271] = 0;
   out_6410555765576752725[272] = 0;
   out_6410555765576752725[273] = 0;
   out_6410555765576752725[274] = 0;
   out_6410555765576752725[275] = 0;
   out_6410555765576752725[276] = 0;
   out_6410555765576752725[277] = 0;
   out_6410555765576752725[278] = 0;
   out_6410555765576752725[279] = 0;
   out_6410555765576752725[280] = 0;
   out_6410555765576752725[281] = 0;
   out_6410555765576752725[282] = 0;
   out_6410555765576752725[283] = 0;
   out_6410555765576752725[284] = 0;
   out_6410555765576752725[285] = 1;
   out_6410555765576752725[286] = 0;
   out_6410555765576752725[287] = 0;
   out_6410555765576752725[288] = 0;
   out_6410555765576752725[289] = 0;
   out_6410555765576752725[290] = 0;
   out_6410555765576752725[291] = 0;
   out_6410555765576752725[292] = 0;
   out_6410555765576752725[293] = 0;
   out_6410555765576752725[294] = 0;
   out_6410555765576752725[295] = 0;
   out_6410555765576752725[296] = 0;
   out_6410555765576752725[297] = 0;
   out_6410555765576752725[298] = 0;
   out_6410555765576752725[299] = 0;
   out_6410555765576752725[300] = 0;
   out_6410555765576752725[301] = 0;
   out_6410555765576752725[302] = 0;
   out_6410555765576752725[303] = 0;
   out_6410555765576752725[304] = 1;
   out_6410555765576752725[305] = 0;
   out_6410555765576752725[306] = 0;
   out_6410555765576752725[307] = 0;
   out_6410555765576752725[308] = 0;
   out_6410555765576752725[309] = 0;
   out_6410555765576752725[310] = 0;
   out_6410555765576752725[311] = 0;
   out_6410555765576752725[312] = 0;
   out_6410555765576752725[313] = 0;
   out_6410555765576752725[314] = 0;
   out_6410555765576752725[315] = 0;
   out_6410555765576752725[316] = 0;
   out_6410555765576752725[317] = 0;
   out_6410555765576752725[318] = 0;
   out_6410555765576752725[319] = 0;
   out_6410555765576752725[320] = 0;
   out_6410555765576752725[321] = 0;
   out_6410555765576752725[322] = 0;
   out_6410555765576752725[323] = 1;
}
void h_4(double *state, double *unused, double *out_2896490222522275789) {
   out_2896490222522275789[0] = state[6] + state[9];
   out_2896490222522275789[1] = state[7] + state[10];
   out_2896490222522275789[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_1964100429216498701) {
   out_1964100429216498701[0] = 0;
   out_1964100429216498701[1] = 0;
   out_1964100429216498701[2] = 0;
   out_1964100429216498701[3] = 0;
   out_1964100429216498701[4] = 0;
   out_1964100429216498701[5] = 0;
   out_1964100429216498701[6] = 1;
   out_1964100429216498701[7] = 0;
   out_1964100429216498701[8] = 0;
   out_1964100429216498701[9] = 1;
   out_1964100429216498701[10] = 0;
   out_1964100429216498701[11] = 0;
   out_1964100429216498701[12] = 0;
   out_1964100429216498701[13] = 0;
   out_1964100429216498701[14] = 0;
   out_1964100429216498701[15] = 0;
   out_1964100429216498701[16] = 0;
   out_1964100429216498701[17] = 0;
   out_1964100429216498701[18] = 0;
   out_1964100429216498701[19] = 0;
   out_1964100429216498701[20] = 0;
   out_1964100429216498701[21] = 0;
   out_1964100429216498701[22] = 0;
   out_1964100429216498701[23] = 0;
   out_1964100429216498701[24] = 0;
   out_1964100429216498701[25] = 1;
   out_1964100429216498701[26] = 0;
   out_1964100429216498701[27] = 0;
   out_1964100429216498701[28] = 1;
   out_1964100429216498701[29] = 0;
   out_1964100429216498701[30] = 0;
   out_1964100429216498701[31] = 0;
   out_1964100429216498701[32] = 0;
   out_1964100429216498701[33] = 0;
   out_1964100429216498701[34] = 0;
   out_1964100429216498701[35] = 0;
   out_1964100429216498701[36] = 0;
   out_1964100429216498701[37] = 0;
   out_1964100429216498701[38] = 0;
   out_1964100429216498701[39] = 0;
   out_1964100429216498701[40] = 0;
   out_1964100429216498701[41] = 0;
   out_1964100429216498701[42] = 0;
   out_1964100429216498701[43] = 0;
   out_1964100429216498701[44] = 1;
   out_1964100429216498701[45] = 0;
   out_1964100429216498701[46] = 0;
   out_1964100429216498701[47] = 1;
   out_1964100429216498701[48] = 0;
   out_1964100429216498701[49] = 0;
   out_1964100429216498701[50] = 0;
   out_1964100429216498701[51] = 0;
   out_1964100429216498701[52] = 0;
   out_1964100429216498701[53] = 0;
}
void h_10(double *state, double *unused, double *out_4680354251909638720) {
   out_4680354251909638720[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_4680354251909638720[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_4680354251909638720[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_7907331316519793486) {
   out_7907331316519793486[0] = 0;
   out_7907331316519793486[1] = 9.8100000000000005*cos(state[1]);
   out_7907331316519793486[2] = 0;
   out_7907331316519793486[3] = 0;
   out_7907331316519793486[4] = -state[8];
   out_7907331316519793486[5] = state[7];
   out_7907331316519793486[6] = 0;
   out_7907331316519793486[7] = state[5];
   out_7907331316519793486[8] = -state[4];
   out_7907331316519793486[9] = 0;
   out_7907331316519793486[10] = 0;
   out_7907331316519793486[11] = 0;
   out_7907331316519793486[12] = 1;
   out_7907331316519793486[13] = 0;
   out_7907331316519793486[14] = 0;
   out_7907331316519793486[15] = 1;
   out_7907331316519793486[16] = 0;
   out_7907331316519793486[17] = 0;
   out_7907331316519793486[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_7907331316519793486[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_7907331316519793486[20] = 0;
   out_7907331316519793486[21] = state[8];
   out_7907331316519793486[22] = 0;
   out_7907331316519793486[23] = -state[6];
   out_7907331316519793486[24] = -state[5];
   out_7907331316519793486[25] = 0;
   out_7907331316519793486[26] = state[3];
   out_7907331316519793486[27] = 0;
   out_7907331316519793486[28] = 0;
   out_7907331316519793486[29] = 0;
   out_7907331316519793486[30] = 0;
   out_7907331316519793486[31] = 1;
   out_7907331316519793486[32] = 0;
   out_7907331316519793486[33] = 0;
   out_7907331316519793486[34] = 1;
   out_7907331316519793486[35] = 0;
   out_7907331316519793486[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_7907331316519793486[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_7907331316519793486[38] = 0;
   out_7907331316519793486[39] = -state[7];
   out_7907331316519793486[40] = state[6];
   out_7907331316519793486[41] = 0;
   out_7907331316519793486[42] = state[4];
   out_7907331316519793486[43] = -state[3];
   out_7907331316519793486[44] = 0;
   out_7907331316519793486[45] = 0;
   out_7907331316519793486[46] = 0;
   out_7907331316519793486[47] = 0;
   out_7907331316519793486[48] = 0;
   out_7907331316519793486[49] = 0;
   out_7907331316519793486[50] = 1;
   out_7907331316519793486[51] = 0;
   out_7907331316519793486[52] = 0;
   out_7907331316519793486[53] = 1;
}
void h_13(double *state, double *unused, double *out_2876827965017568376) {
   out_2876827965017568376[0] = state[3];
   out_2876827965017568376[1] = state[4];
   out_2876827965017568376[2] = state[5];
}
void H_13(double *state, double *unused, double *out_1248173396115834100) {
   out_1248173396115834100[0] = 0;
   out_1248173396115834100[1] = 0;
   out_1248173396115834100[2] = 0;
   out_1248173396115834100[3] = 1;
   out_1248173396115834100[4] = 0;
   out_1248173396115834100[5] = 0;
   out_1248173396115834100[6] = 0;
   out_1248173396115834100[7] = 0;
   out_1248173396115834100[8] = 0;
   out_1248173396115834100[9] = 0;
   out_1248173396115834100[10] = 0;
   out_1248173396115834100[11] = 0;
   out_1248173396115834100[12] = 0;
   out_1248173396115834100[13] = 0;
   out_1248173396115834100[14] = 0;
   out_1248173396115834100[15] = 0;
   out_1248173396115834100[16] = 0;
   out_1248173396115834100[17] = 0;
   out_1248173396115834100[18] = 0;
   out_1248173396115834100[19] = 0;
   out_1248173396115834100[20] = 0;
   out_1248173396115834100[21] = 0;
   out_1248173396115834100[22] = 1;
   out_1248173396115834100[23] = 0;
   out_1248173396115834100[24] = 0;
   out_1248173396115834100[25] = 0;
   out_1248173396115834100[26] = 0;
   out_1248173396115834100[27] = 0;
   out_1248173396115834100[28] = 0;
   out_1248173396115834100[29] = 0;
   out_1248173396115834100[30] = 0;
   out_1248173396115834100[31] = 0;
   out_1248173396115834100[32] = 0;
   out_1248173396115834100[33] = 0;
   out_1248173396115834100[34] = 0;
   out_1248173396115834100[35] = 0;
   out_1248173396115834100[36] = 0;
   out_1248173396115834100[37] = 0;
   out_1248173396115834100[38] = 0;
   out_1248173396115834100[39] = 0;
   out_1248173396115834100[40] = 0;
   out_1248173396115834100[41] = 1;
   out_1248173396115834100[42] = 0;
   out_1248173396115834100[43] = 0;
   out_1248173396115834100[44] = 0;
   out_1248173396115834100[45] = 0;
   out_1248173396115834100[46] = 0;
   out_1248173396115834100[47] = 0;
   out_1248173396115834100[48] = 0;
   out_1248173396115834100[49] = 0;
   out_1248173396115834100[50] = 0;
   out_1248173396115834100[51] = 0;
   out_1248173396115834100[52] = 0;
   out_1248173396115834100[53] = 0;
}
void h_14(double *state, double *unused, double *out_1032771649440202970) {
   out_1032771649440202970[0] = state[6];
   out_1032771649440202970[1] = state[7];
   out_1032771649440202970[2] = state[8];
}
void H_14(double *state, double *unused, double *out_5046888861511870997) {
   out_5046888861511870997[0] = 0;
   out_5046888861511870997[1] = 0;
   out_5046888861511870997[2] = 0;
   out_5046888861511870997[3] = 0;
   out_5046888861511870997[4] = 0;
   out_5046888861511870997[5] = 0;
   out_5046888861511870997[6] = 1;
   out_5046888861511870997[7] = 0;
   out_5046888861511870997[8] = 0;
   out_5046888861511870997[9] = 0;
   out_5046888861511870997[10] = 0;
   out_5046888861511870997[11] = 0;
   out_5046888861511870997[12] = 0;
   out_5046888861511870997[13] = 0;
   out_5046888861511870997[14] = 0;
   out_5046888861511870997[15] = 0;
   out_5046888861511870997[16] = 0;
   out_5046888861511870997[17] = 0;
   out_5046888861511870997[18] = 0;
   out_5046888861511870997[19] = 0;
   out_5046888861511870997[20] = 0;
   out_5046888861511870997[21] = 0;
   out_5046888861511870997[22] = 0;
   out_5046888861511870997[23] = 0;
   out_5046888861511870997[24] = 0;
   out_5046888861511870997[25] = 1;
   out_5046888861511870997[26] = 0;
   out_5046888861511870997[27] = 0;
   out_5046888861511870997[28] = 0;
   out_5046888861511870997[29] = 0;
   out_5046888861511870997[30] = 0;
   out_5046888861511870997[31] = 0;
   out_5046888861511870997[32] = 0;
   out_5046888861511870997[33] = 0;
   out_5046888861511870997[34] = 0;
   out_5046888861511870997[35] = 0;
   out_5046888861511870997[36] = 0;
   out_5046888861511870997[37] = 0;
   out_5046888861511870997[38] = 0;
   out_5046888861511870997[39] = 0;
   out_5046888861511870997[40] = 0;
   out_5046888861511870997[41] = 0;
   out_5046888861511870997[42] = 0;
   out_5046888861511870997[43] = 0;
   out_5046888861511870997[44] = 1;
   out_5046888861511870997[45] = 0;
   out_5046888861511870997[46] = 0;
   out_5046888861511870997[47] = 0;
   out_5046888861511870997[48] = 0;
   out_5046888861511870997[49] = 0;
   out_5046888861511870997[50] = 0;
   out_5046888861511870997[51] = 0;
   out_5046888861511870997[52] = 0;
   out_5046888861511870997[53] = 0;
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

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_1520136949545670133) {
  err_fun(nom_x, delta_x, out_1520136949545670133);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_5028598293345485084) {
  inv_err_fun(nom_x, true_x, out_5028598293345485084);
}
void pose_H_mod_fun(double *state, double *out_1187816111513252734) {
  H_mod_fun(state, out_1187816111513252734);
}
void pose_f_fun(double *state, double dt, double *out_7179860701368214742) {
  f_fun(state,  dt, out_7179860701368214742);
}
void pose_F_fun(double *state, double dt, double *out_6410555765576752725) {
  F_fun(state,  dt, out_6410555765576752725);
}
void pose_h_4(double *state, double *unused, double *out_2896490222522275789) {
  h_4(state, unused, out_2896490222522275789);
}
void pose_H_4(double *state, double *unused, double *out_1964100429216498701) {
  H_4(state, unused, out_1964100429216498701);
}
void pose_h_10(double *state, double *unused, double *out_4680354251909638720) {
  h_10(state, unused, out_4680354251909638720);
}
void pose_H_10(double *state, double *unused, double *out_7907331316519793486) {
  H_10(state, unused, out_7907331316519793486);
}
void pose_h_13(double *state, double *unused, double *out_2876827965017568376) {
  h_13(state, unused, out_2876827965017568376);
}
void pose_H_13(double *state, double *unused, double *out_1248173396115834100) {
  H_13(state, unused, out_1248173396115834100);
}
void pose_h_14(double *state, double *unused, double *out_1032771649440202970) {
  h_14(state, unused, out_1032771649440202970);
}
void pose_H_14(double *state, double *unused, double *out_5046888861511870997) {
  H_14(state, unused, out_5046888861511870997);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
