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



figure(1)
subplot(311)
plot(timestamp_second,Position_second(:,1));hold on;
plot(timestamp_second,State_second(:,1));
legend('position-Surge');
subplot(312)
plot(timestamp_second,Position_second(:,2)); hold on;
plot(timestamp_second,State_second(:,2));
legend('position-Sway');
subplot(313)
plot(timestamp_second,Position_second(:,6));hold on;
plot(timestamp_second,State_second(:,3)*180/pi);
legend('position-Yaw');





