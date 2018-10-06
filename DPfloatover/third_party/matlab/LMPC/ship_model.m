function dx = ship_model(t,state,control)
psi = state(3,1);
u = state(4,1);
v = state(5,1);
r = state(6,1);
nu = [u;v;r];

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

dx(1:3,1) = [cos(psi) -sin(psi) 0;sin(psi) cos(psi) 0;0 0 1]*nu;
dx(4:6,1) = inv(M)*(-C*nu-D*nu+[1 0;0 0;0 1]*control);





