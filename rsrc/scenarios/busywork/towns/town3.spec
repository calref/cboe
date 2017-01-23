@once-dlog = 0
	sdf 3, 0
	msg 0, -1, 1
	pic 193, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 3, 1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-trait = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 3, -1
	ex2 1, 2, -1
	goto -1
@once-disp-msg = 3
	sdf 3, 2
	msg 7, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 34, 15, -1
	ex2 9, 5, -1
	goto -1
@change-ter = 5
	sdf -1, -1
	msg 8, 9, -1
	pic 0, 4
	ex1 34, 15, -1
	ex2 10, -1, -1
	goto -1
@once-disp-msg = 6
	sdf 3, 3
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 7
	sdf -1, -1
	msg 11, -1, -1
	pic 0, 4
	ex1 26, 20, -1
	ex2 170, -1, -1
	goto -1
@once-trap = 8
	sdf 3, 4
	msg -1, -1, -1
	pic 27, 4
	ex1 3, 2, -1
	ex2 25, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 12, -1, 1
	pic 13, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 10
@xp = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto 11
@set-sdf = 11
	sdf 100, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
