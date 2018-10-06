
figure(1)
plot(Xout(:,1),Xout(:,2),'b+',x_real(:,1),x_real(:,2),'r')
xlabel('$x$(m)','interpreter','latex')
ylabel('$y$(m)','interpreter','latex')
legend('目标点','实船轨迹')
axis equal
hold on
axes('Position',[0.25,0.5,0.25,0.25]);
ly0=plot(Xout(:,1),Xout(:,2),'b+',x_real(:,1),x_real(:,2),'r');
xlim([2.4,2.52]);ylim([2.4,2.52]);
set(gca,'xtick',[2.4 2.44 2.48 2.52]);
set(gca,'ytick',[2.4 2.44 2.48 2.52]);
set(ly0,'linewidth',1);


t=T:T:N*T;
figure(2)
subplot(2,1,1)
plot(t,Xout(:,1),'b',t,x_real(:,1),'r')
xlabel('$t$(s)','interpreter','latex')
ylabel('$x$(m)','interpreter','latex')
legend('目标轨迹','实船轨迹')
hold on
axes('Position',[0.26,0.7,0.4,0.1]);
ly1=plot(t,Xout(:,1),'b',t,x_real(:,1),'r');
xlim([5,20]);ylim([2.4,2.5]);
set(gca,'xtick',[5 10 15 20]);
set(gca,'ytick',[2.38 2.42 2.46 2.5]);
set(ly1,'linewidth',1);
subplot(2,1,2)
plot(t,Xout(:,2),'b',t,x_real(:,2),'r')
xlabel('$t$(s)','interpreter','latex')
ylabel('$y$(m)','interpreter','latex')
legend('目标轨迹','实船轨迹')
hold on
axes('Position',[0.26,0.2,0.4,0.1]);
ly2=plot(t,Xout(:,2),'b',t,x_real(:,2),'r');
xlim([5,20]);ylim([2.4,2.5]);
set(gca,'xtick',[5 10 15 20]);
set(gca,'ytick',[2.38 2.42 2.46 2.5]);
set(ly2,'linewidth',1);

figure(3)
subplot(2,1,1)
stairs(t,u_real(:,1))
xlabel('$t$(s)','interpreter','latex')
ylabel('$\tau_u$(N)','interpreter','latex')
subplot(2,1,2)
stairs(t,u_real(:,2))
xlabel('$t$(s)','interpreter','latex')
ylabel('$\tau_r$(Nm)','interpreter','latex')

figure(4)
subplot(2,1,1)
stairs(t,delta_u_real(:,1))
xlabel('$t$(s)','interpreter','latex')
ylabel('$\Delta\tau_u$(N)','interpreter','latex')
subplot(2,1,2)
stairs(t,delta_u_real(:,2))
xlabel('$t$(s)','interpreter','latex')
ylabel('$\Delta\tau_r$(Nm)','interpreter','latex')