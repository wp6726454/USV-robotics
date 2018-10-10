clear
close all;

name='third';
path='/home/skloe/Coding/CPP1X/USV/DPfloatover/controller/pidcontroller/example/';
path=strcat(path,name);
x=csvread(strcat(path,'_x.csv'));
u=csvread(strcat(path,'_u.csv'));
alpha=csvread(strcat(path,'_alpha.csv'));
tau=csvread(strcat(path,'_tau.csv'));
BalphaU=csvread(strcat(path,'_BalphaU.csv'));

figure(1)
subplot(311);
plot(x(1,:));
title('x');
subplot(312);
plot(x(2,:));
subplot(313);
plot(x(3,:));


figure(2)
subplot(311);
plot(u(1,:));
title('u');
subplot(312);
plot(u(2,:));
subplot(313);
plot(u(3,:));

figure(3)
subplot(311);
plot(alpha(1,:)*180/pi);
title('Angle');
subplot(312);
plot(alpha(2,:)*180/pi);
subplot(313);
plot(alpha(3,:)*180/pi);


figure(4)
subplot(311);
plot(tau(1,:)); hold on;
plot(BalphaU(1,:));
title('tau');
legend('tau','BalphaU');
subplot(312);
plot(tau(2,:));hold on;
plot(BalphaU(2,:));
legend('tau','BalphaU');
subplot(313);
plot(tau(3,:));hold on;
plot(BalphaU(3,:));
legend('tau','BalphaU');