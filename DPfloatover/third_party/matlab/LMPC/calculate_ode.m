
clear
clc

syms x1 x2 x3 x4 x5 x6;
syms tau_u1 tau_r1;
nu = [x4;x5;x6];
tau = [tau_u1;tau_r1];

%% 船模参数
m11 = 25.8;
m22 = 33.8;
m23 = 1;
m32 = 1;
m33 = 2.8;
M = [m11 0 0;0 m22 m23;0 m32 m33];
D = [0.72 0 0;0 0.89 0.03;0 0.03 1.9];
c23 = m11*x4;
c31 = m22*x5+1/2*(m23+m32)*x6;
C = [0 0 -c31;0 0 c23;c31 -c23 0];

%% 求雅克比矩阵
dx_1 = [cos(x3) -sin(x3) 0;sin(x3) cos(x3) 0;0 0 1]*nu
dx_2 = inv(M)*(-C*nu-D*nu+[1 0;0 0;0 1]*tau)