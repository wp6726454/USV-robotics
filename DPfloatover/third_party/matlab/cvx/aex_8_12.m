clear;
close all

%% Additional exercise 8.12
%% Infeasible start Newton method for LP
%% centering problem

%% generate A and b randomly
m=30;
n=100;
A=randn(m,n);
A(1,:)=A(1,:)-min(min(A))*ones(1,n);
c=randn(n,1);
x=randn(n,1);
x(x<0)=-x(x<0);
x=x+0.5; % positive x

b=A*x;

% iteration constant
alpha=0.1;
beta=0.5;
tolerance=1e-6;


%% cvx
cvx_begin
    variable x(n)
    minimize(c'*x-sum(log(x)))
    subject to 
        A*x==b;
cvx_end

cvx_x=x;


%% Infeasible start Newton method
constraint_delta=1e-2;

x=ones(n,1);
v=ones(m,1);
inf_newton_x=x;
inf_newton_v=v;
inf_newton_val=0;

iteration_step=1;
plot_r=[];
while true
    % compute primal and dual Newton steps
    g=c-1./x;
    inv_H=diag(x.^2);
    omega=(A*inv_H*A')\(A*x-b-A*inv_H*g); 
    nt_v=omega-v;                               % dual step
    nt_x=-inv_H*(g+A'*omega);            % Newton step
    
    constrain_error=norm(A*x-b);
    r= inewton(x,v,A,b,c);
    
    % stopping criterion
    if constrain_error<=constraint_delta && r<=tolerance
        inf_newton_x=x;
        inf_newton_v=v;
        inf_newton_val=objfunction(c,x);
        break;
    else
        % backtracking line search in norm of r
        t=1;
        while inewton(x+t*nt_x,v+t*nt_v,A,b,c)>(1-alpha*t)*r
            t=beta*t;
        end

        % Update
        x=x+t*nt_x;
        v=v+t*nt_v;
        
        iteration_step=iteration_step+1;
        plot_r=[plot_r; r];
    end
end
iteration_step=iteration_step+1;
plot_r=[plot_r; r];

figure(1)
semilogy(1:iteration_step-1,plot_r,'-ro');
xlabel('iteration')
ylabel('|r|')


function r=inewton(x,v,A,b,c)
    r=c-1./x+A'*v;
    r=[r;A*x-b];
    r=norm(r);
    
    % check if x is all positive
    if isempty(find(x<0, 1))
        r=r;
    else
        r=r*10;
    end
end

function y=objfunction(c,x)
    y=c'*x-sum(log(x));
end