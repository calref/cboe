@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@if-sdf = 1
	sdf 14, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 -1, -1, -1
	goto 4
@nuke-monsts = 2
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 45, -1, -1
	ex2 -1, -1, -1
	goto 3
@nuke-monsts = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 47, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf 14, 1
	msg 1, -1, 1
	pic 123, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 5
	sdf -1, -1
	msg 7, -1, 1
	pic 181, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 6
	sdf 14, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@if-trait = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 8, -1
	ex2 1, 2, -1
	goto 9
@disp-msg = 8
	sdf -1, -1
	msg 13, 14, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 9
	sdf -1, -1
	msg 15, 16, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 10
	sdf 14, 3
	msg 17, -1, 1
	pic 75, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nuke-monsts = 11
	sdf -1, -1
	msg 23, 24, -1
	pic 0, 4
	ex1 45, -1, -1
	ex2 -1, -1, -1
	goto 12
@nuke-monsts = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 47, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 25, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 16, -1
	ex2 11, 0, -1
	goto 15
@disp-msg = 15
	sdf -1, -1
	msg 26, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-mage = 16
	sdf -1, -1
	msg 27, 28, -1
	pic 0, 4
	ex1 55, 0, -1
	ex2 -1, -1, -1
	goto 17
@spell-priest = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 61, 0, -1
	ex2 -1, -1, -1
	goto -1
