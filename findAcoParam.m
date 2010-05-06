

K = [10 20 30];
Ants = [10 20 30 40 50];

Rmax = load('Rmax.txt');
Rmean = load('Rmean.txt');
R = zeros(length(K),length(Ants),2);
R(:,:,1) = Rmean;
R(:,:,2) = Rmax;

title_str = {'R_{mean} for different parameters',...
	'R_{max} for different parameters'};
xlab = 'Number of ants';
ylab = 'R';
opt = {'-or', '-xb', '-+g'};

for r = 1:2
	figure
	clf
	hold on
	for i = 1:length(K)
		plot(Ants, R(i,:,r), opt{i})
	end
	plot(Ants, 0.8752*ones(1,length(Ants)), ':')
	title(title_str{r})
	xlabel(xlab)
	ylabel(ylab)
	
	leg = {};
	for k=K
		leg = [leg; strcat('K=',num2str(k)) ];
	end
	leg = [leg;'Optm'];
	legend( leg ,'Location', 'NorthWest')
end
shg
