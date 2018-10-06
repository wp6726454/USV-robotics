function y=fB(x)

lx=[20;20;-20;-20];
ly=[5;-5;5;-5];

    b31=-ly(1)*cos(x(1))+lx(1)*sin(x(1));
    b32=-ly(2)*cos(x(2))+lx(2)*sin(x(2));
    b33=-ly(3)*cos(x(3))+lx(3)*sin(x(3));
    b34=-ly(4)*cos(x(4))+lx(4)*sin(x(4));
    
    B=[cos(x)';sin(x)';b31,b32,b33,b34];
    y=B;
end