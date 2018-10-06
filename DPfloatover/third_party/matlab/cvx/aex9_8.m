clear;
close all;

%% Additional exercise 9.8
%% standard form LP barrier method with 
% infeasible start Newton method

%% generate A and b randomly
m=100;
n=500;
A=randn(m,n);
A(1,:)=A(1,:)-min(min(A))*ones(1,n);
c=randn(n,1);
x=randn(n,1);
x(x<0)=-x(x<0);
x=x+0.5; % positive x

b=A*x;


%% Infeasible start Newton method

% iteration constant
alpha=0.1;
beta=0.5;
tolerance=1e-3;
constraint_delta=1e-2;
duality_gap=1e-3;
out_t=1;
mu=10;

x=ones(n,1);
v=ones(m,1);
inf_newton_x=x;
inf_newton_v=v;
inf_newton_val=0;

history=[];
tic
while true
    history=[history zeros(2,1)];
    % centering step
    
    while true
        % compute primal and dual Newton steps
        g=out_t*c-1./x;
        inv_H=diag(x.^2);
        omega=(A*inv_H*A')\(A*x-b-A*inv_H*g);
        nt_v=omega-v;                               % dual step
        nt_x=-inv_H*(g+A'*omega);            % Newton step
        
        constrain_error=norm(A*x-b);
        r= inewton(x,v,A,b,c,out_t);
        % stopping criterion
        if constrain_error<=constraint_delta && r<=tolerance
            break;
        else
            % backtracking line search in norm of r
            inner_t=1;
            while inewton(x+inner_t*nt_x,v+inner_t*nt_v,A,b,c,out_t)>(1-alpha*inner_t)*r
                inner_t=beta*inner_t;
            end
            
            % Update the inter loop
            x=x+inner_t*nt_x;
            v=v+inner_t*nt_v;
            history(1,end)=history(1,end)+1;
        end
    end
    
    % update the outer loop
    t_duality_gap=m/out_t;
    history(2,end)=t_duality_gap;
    if t_duality_gap<duality_gap    % stopping criterion
        inf_newton_x=x;
        inf_newton_v=v;
        inf_newton_val=c'*x;
        break;
    else
        % increase t
        out_t=mu*out_t;
    end
end
toc
[xx,yy]=stairs(cumsum(history(1,:)),history(2,:));
semilogy(xx,yy);



%% cvx
cvx_begin
    variable x(n)
    minimize(c'*x)
    subject to
        A*x-b==0;
        x>=0;
cvx_end



%% utility function

function r=inewton(x,v,A,b,c,out_t)
    r=out_t*c-1./x+A'*v;
    r=[r;A*x-b];
    r=norm(r);
    % check if x is all positive
    if isempty(find(x<0, 1))
        r=r;
    else
        r=r*10;
    end
end


