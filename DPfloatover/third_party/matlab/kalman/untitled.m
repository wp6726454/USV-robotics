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

