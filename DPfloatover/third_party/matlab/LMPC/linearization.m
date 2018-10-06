
clear
clc

syms x y psi u v r;
syms tau1 tau2 tau3;

nu = [u;v;r];
state = [x;y;psi;u;v;r];
% tau = [tau1;tau2];
tau = [tau1;tau2;tau3];

%% 船模参数
m11 = 25.8;
m22 = 33.8;
m23 = 1;
m32 = 1;
m33 = 2.8;
M = [m11 0 0;0 m22 m23;0 m32 m33];
D = [0.72 0 0;0 0.89 0.03;0 0.03 1.9];
c23 = m11*u;
c31 = m22*v+1/2*(m23+m32)*r;
C = [0 0 -c31;0 0 c23;c31 -c23 0];

%% 求雅克比矩阵
dx_1 = [cos(psi) -sin(psi) 0;sin(psi) cos(psi) 0;0 0 1]*nu;
%dx_2 = inv(M)*(-C*nu-D*nu+[1 0;0 0;0 1]*tau);
dx_2 = inv(M)*(-C*nu-D*nu+tau);
f = [dx_1;dx_2];
A = jacobian(f,state);
B = jacobian(f,tau);

%% 离散化
I = eye(6);
syms T;
AA = I + T*A;
BB = T*B;
AA1 = vpa(AA,5);
BB1 = vpa(BB,5);
