% initial condition response of state-space model for oscillating masses
clear;
close all;


%% utility function in matlab
A_bar=[-2 1 0 0 0 0;
        1 -2 1 0 0 0;
        0 1 -2 1 0 0;
        0 0 1 -2 1 0;
        0 0 0 1 -2 1;
        0 0 0 0 1 -2];
    
A=sparse([zeros(6) eye(6);
    A_bar zeros(6)]);

B=sparse([zeros(6,3);
    1 0 0;
    -1 0 0;
    0 1 0;
    0 0 1;
    0 -1 0;
    0 0 -1]);

C=[1 zeros(1,11)];
x0=[0 1 0 0 0 0 0 0 0 0 0 0];

sys=ss(A,B,C,[]);
figure(1)
initial(sys,x0,200,'-r'); hold on;

%% continuous to discrete-time using matlab function
Ts=0.01;
sysd=c2d(sys,Ts);
initial(sysd, x0,200, '--b'); hold on;


%%
sparse_eps=1e-5;
Ad=sysd.A;
Ad(find(abs(Ad)<sparse_eps))=0;
Bd=sysd.B;
Bd(find(abs(Bd)<sparse_eps))=0;
totallength=20000;
x=zeros(12,totallength);
time=Ts*(1:totallength);
x(:,1)=x0';
for i=1:totallength-1
    x(:,i+1)= Ad*x(:,i);
end
plot(time,x(1,:),'ok');


%% output csv file
csvwrite('../controller/example/Ad.csv',Ad);
csvwrite('../controller/example/Bd.csv',Bd);
