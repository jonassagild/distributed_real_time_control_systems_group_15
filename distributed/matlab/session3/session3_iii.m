% method finds a relation between voltage and lux read from ldr

% constants
pwm = [50 100 150 200 250];
lux = [11.2 17.46 27.35 36.88 47.63];


plot(pwm, lux, '.');

% finds a linear relationship
B = polyfit(pwm, lux, 1);


% function
f = @(x) B(1)*x + B(2);

fplot(f);

% disps
disp(B);