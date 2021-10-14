@once-dlog = 0
	sdf 16, 4
	msg 0, -1, 1
	pic 78, 3
	ex1 29, 3, -1
	ex2 -1, -1, -1
	goto 14
@if-sdf = 1
	sdf 16, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 3, -1
	ex2 -1, -1, -1
	goto 0
@once-disp-msg = 3
	sdf 16, 2
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 7, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@xp = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 6
	sdf 16, 5
	msg 8, -1, -1
	pic 27, 4
	ex1 4, 3, -1
	ex2 80, -1, -1
	goto -1
@button-generic = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 14, 15, -1
	ex2 15, 1, -1
	goto -1
@button-generic = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 19, -1
	ex2 15, 1, -1
	goto -1
@button-generic = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 11, -1
	ex2 15, 1, -1
	goto -1
@once-set-sdf = 10
	sdf 16, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 11
@damage = 11
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 7, 12, -1
	ex2 5, 1, -1
	goto -1
@once-set-sdf = 12
	sdf 16, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@damage = 13
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 7, 12, -1
	ex2 8, 1, -1
	goto -1
@block-move = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
