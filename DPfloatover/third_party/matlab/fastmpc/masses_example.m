clear;
close all;
% Runs 100 steps of fast MPC on an example with 6 masses
% connected by springs, using fmpc_step().
rand('state',0); % make

MPC=generateMPCdata();

%% start MPC
totalstep=100;
x=zeros(MPC.n,totalstep);
u=zeros(MPC.m,totalstep-1);
J=zeros(1,totalstep-1);
x(:,1)=[0;0;0;0;0;0;0;0; 0; 0; 0; 0];


tic 
for i=1:totalstep-1

    % update g and b
    MPC.g(1:MPC.m,1)=MPC.r+2*MPC.S'*x(:,i);
    MPC.b(1:MPC.n,1)=MPC.A*x(:,i);
    % start QP
    cvx_begin
    cvx_quiet(true);
        variables z(MPC.nz,1);
        minimize(z'*MPC.H*z+MPC.g'*z);
        subject to
            MPC.P*z<=MPC.h;
            MPC.C*z==MPC.b;
    cvx_end
    
    
    if strcmp(cvx_status,'Solved')
        %save J
        J(i)=cvx_optval;
        % save u
        u(:,i)=z(1:MPC.m,1);
        % update  x
        disturbance=[zeros(MPC.n/2,1);rand(MPC.n/2,1)-0.5*ones(MPC.n/2,1)];
        x(:,i+1)=MPC.A*x(:,i)+MPC.B*u(:,i)+disturbance;
    else
        break;
    end

clear z;
end

toc

figure(1)
subplot(211)
stairs(x(1,:));
subplot(212)
stairs(u(1,:));
ylim([-0.6 0.6]);

%% compare to fast mpc
% copy csv file
path='/home/scar1et/Coding/CPP1X/USV/DPfloatover/third_party/fast_mpc/example/';

fast_u=csvread(strcat(path,'u.csv'));
fast_x=csvread(strcat(path,'x.csv'));

figure(1)
subplot(211)
stairs(x(1,:),fast_x(1,:));
subplot(212)
stairs(u(1,:));
ylim([-0.6 0.6]);




