@once-disp-msg = 0
	sdf 7, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 21, 17, -1
	ex2 147, 2, -1
	goto -1
@if-spec-item = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 4, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 3
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 4
	sdf -1, -1
	msg 2, -1, -1
	pic 0, 4
	ex1 21, 14, -1
	ex2 148, -1, -1
	goto 5
@change-ter = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 22, 14, -1
	ex2 148, -1, -1
	goto 6
@change-ter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 21, 17, -1
	ex2 148, -1, -1
	goto 7
@change-ter = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 22, 17, -1
	ex2 148, -1, -1
	goto -1
@if-ter = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 17, 15, -1
	ex2 147, 9, -1
	goto -1
@change-ter = 9
	sdf -1, -1
	msg 3, -1, -1
	pic 0, 4
	ex1 17, 15, -1
	ex2 148, -1, -1
	goto 10
@change-ter = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 17, 16, -1
	ex2 148, -1, -1
	goto -1
@once-dlog = 11
	sdf -1, -1
	msg 4, -1, 1
	pic 79, 1
	ex1 33, 12, -1
	ex2 -1, -1, -1
	goto -1
@rest = 12
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 250, 200, -1
	ex2 -1, -1, -1
	goto -1
