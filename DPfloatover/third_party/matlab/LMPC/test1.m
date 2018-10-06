clear
clc

%% 参考轨迹生成
N = 100; %参考点个数 187
T = 1; %采样时间 2

Xout=zeros(N,6);
Tout=zeros(N,1);

%定点
for k1=1:1:N
    Xout(k1,1)=2.5;
    Xout(k1,2)=2.5;
    Xout(k1,3)=pi/4;
    Xout(k1,4)=0;
    Xout(k1,5)=0;
    Xout(k1,6)=0;
    Tout(k1,1)=(k1-1)*T;
end
    

%% 参数设置
Nx = 2; %输出个数
Ns = 6; %状态个数
Nu = 2; %输入个数

Np = 10; %预测时域 10
Nc = 8; %控制时域 8
q=100*eye(2);
Q_cell=cell(Np,Np);
for i=1:1:Np
    for j=1:1:Np
        if i==j
            Q_cell{i,j}=q;
        else 
            Q_cell{i,j}=zeros(Nx,Nx);
        end 
    end
end
Q=cell2mat(Q_cell);
R=1*eye(Nu*Nc,Nu*Nc);
x0 = [2 2 pi/4 0 0 0];%初始状态
tau_u = 0;%0.075
tau_r = 0;%0.015

x_real=zeros(N,Ns);
x_piao=zeros(N,Ns);
u_real=zeros(N,Nu);
u_piao=zeros(N,Nu);
x_real(1,:)=x0;
x_piao(1,:)=x_real(1,:)-Xout(1,:);

u_max = [2;0.4];
u_min = [-2;-0.4];
delta_u_max = [1;0.2];
delta_u_min = [-1;-0.2];
y_max = [10;12];
y_min = [-10;-12];
Umax = kron(ones(Nc,1),u_max);
Umin = kron(ones(Nc,1),u_min);
delta_Umax = kron(ones(Nc,1),delta_u_max);
delta_Umin = kron(ones(Nc,1),delta_u_min);
Ymax = kron(ones(Np,1),y_max);
Ymin = kron(ones(Np,1),y_min);
A_temp = zeros(Nc,Nc);
for i = 1:1:Nc
    for j = 1:1:Nc
        if j <= i
            A_temp(i,j) = 1;
        end
    end
end
A_temp = kron(A_temp,eye(Nu));


%% 模型预测控制
for i = 1:1:N
    %% 预测模型
    psi = Xout(i,3);
    u = Xout(i,4);
    v = Xout(i,5);
    r = Xout(i,6);
    a = [ 1.0,   0, -1.0*T*(v*cos(psi) + u*sin(psi)),                      T*cos(psi),                 -1.0*T*sin(psi),                                 0;
            0, 1.0,  T*(u*cos(psi) - 1.0*v*sin(psi)),                      T*sin(psi),                      T*cos(psi),                                 0;
            0,   0,                              1.0,                               0,                               0,                                 T;
            0,   0,                                0,                1.0 - 0.027907*T,                      1.3101*T*r,         T*(0.077519*r + 1.3101*v);
            0,   0,                                0, -1.0*T*(0.76079*r - 0.085434*v), T*(0.085434*u - 0.026292) + 1.0,     -1.0*T*(0.76079*u - 0.019393);
            0,   0,                                0,  -1.0*T*(0.085434*r + 2.8877*v),   -1.0*T*(2.8877*u + 0.0013242), 1.0 - 1.0*T*(0.085434*u + 0.6855)];
    b = [         0,           0;
                  0,           0;
                  0,           0;
          0.03876*T,           0;
                  0, -0.010679*T;
                  0,   0.36096*T];
    c = [1 0 0 0 0 0;0 1 0 0 0 0];
    a_piao = [a b;zeros(Nu,Ns) eye(Nu)];
    b_piao = [b;eye(Nu)];
    c_piao = [c zeros(Nx,Nu)];
    
    A_cell=cell(Np,1);
    B_cell=cell(Np,Nc);
    for j=1:1:Np
        A_cell{j,1}=c_piao*a_piao^j;
        for k=1:1:Nc
           if k<=j
                B_cell{j,k}=c_piao*(a_piao^(j-k))*b_piao;
           else
                B_cell{j,k}=zeros(Nx,Nu);
           end
        end
    end
    A=cell2mat(A_cell);
    B=cell2mat(B_cell);
    
    %% 滚动优化
    H=2*(B'*Q*B+R);
    if i == 1
        xi = [x_piao(i,:)';-tau_u;-tau_r];
        Ut = kron(ones(Nc,1),zeros(2,1));
    else
        xi = [x_piao(i,:)';u_piao(i-1,:)'];
        Ut = kron(ones(Nc,1),u_real(i-1,:)');
    end
    f=2*B'*Q*A*xi;
    A_cons=[A_temp;-A_temp;B;-B];
    Yref = zeros(2*Np,1);
    for z = 1:1:Np
        Yref((2*z-1):(2*z),1) = Xout(i,1:2)';
    end
    b_cons=[Umax-Ut;Ut-Umin;Ymax-Yref-A*xi;-Ymin+Yref+A*xi];
    lb=delta_Umin;
    ub=delta_Umax;
    [X,fval(i,1),exitflag(i,1),output(i,1)]=quadprog(H,f,A_cons,b_cons,[],[],lb,ub);
    delta_u_real(i,1:2)=X(1:2,1)';
    
    %% 反馈校正
    if i == 1
        u_real(i,1:2)=X(1:2,1)';
    else
        u_real(i,1:2)=u_real(i-1,1:2)+X(1:2,1)';
    end
    u_piao(i,:) = u_real(i,:) - [tau_u tau_r];   
    X00=x_real(i,:);
    [t,state] = ode45(@ship_model,[0,T],X00,[],u_real(i,:)');
    if i<N
        x_real(i+1,:) = state(end,:);
        x_piao(i+1,:) = x_real(i+1,:) - Xout(i+1,:);
    end

end


% %% 画图
% figure(1)
% plot(Xout(:,1),Xout(:,2),'b+',x_real(:,1),x_real(:,2),'r')
% axis equal



























