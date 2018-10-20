clear;
close all;

% copy csv file
name='First';
path='/home/skloe/Coding/CPP1X/USV/DPfloatover/controller/pidcontroller/example/';
path=strcat(path,name);
Rtau=csvread(strcat(path,'_Balpha.csv'));
u=csvread(strcat(path,'_u.csv'));
alpha=csvread(strcat(path,'_alpha.csv'));
alpha_deg=csvread(strcat(path,'_alpha_deg.csv'));
dtau=csvread(strcat(path,'_tau.csv'));
rotation=csvread(strcat(path,'_rotation.csv'));

figure
subplot(311)
plot(Rtau(1,2:end),'-r','linewidth',2); hold on;
plot(dtau(1,:),':k','linewidth',2); 
ylabel('taux(N)')
legend('achieved force','desired force')
subplot(312)
plot(Rtau(2,2:end),'-r','linewidth',2);hold on;
plot(dtau(2,:),':k','linewidth',2); 
ylabel('tauy(N)')
legend('achieved force','desired force')
subplot(313)
plot(Rtau(3,2:end),'-r','linewidth',2);hold on;
plot(dtau(3,:),':k','linewidth',2); 
legend('achieved force','desired force')
ylabel('taun(N m)')

figure
subplot(311)
plot(u(1,:),'linewidth',2);
ylabel('u1(%)')
title('real-time thrust');
subplot(312)
plot(u(2,:),'linewidth',2)
ylabel('u2(%)')
subplot(313)
plot(u(3,:),'linewidth',2)
ylabel('u3(%)')



figure
subplot(311)
plot(alpha(1,:)*180/pi,'linewidth',2);hold on;
plot(alpha_deg(1,:),':k','linewidth',2);
legend('double angle','int angle');
ylabel('a1(deg)')
title('real-time angle');
subplot(312)
plot(alpha(2,:)*180/pi,'linewidth',2); hold on;
plot(alpha_deg(2,:),':k','linewidth',2);
legend('double angle','int angle');
ylabel('a2(deg)')
subplot(313)
plot(alpha(3,:)*180/pi,'linewidth',2); hold on;
plot(alpha_deg(3,:),':k','linewidth',2);
legend('double angle','int angle');
ylabel('a3(deg)')


figure
subplot(311)
plot(rotation(1,:),'linewidth',2)
ylabel('a1(deg)')
title('real-time rotation');
subplot(312)
plot(rotation(2,:),'linewidth',2)
ylabel('a2(deg)')
subplot(313)
plot(rotation(3,:),'linewidth',2)
ylabel('a3(deg)')

