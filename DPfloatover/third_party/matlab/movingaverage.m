
yaw=Position_second(:,6);
length_=length(yaw);
num=100;
average_yaw=zeros(length_-num,1);
for i=1:(length_-num)
    temp=0;
    for j=1:num
        temp=temp+yaw(i+j-1);
    end
    average_yaw(i)=temp/num;
end
figure(1)
plot(yaw);hold on;
plot(average_yaw);

figure(2)
plot(diff(average_yaw));
xlim([30000 50000])