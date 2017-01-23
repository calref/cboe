@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 17, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 17, 1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 17, 2
	msg 2, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 5
	sdf 17, 4
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 1, -1
	ex2 10, -1, -1
	goto -1
@if-ter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 38, -1
	ex2 0, 7, -1
	goto -1
@change-ter = 7
	sdf -1, -1
	msg 4, -1, -1
	pic 0, 4
	ex1 2, 38, -1
	ex2 97, -1, -1
	goto -1
@if-ter = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 38, -1
	ex2 0, 9, -1
	goto -1
@change-ter = 9
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 5, 38, -1
	ex2 97, -1, -1
	goto -1
@once-trap = 10
	sdf 17, 5
	msg -1, -1, -1
	pic 27, 4
	ex1 3, 1, -1
	ex2 40, -1, -1
	goto -1
