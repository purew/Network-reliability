clf

R = load('data/percolation.plot');
% Format: x, p, reliability

lenP = length(R(1,:));
lenX = length(R(:,1));

P = 0:1/lenP:1;
P = P(1:end-1);
X = 0:1/lenX:1;
X = X(1:end-1)*lenX;



hold on 

%title('Percolation problem')
xlabel('p')
ylabel('x*N')
zlabel('R(p,x)')
surf(P,X,R)
shading faceted;

view(260,20)
shg


