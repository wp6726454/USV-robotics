clear
close all;

%% Optimal spacecraft landing

% constant
h = 1;
g = 0.1;
m = 10;
Fmax = 10;
p0 = [50,50,100]';
v0 = [-10,0,-10]';
alpha = 0.5;
gamma = 1;
K = 35;

% difference matrix
c1 = [-1 1 zeros(1,K-1)];
c2=[1 1 zeros(1,K-1)];
r = zeros(K,1);
D=toeplitz(c1,r);
S=toeplitz(c2,r);
e3=[0 0 1]';
e12=[1 0;
        0  1;
        0 0];
    
%% minimize fuel consumption

cvx_begin
    variables v(3,K+1) p(3,K+1) f(3,K);
    minimize(sum(norms(f,2,1)));
    subject to
        p(:,1)==p0;
        v(:,1)==v0;
        p(:,K+1)==zeros(3,1);
        v(:,K+1)==zeros(3,1);
        v*D==h/m*f-h*g*repmat(e3,1,K);
        p*D==h/2*v*S;
        norms(f,2,1)<=Fmax;
        e3'*p>=alpha*norms(e12'*p,2,1);
cvx_end

% use the following code to plot your trajectories
% plot the glide cone (don't modify)
% -------------------------------------------------------
x = linspace(-40,55,30); y = linspace(0,55,30);
[X,Y] = meshgrid(x,y);
Z = alpha*sqrt(X.^2+Y.^2);
figure; colormap autumn; surf(X,Y,Z);
axis([-40,55,0,55,0,105]);
grid on; hold on;


plot3(p(1,:),p(2,:),p(3,:),'b','linewidth',1.5);
quiver3(p(1,1:K),p(2,1:K),p(3,1:K),...
        f(1,:),f(2,:),f(3,:),0.3,'k','linewidth',1.5);
title(['Total fuel consumption is ',num2str(cvx_optval)]);
    
%% minimize touch-down time







