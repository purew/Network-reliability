 

%clf
%A = load result_symmetries;
%result = A

plot(A(:,2)/A(end,2),A(:,3), '-or')
title('The degeneracy for a circular 7-node network')
xlabel('\rho(l)')
ylabel('D - number of possible networks')

hold on
shg