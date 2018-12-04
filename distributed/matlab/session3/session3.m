% calculates the time_constant of values. Time used on each iteration is
% defined by interval_time. Calculates static gain as well. 
% assumptions: Each iteration uses the same amount of time. 
% the code assumes that the data has reached steady state. 

% constants
interval_time = 30; % amount of time used on each iteration

% values from response
values = [250.00 248.00 249.00 243.00 241.00 234.00 232.00 225.00 222.00 215.00 212.00 206.00 203.00 196.00 192.00 190.00 183.00 181.00 175.00 174.00 168.00 166.00 161.00 159.00 154.00 153.00 148.00 147.00 142.00 139.00 138.00 134.00 133.00 129.00 128.00 125.00 124.00 120.00 121.00 116.00 117.00 113.00 113.00 109.00 108.00 108.00 104.00 106.00 102.00 102.00 99.00 100.00 97.00 97.00 94.00 95.00 92.00 93.00 90.00 89.00 90.00 87.00 88.00 85.00 86.00 84.00 85.00 82.00 83.00 81.00 80.00 82.00 79.00 80.00 78.00 79.00 76.00 78.00 76.00 77.00 74.00 76.00 73.00 75.00 72.00 73.00 74.00 71.00 73.00 71.00 72.00 70.00 72.00 69.00 71.00 69.00 70.00 69.00 70.00 68.00 68.00 69.00 67.00 68.00 66.00 68.00 66.00 68.00 65.00 68.00 65.00 65.00 67.00 64.00 66.00 64.00 66.00 64.00 66.00 64.00 66.00 64.00 66.00 64.00 65.00 63.00 63.00 64.00 63.00 64.00 63.00 64.00 62.00 64.00 62.00 63.00 62.00 64.00 62.00 64.00 62.00 63.00 62.00 63.00 61.00 64.00 61.00 63.00 61.00 63.00 61.00 63.00 61.00 62.00 61.00 63.00 61.00 63.00 61.00 62.00 61.00 62.00 61.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 60.00 62.00 59.00 62.00 60.00 61.00 59.00 62.00 59.00 61.00 59.00 61.00 59.00 61.00 60.00 61.00 60.00 61.00 59.00 61.00 59.00 61.00 59.00 62.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 60.00 59.00 60.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 60.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 61.00 59.00 61.00 58.00 61.00 58.00 61.00 59.00 61.00 59.00 60.00 59.00 60.00 59.00 60.00 59.00 60.00 59.00 61.00 58.00 61.00 59.00 61.00 59.00 61.00 58.00 61.00 59.00 60.00 59.00 60.00 59.00 60.00 59.00 61.00 59.00 60.00 59.00 60.00 59.00 60.00 59.00 60.00 59.00 60.00 59.00 60.00 58.00 61.00 58.00 60.00 59.00 60.00 59.00 60.00 59.00 60.00 58.00 60.00 ];

% calculate lux values of values
values = 10.^((-0.0021.*values-0.6316)./(-0.7757));

% plots
x = 1:1:length(values);
plot(x, values);


% time_constant
time_constant_y_value = values(1)+ (values(length(values))-values(1))*0.63;

% find the time where values is closest to time_constant_y_value
prev_value = values(1);
for i = 1:1:length(values)
    if (prev_value < time_constant_y_value && time_constant_y_value < values(i))
        % time_constant_y_value is between last and this value. 
        % set this time i to be the time constant
        time_constant = i;
     
        
    elseif values(i) == time_constant_y_value
        % time_constant_y_value was equal to a value in values
        % we set this time i to be the time constant
        time_constant = i;
    
        
    end
    prev_value = values(i);
end
 

% gain

change_lux = values(length(values)) - values(1);
static_gain = change_lux/change_u;

disp('static_gain:');
disp(static_gain);
disp('time_constant:');
disp(time_constant*interval_time/(16*10^6));


