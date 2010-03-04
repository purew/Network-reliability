clf

load temp

X = 0:1/(length(temp(:,1))-1):1;
P = 0:1/(length(temp(1,:))-1):1;

surf(P,X,temp);
%shading faceted;
%shading interp;
shading flat;
title('Percolation problem')
xlabel('p')
 ylabel('x')
 zlabel('R(p,x)')
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
