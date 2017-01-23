@once-trap = 0
	sdf 4, 4
	msg -1, -1, -1
	pic 27, 4
	ex1 3, 2, -1
	ex2 20, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 4, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 2
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 4, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 5, -1
	ex2 -1, -1, -1
	goto 4
@disp-msg = 4
	sdf -1, -1
	msg 2, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 5
	sdf 4, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto 6
@once-give-spec-item = 6
	sdf 4, 3
	msg 3, 4, -1
	pic 0, 4
	ex1 6, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 7
	sdf 4, 5
	msg 7, -1, -1
	pic 77, 1
	ex1 207, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 8
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 9
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 10
	sdf 4, 1
	msg 5, 6, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
