@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-give-dlog = 1
	sdf 110, 0
	msg 0, -1, -1
	pic 78, 1
	ex1 365, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 2
	sdf 110, 1
	msg 6, -1, -1
	pic 78, 1
	ex1 367, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 3
	sdf 110, 2
	msg 12, -1, -1
	pic 78, 1
	ex1 369, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 4
	sdf 110, 3
	msg 12, -1, -1
	pic 78, 1
	ex1 370, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 5
	sdf 110, 4
	msg 12, -1, -1
	pic 78, 1
	ex1 368, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 6
	sdf 110, 5
	msg 12, -1, -1
	pic 78, 1
	ex1 366, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 7
	sdf 110, 6
	msg 12, -1, 1
	pic 78, 1
	ex1 366, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 8
	sdf 110, 7
	msg 12, -1, -1
	pic 78, 1
	ex1 368, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 9
	sdf 201, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 10
@once-dlog = 10
	sdf -1, -1
	msg 18, -1, 1
	pic 14, 3
	ex1 41, 12, -1
	ex2 48, 15, -1
	goto 11
@block-move = 11
	sdf -1, -1
	msg 24, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@if-gold = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 13, -1
	ex2 0, -1, -1
	goto 14
@gold = 13
	sdf -1, -1
	msg 25, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 14
	sdf -1, -1
	msg 26, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 15
	sdf 201, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 16
@once-out-encounter = 16
	sdf -1, -1
	msg 27, 28, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 17
	sdf -1, -1
	msg 29, -1, -1
	pic 0, 4
	ex1 497, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 18
	sdf -1, -1
	msg 30, 31, -1
	pic 0, 4
	ex1 331, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 19
	sdf 201, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 20
@once-dlog = 20
	sdf -1, -1
	msg 32, -1, 1
	pic 190, 1
	ex1 29, 22, -1
	ex2 -1, -1, -1
	goto 21
@block-move = 21
	sdf -1, -1
	msg 38, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 22
	sdf -1, -1
	msg 39, -1, 1
	pic 28, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 23
@once-out-encounter = 23
	sdf 201, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 24
	sdf 201, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 25
@once-dlog = 25
	sdf -1, -1
	msg 45, -1, 1
	pic 14, 3
	ex1 41, 27, -1
	ex2 48, 26, -1
	goto 30
@once-out-encounter = 26
	sdf 201, 3
	msg 51, 52, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-gold = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 28, -1
	ex2 0, -1, -1
	goto 29
@gold = 28
	sdf -1, -1
	msg 53, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 29
	sdf -1, -1
	msg 54, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 30
	sdf -1, -1
	msg 55, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
