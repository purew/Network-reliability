clf

R = load('data/percolation.plot');
% Format: x, p, reliability

lenP = length(R(1,:));
lenX = length(R(:,1));

P = 0:1/lenP:1;
P = P(1:end-1);
X = 0:1/lenX:1;
X = X(1:end-1);



hold on 

title('Percolation problem')
xlabel('p')
ylabel('x')
zlabel('R(p,x)')
surf(P,X,R)
shading faceted;
%shading interp;
%shading flat;

shg


% P = temp(:,1);
% X = temp(:,2);
% R = temp(:,3);
% 
% Plen = round( P( length(P) )/P(2)+1 );
% %Xlen = X( length(X) )/X(Plen+1);
% 
% Pn = 0:P(2):P(length(P));
% Xn = 0:X(Plen+1):X(length(X));
% 
% [Pm, Xm] = meshgrid(Pn,Xn);
% 
% for i = P(1:Plen);
% 	
% 	plot3(P,X,R);
% 	%shg
% 	
