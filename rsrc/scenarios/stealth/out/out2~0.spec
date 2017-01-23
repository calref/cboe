@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 190, 1
	ex1 7, 1, -1
	ex2 -1, -1, -1
	goto 2
@start-shop = 1
	sdf -1, -1
	msg 17, -1, -1
	pic 0, 4
	ex1 32, 5, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 202, 0
	msg 6, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 202, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 4
	sdf 202, 1
	msg 7, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 5
	sdf 101, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 6, -1
	goto -1
@block-move = 6
	sdf -1, -1
	msg 8, 9, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 7
	sdf 202, 3
	msg 10, -1, 1
	pic 19, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 8
	sdf -1, -1
	msg 16, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
