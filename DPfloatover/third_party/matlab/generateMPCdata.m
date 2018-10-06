function MPC = generateMPCdata()

clear;
close all;

%% properties of masses system
k = 1;          % spring constant 
lam = 0;        % damping constant 
a = -2*k;
b = -2*lam;
c = k;
d = lam;

n = 12; % state dimension
m = 3; % input dimension

Acts = [zeros(n/2) eye(n/2);
    [a,c,0,0,0,0,b,d,0,0,0,0;
     c,a,c,0,0,0,d,b,d,0,0,0;
     0,c,a,c,0,0,0,d,b,d,0,0;
     0,0,c,a,c,0,0,0,d,b,d,0;
     0,0,0,c,a,c,0,0,0,d,b,d;
     0,0,0,0,c,a,0,0,0,0,d,b]];

Bcts = [zeros(n/2,m);
    [1, 0, 0;
    -1, 0, 0;
     0, 1, 0;
     0, 0, 1;
     0,-1, 0;
     0, 0,-1]];
 
Ccts=[1 zeros(1,11)];

clear k lam a b c d;
%% convert to discrete-time system
sys=ss(Acts,Bcts,Ccts,[]);
ts = 0.5;       % sampling time
sysd=c2d(sys,ts);
sparse_eps=1e-5;
A=sysd.A;
% A(find(abs(A)<sparse_eps))=0;
B=sysd.B;
% B(find(abs(B)<sparse_eps))=0;

clear sys sysd;
clear Acts Bcts Ccts;
clear sparse_eps;
%% objective matrices
Q = 10*eye(n);      
R = eye(m); 
S = [eye(m);
    eye(m);
    eye(m);
    eye(m)];
Qf = eye(n); 
q = ones(n,1);
qf = zeros(n,1);
r = zeros(m,1);

%% constraints
Fx=[eye(n/2) zeros(n/2);
    -eye(n/2) zeros(n/2)];
fx=4*ones(n,1);
Fu=[eye(m);
    -eye(m)];
fu=0.5*ones(2*m,1);

Ff=[eye(n);
    -eye(n)];
Positionmax=2;
velocitymax=0.5;
ff=[Positionmax*ones(n/2,1);
    velocitymax*ones(n/2,1);
    Positionmax*ones(n/2,1);
    velocitymax*ones(n/2,1)];
w=zeros(n,1);

%% specify MPC parameters
T=30;


%% generate QP matrix
l_Fx=length(fx);
l_Fu=length(fu);
l_Ff=length(ff);

nz=T*(m+n);

% H and g
H=zeros(nz);
g=zeros(nz,1); % the first row block are not specified
H(1:m,1:m)=R;
H(end-n+1:end,end-n+1:end)=Qf;
g(1:m,1)=r;
g(end-n+1:end,1)=qf;

% P and h
P=zeros(T*l_Fu+(T-1)*l_Fx+l_Ff,nz);
h=zeros(T*l_Fu+(T-1)*l_Fx+l_Ff,1);
P(end-l_Ff-l_Fu+1:end, end-m-n+1:end)=[Fu zeros(l_Fu,n);
                                        zeros(l_Ff,m) Ff];
h(end-l_Ff-l_Fu+1:end)=[fu;ff];  

% C and b
C=zeros(T*n,nz);
C(1:n,1:(m+n))=[-B eye(n)];
b=zeros(T*n,1); % the first row block are not specified


for i=1:T-1
    index=(1+(m+n)*(i-1)):(m+n)*i;
    H(index+m, index+m)=[Q S;
                        S' R];
    g(index+m, 1)=[q;r];             
                    
    index_P=(1+(l_Fx+l_Fu)*(i-1)):(l_Fx+l_Fu)*i;
    P(index_P,index)=[Fu zeros(l_Fu,n);
                    zeros(l_Fx,m) Fx];
                
    h(index_P)=[fu; fx];
    
    index_Cr=(1+n*i):n*(i+1);
    index_Cc=(1+m+(m+n)*(i-1)):((m+n)*(i+1));
    C(index_Cr, index_Cc)=[-A -B eye(n)];
end

clear index index_Cc index_Cr index_P;
clear i;

%% start MPC
totalstep=500;

MPC.A=A;
MPC.B=B;
MPC.H=H;
MPC.P=P;
MPC.C=C;
MPC.h=h;
MPC.T=T;
MPC.m=m;
MPC.n=n;
MPC.nz=nz;
MPC.nmu=n*T;
MPC.g=g;
MPC.b=b;
MPC.Q=Q;
MPC.S=S;
MPC.R=R;
MPC.Qf=Qf;
MPC.q=q;
MPC.qf=qf;
MPC.r=r;
MPC.Fx=Fx;
MPC.fx=fx;
MPC.Fu=Fu;
MPC.fu=fu;
MPC.Ff=Ff;
MPC.ff=ff;

end