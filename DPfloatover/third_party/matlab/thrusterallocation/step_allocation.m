%============Control Allocation============================%
%=========IEEE CST Constrained Nonlinear Control Allocation 
%=========With Singularity Avoidance Using
%=========Sequential Quadratic Programming
clear;
close all;
tic
%% Set Parameters
Q=10*eye(3);
R=eye(4);
lx=[20;20;-20;-20];
ly=[5;-5;5;-5];
N=200;%Simulation time
i=1:30;
umax=1;
umin=0;
deltaumax=0.02;
deltaumin=-0.02;
alphamax=25/180*pi;
alphamin=-25/180*pi;
deltaamax=1/180*pi;
deltaamin=-1/180*pi;
%Desired forces
m=2.8*sin(pi/15*i);
M=[zeros(2,30);m];
tau=[zeros(3,70),M,zeros(3,100)]+[2.7+0.0*rand(1,200);0.0*rand(1,200);zeros(1,200)];

%% Optimization
syms a b c d
u0=zeros(4,1)+0.021;
alpha0=zeros(4,1)+0.01;
ut=zeros(8,N);
for k=1:200
    
    %cvx_solver gurobi
    cvx_begin
    variable u(11)

%% Initial states

    
    B0=fB(alpha0);
    x=[a;b;c;d];
    JB=jacobian(fB(x)*u0,x);
    JR=jacobian(frho(x),x);
    JB=subs(JB,x,alpha0);
    JR=subs(JR,x,alpha0);
    JB=eval(JB);
    JR=eval(JR);
    
    
    u(1:4)-(umax-u0)<=0;
    u(1:4)-(umin-u0)>=0;
    u(1:4)-deltaumax<=0;
    u(1:4)-deltaumin>=0;
    u(5:8)-(alphamax-alpha0)<=0;
    u(5:8)-(alphamin-alpha0)>=0;
    u(5:8)-deltaamax<=0;
    u(5:8)-deltaamin>=0;
    u(9:11)+fB(alpha0)*(u(1:4)+u0)+JB*u(5:8)==tau(:,k);
    
    W1=[3/2*(abs(u0(1)))^(1/2)*sign(u0(1)),3/2*(abs(u0(2)))^(1/2)*sign(u0(2)),3/2*(abs(u0(3)))^(1/2)*sign(u0(3)),3/2*(abs(u0(4)))^(1/2)*sign(u0(4))];
    W2=[3/4*abs(u0(1))^(-1/2),3/4*abs(u0(2))^(-1/2),3/4*abs(u0(3))^(-1/2),3/4*abs(u0(4))^(-1/2)];
    
    
    minimize W1*u(1:4)+W2*(u(1:4).^2)+(u(9:11)')*Q*u(9:11)+(u(5:8)')*R*u(5:8)+JR*u(5:8)
    cvx_end
    
    ut(:,k)=u(1:8)+[u0;alpha0];
    s(:,k)=u(9:11);
    u0=ut(1:4,k);
    alpha0=ut(5:8,k);
    Rtau(:,k)=fB(alpha0)*u0;
end
toc



%% compare
% copy csv file
path='/home/scar1et/Coding/CPP1X/USV/DPfloatover/third_party/fast_mpc/example/';
myRtau=csvread(strcat(path,'Balpha.csv'));
myu=csvread(strcat(path,'u.csv'));
myalpha=csvread(strcat(path,'alpha.csv'));

figure
subplot(311)
plot(tau(1,:)/400,':b','linewidth',2); hold on;
plot(Rtau(1,:)/400,'-r','linewidth',2);hold on;
plot(myRtau(1,2:end)/400,'--k','linewidth',2);
legend('desired','achieved cvx','achieved fast');
ylabel('taux(%)')
title('Achieved Forces')
subplot(312)
plot(tau(2,:)/400,':b','linewidth',2); hold on;
plot(Rtau(2,:)/400','-r','linewidth',2);hold on;
plot(myRtau(2,2:end)/400,'--k','linewidth',2);
legend('desired','achieved cvx','achieved fast');
ylabel('tauy(%)')
subplot(313)
plot(tau(3,:)/400,':b','linewidth',2); hold on;
plot(Rtau(3,:)/400','-r','linewidth',2);hold on;
plot(myRtau(3,3:end)/400,'--k','linewidth',2);
legend('desired','achieved cvx','achieved fast');
ylabel('taun(%)')
%ylim([-100,100])




figure
subplot(411)
plot(ut(1,:),'-r','linewidth',2); hold on;
plot(myu(1,2:end),'--k','linewidth',2);
ylabel('u1(%)')
subplot(412)
plot(ut(2,:),'-r','linewidth',2);hold on;
plot(myu(2,2:end),'--k','linewidth',2);
ylabel('u2(%)')
subplot(413)
plot(ut(3,:),'-r','linewidth',2); hold on;
plot(myu(3,2:end),'--k','linewidth',2);
ylabel('u3(%)')
subplot(414)
plot(ut(4,:),'-r','linewidth',2); hold on;
plot(myu(4,2:end),'--k','linewidth',2);
ylabel('u4(%)')


figure
subplot(411)
plot(ut(5,:)/pi*180,'-r','linewidth',2) ;hold on;
plot(myalpha(1,2:end)/pi*180,'--k','linewidth',2);
ylabel('a1(deg)')
subplot(412)
plot(ut(6,:)/pi*180,'-r','linewidth',2);hold on;
plot(myalpha(2,2:end)/pi*180,'--k','linewidth',2);
ylabel('a2(deg)')
subplot(413)
plot(ut(7,:)/pi*180,'-r','linewidth',2);hold on;
plot(myalpha(3,2:end)/pi*180,'--k','linewidth',2);
ylabel('a3(deg)')
subplot(414)
plot(ut(8,:)/pi*180,'-r','linewidth',2);hold on;
plot(myalpha(4,2:end)/pi*180,'--k','linewidth',2);
ylabel('a4(deg)')
