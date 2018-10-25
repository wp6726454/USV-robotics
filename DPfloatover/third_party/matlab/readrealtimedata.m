clear;
close all;

% read csv file
path='/home/skloe/Coding/CPP1X/USV/DPfloatover/QT/build/dataprocess/';
name_first='First.csv';
path_first=strcat(path,name_first);
totaldata_first=csvread(path_first,1,0);
% generate timestamp in seconds
timestamp0_first=totaldata_first(1,3);
timestamp_first=(totaldata_first(:,3)-timestamp0_first)*86400.0;  % seconds
Position_first=totaldata_first(:,4:9);
State_first=totaldata_first(:,10:15);
Tau_first=totaldata_first(:,16:18);
est_first=totaldata_first(:,19:21);
alpha_first=totaldata_first(:,22:24);
rpm_first=totaldata_first(:,25:27);

name_second='Second.csv';
path_second=strcat(path,name_second);
totaldata_second=csvread(path_second,1,0);
% generate timestamp in seconds
timestamp0_second=totaldata_second(1,3);
timestamp_second=(totaldata_second(:,3)-timestamp0_second)*86400.0;  % seconds
Position_second=totaldata_second(:,4:9);
State_second=totaldata_second(:,10:15);
Tau_second=totaldata_second(:,16:18);
est_second=totaldata_second(:,19:21);
alpha_second=totaldata_second(:,22:24);
rpm_second=totaldata_second(:,25:27);


%% figure 1 
figure(1)
subplot(311)
plot(timestamp_second,Position_second(:,1),'-r','linewidth',2);hold on;
plot(timestamp_second,State_second(:,1),':k','linewidth',2); hold on;
plot([0,timestamp_second(end)],[6,6],'--b');
legend('position','State');
xlabel('time(s)');
ylabel('surge(m)');
title('Position comparison')

subplot(312)
plot(timestamp_second,Position_second(:,2),'-r','linewidth',2); hold on;
plot(timestamp_second,State_second(:,2),':k','linewidth',2); hold on;
plot([0,timestamp_second(end)],[4,4],'--b');
legend('position','State');
xlabel('time(s)');
ylabel('sway(m)');

subplot(313)
plot(timestamp_second,Position_second(:,6),'-r','linewidth',2);hold on;
plot(timestamp_second,State_second(:,3)*180/pi,':k','linewidth',2); hold on;
plot([0,timestamp_second(end)],[0,0],'--b');
legend('position','State');
xlabel('time(s)');
ylabel('Yaw(rad)');

%% figure 2 
figure(2)
subplot(311)
plot(timestamp_second,State_second(:,4),'-r','linewidth',2);
xlabel('time(s)');
ylabel('surge(m/s)');
title('Velocity')

subplot(312)
plot(timestamp_second,State_second(:,5),'-r','linewidth',2);
xlabel('time(s)');
ylabel('sway(m/s)');

subplot(313)
plot(timestamp_second,State_second(:,6),'-r','linewidth',2);
xlabel('time(s)');
ylabel('yaw(rad/s)');



%% figure 3
figure(3)
title('Trajectory')
plot(Position_second(:,2),Position_second(:,1),'ro','MarkerSize',2); 


%% figure 4
figure(4)
subplot(311)
plot(timestamp_second, Tau_second(:,1),'-r','linewidth',2); hold on; 
plot(timestamp_second, est_second(:,1),':k','linewidth',2);
xlabel('time(s)');
ylabel('surge(N)');
legend('Desired force','Estimated force');
title('PID force')
subplot(312)
plot(timestamp_second, Tau_second(:,2),'-r','linewidth',2); hold on; 
plot(timestamp_second, est_second(:,2),':k','linewidth',2);
xlabel('time(s)');
ylabel('sway(N)');
legend('Desired force','Estimated force');
subplot(313)
plot(timestamp_second, Tau_second(:,3),'-r','linewidth',2); hold on; 
plot(timestamp_second, est_second(:,3),':k','linewidth',2);
xlabel('time(s)');
ylabel('Yaw(N*m)');
legend('Desired force','Estimated force');


