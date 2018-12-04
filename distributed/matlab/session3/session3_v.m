% method finds a relation between timeconstant and static gain.

% gain up
static_gain_up = [0.6360 0.7573 0.8480 0.9074 0.9574];
end_lux_up = [10 20 30 40 47];

% gain down
static_gain_down = [-1.2843 -1.1876 -1.1181 -1.0349 -1.0076];
end_lux_down = [40 30 20 10 5];


plot(static_gain_up, end_lux_up, 'o');
hold on;


figure(2);
plot(static_gain_down, end_lux_down, 'x');
hold on;


% finds a linear relationship
B_up = polyfit(static_gain_up, end_lux_up, 1);
B_down = polyfit(static_gain_down, end_lux_down, 1); 


% function
f_up = @(x) B_up(1)*x + B_up(2);
f_down = @(x) B_down(1)*x + B_down(2);

% plots
figure(1)
plot([min(static_gain_up) max(static_gain_up)], [f_up(min(static_gain_up)) f_up(max(static_gain_up))]);

figure(2)
plot([min(static_gain_down) max(static_gain_down)], [f_down(min(static_gain_down)) f_down(max(static_gain_down))]);


% disps
disp('UP:')
disp(B_up);
disp('DOWNS:')
disp(B_down);