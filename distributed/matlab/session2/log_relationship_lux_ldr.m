% function finds the relation between arduino LDR input vs LUX

% constants
r2 = 10000; % resistor seriell with the LDR
vin = 5; % voltage over LDR
vout = sort([0.30 0.98 1.24 1.65 2.52 2.91 3.15 3.48]); % values recorded using multimeter
anread = sort([67 224 260 350 530 640 672 740]); % values arduino shows when displaying LDR values
lux = sort([10 20 30 100 200 290 410 570]); % values recorded using luxometer

% preallocated resistance list
resistance = 1:1:length(vout);

% loops through preallocated list to calculate resistance
for i=1:length(vout)
    resistance(i)= r2.*(vin/vout(i)-1);
end

% polynomial that describes the linear logarithmic relationship between lux
% and anread 
coefficients_lux_resistance = polyfit(log10(lux), log10(resistance), 1);

% coefficients_lux_resistance contains the coefficients needed to
% represents the regression line i a loglog scale with resistance on y-axis
% and lux on x-axis. 

% finds the linear regression between anread and resistance. 
coefficients_anread_resistance = polyfit(anread, log10(resistance), 1);

% finds the function between input anread and output resistance
resistance_out = @(anread) coefficients_anread_resistance(1)*anread + coefficients_anread_resistance(2);

% finds the function between input lux and output resistance
resistance_out_2 = @(lux) coefficients_lux_resistance(1)*lux + coefficients_lux_resistance(2);

% finds the function between input anread and output lux
lux_out = @(anread) (coefficients_anread_resistance(1)*anread + coefficients_anread_resistance(2) - coefficients_lux_resistance(2))/coefficients_lux_resistance(1);

disp(coefficients_anread_resistance(1));
disp(coefficients_anread_resistance(2));
disp(coefficients_lux_resistance(2));
disp(coefficients_lux_resistance(1));

% plots anread vs lux
figure(2);
semilogy(anread, 10.^lux_out(anread));
hold on;
semilogy(anread, lux, '.');

% % best fit line for the polynomial regression line between anread and
% % resistance 
% values_regression_anread_resistance = [polyval(coefficients_anread_resistance, anread)];
% % end

% % plots the linear regression and the values in a loglog scale
% values_regression_lux_resistance = polyval(coefficients_lux_resistance, log10(lux));% best fit line for the polynomial regression line between lux and
% % resistance
% loglog(lux, 10.^values_regrxession_lux_resistance_2, '.');
% hold on;
% loglog(lux, 10.^values_regression_lux_resistance);
% % end plot 

% % plots the linear regression and the values in a semilog scale
% figure(1)
% semilogy(anread, resistance, '.');

% % best fit line for the polynomial regression line between lux and
% % resistance
% values_regression_lux_resistance = [polyval(coefficients_lux_resistance, log10(lux))];
% f = @(x) coefficients_lux_resistance[0]*x + coefficients_lux_resistance[1];
% values_regression_lux_resistance_2 = f(log10(lux));
% % end best fit

% grid on;
% hold on;
% semilogy(anread, 10.^values_regression_anread_resistance); 
% 
% % end plot

% % plots the linear regression and the values in a loglog scale
% loglog(lux, resistance, '.');
% hold on;
% loglog(lux, 10.^values_regression_lux_resistance);
% % end plot

% % checks if anread and resistance has a linear relationship in a
% % semilogscale. Use the plots to evaluate the relationship
% figure(1)
% hold off;
% loglog(anread, resistance);
% grid on;
% 
% 
% figure(2)
% plot(anread, resistance);
% grid on;
% 
% figure(3)
% semilogy(anread, resistance);
% grid on;
% 
% % end check


