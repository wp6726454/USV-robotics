clear;
close all;

% copy csv file
path='/home/scar1et/Coding/CPP1X/USV/DPfloatover/controller/fast_mpc/example/';

basic_u=csvread(strcat(path,'basic_u.csv'));
basic_x=csvread(strcat(path,'basic_x.csv'));
app_u=csvread(strcat(path,'app_u.csv'));
app_x=csvread(strcat(path,'app_x.csv'));

figure(1)
subplot(411)
stairs(basic_x(1,:),'-r','linewidth',2);hold on;
stairs(app_x(1,:),'-k','linewidth',2);
legend('basic QP', 'fast QP');
box off;
xlim([800 1000]);
ylabel('x_1');

subplot(412)
stairs(basic_x(2,:),'-r','linewidth',2);hold on;
stairs(app_x(2,:),'-k','linewidth',2);
legend('basic QP', 'fast QP');
box off;
xlim([800 1000]);
ylabel('x_2');

subplot(413)
stairs(basic_u(1,:),'-r','linewidth',2);hold on;
stairs(app_u(1,:),'-k','linewidth',2);
legend('basic QP', 'fast QP');
box off;
ylabel('u_1');
ylim([-0.8 0.8]);
xlim([800 1000]);

subplot(414)
stairs(basic_u(2,:),'-r','linewidth',2);hold on;
stairs(app_u(2,:),'-k','linewidth',2);
legend('basic QP', 'fast QP');
box off;
ylabel('u_2');
ylim([-0.8 0.8]);
xlim([800 1000]);