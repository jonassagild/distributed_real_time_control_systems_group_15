% method finds a relation between timeconstant and lux.

% constants up
time_const_up = [1.6875e-06 1.8125e-06 1.8750e-06 1.9375e-06 2.0000e-06];
end_lux_up = [10 20 30 40 47];

% const down
time_const_down = [5.6250e-07 5.6250e-07 5.6250e-07 5.0000e-07 5.0000e-07];
end_lux_down = [40 30 20 10 5];


plot(time_const_up, end_lux_up, 'o');
hold on;


figure(2);
plot(time_const_down, end_lux_down, 'x');
hold on;


% finds a linear relationship
B_up = polyfit(time_const_up, end_lux_up, 1);
B_down = polyfit(time_const_down, end_lux_down, 1); 


% function
f_up = @(x) B_up(1)*x + B_up(2);
f_down = @(x) B_down(1)*x + B_down(2);

% plots
figure(1)
plot([min(time_const_up) max(time_const_up)], [f_up(min(time_const_up)) f_up(max(time_const_up))]);

figure(2)
plot([min(time_const_down) max(time_const_down)], [f_down(min(time_const_down)) f_down(max(time_const_down))]);


% disps
% disps
disp('UP:')
disp(B_up);
disp('DOWNS:')
disp(B_down);