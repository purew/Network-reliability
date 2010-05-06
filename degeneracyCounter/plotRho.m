 

%clf
%A = load result_symmetries;
%result = A
opt = '-r';

plotdim = [1 3];
subplot(plotdim(1),plotdim(2),1)
plot(A(:,2)/A(end,2),A(:,3), opt)
%title('The degeneracy for a circular 7-node network')
xlabel('\rho(l)')
ylabel('D - number of possible networks')


subplot(plotdim(1),plotdim(2),2)
plot(A(:,2)/A(end,2),A(:,4), opt)
xlabel('\rho(l)')
ylabel('max(R) - The highest reliability')

subplot(plotdim(1),plotdim(2),3)
plot(A(2:end,2)/A(end,2),diff(A(:,4)), opt)
xlabel('\rho(l)')
ylabel('D(max(R)) - The change of R')

%hold on
shg