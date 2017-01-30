@disp-msg = 0
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 1
	sdf 201, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 2
	sdf 201, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 3
@once-dlog = 3
	sdf -1, -1
	msg 2, -1, 1
	pic 17, 3
	ex1 41, 6, -1
	ex2 22, 10, -1
	goto 4
@block-move = 4
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@block-move = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-set-sdf = 6
	sdf 201, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@if-gold = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 8, -1
	ex2 0, -1, -1
	goto 9
@gold = 8
	sdf -1, -1
	msg 9, 10, -1
	pic 0, 4
	ex1 100, 1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 9
	sdf -1, -1
	msg 11, 10, -1
	pic 0, 4
	ex1 100, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 10
	sdf 201, 0
	msg 12, 13, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 11
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 12
@food = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
