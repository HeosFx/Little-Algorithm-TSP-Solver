param N; # number of cities

set Cities := 1..N; # set of cities

param x_coord{Cities}; # x-coordinates of cities
param y_coord{Cities}; # y-coordinates of cities

# distance between two cities
param dist{i in Cities, j in Cities} := if sqrt((x_coord[i]-x_coord[j])^2 + (y_coord[i]-y_coord[j])^2) > 0 then sqrt((x_coord[i]-x_coord[j])^2 + (y_coord[i]-y_coord[j])^2) else 9999999999999;

var x{Cities, Cities} binary; # 1 if path (i,j) is taken, 0 otherwise
var u{Cities} >= 0; # tour length

# minimize tour length
minimize tour_length: sum{i in Cities, j in Cities} dist[i,j] * x[i,j];

# each city is visited exactly once
s.t. visit_once_in{i in Cities}: sum{j in Cities} x[i,j] = 1;
s.t. visit_once_out{i in Cities}: sum{j in Cities} x[j,i] = 1;

# subtour elimination
s.t. subtour{i in Cities, j in Cities: i != j and i > 1 and j > 1}: u[i] - u[j] + N * x[i,j] <= N-1;

solve;

# print tour length
display tour_length;

data;

param N := 10;

param x_coord :=
	1 565
	2 25
	3 345
	4 945
	5 845
	6 880 
	7 25
	8 525
	9 580
	10 650;

param y_coord :=
	1 575
	2 185
	3 750
	4 685
	5 655
	6 660 
	7 230
	8 1000
	9 1175
	10 1130;

end;
