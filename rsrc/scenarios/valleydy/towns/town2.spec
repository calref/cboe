@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 2, 1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 2, 1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 3
	sdf 2, 2
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 2, -1
	ex2 15, -1, -1
	goto -1
@disp-sm-msg = 4
	sdf -1, -1
	msg 2, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 5
	sdf -1, -1
	msg 3, 4, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 6
	sdf 2, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 7, -1
	ex2 -1, -1, -1
	goto -1
@nuke-monsts = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 58, -1, -1
	ex2 -1, -1, -1
	goto 8
@nuke-monsts = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 60, -1, -1
	ex2 -1, -1, -1
	goto 9
@nuke-monsts = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 62, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 2, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 11, -1
	ex2 -1, -1, -1
	goto 12
@disp-msg = 11
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 12
	sdf 2, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 13, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 13
	sdf 2, 4
	msg 6, 7, -1
	pic 0, 4
	ex1 71, -1, -1
	ex2 -1, -1, -1
	goto -1
