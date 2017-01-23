@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 244, 1
	ex1 24, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 1
	sdf 210, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 2
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 212, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 4
@once-dlog = 4
	sdf -1, -1
	msg 7, -1, 1
	pic 73, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@set-sdf = 5
	sdf 212, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 7, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 7
	sdf -1, -1
	msg 13, -1, 1
	pic 73, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 8
@set-sdf = 8
	sdf 210, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 9
@spell-priest = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 58, 0, -1
	ex2 -1, -1, -1
	goto 10
@once-give-spec-item = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 11
	sdf -1, -1
	msg 19, -1, 1
	pic 73, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 12
	sdf 210, 2
	msg 25, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 13
	sdf 210, 0
	msg 31, -1, 1
	pic 49, 3
	ex1 22, 15, -1
	ex2 -1, -1, -1
	goto 14
@block-move = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-out-encounter = 15
	sdf -1, -1
	msg 37, 38, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 16
	sdf -1, -1
	msg 39, 40, -1
	pic 0, 4
	ex1 -1, 1000, -1
	ex2 200, -1, -1
	goto -1
@set-sdf = 17
	sdf 210, 0
	msg 41, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 18
	sdf 210, 3
	msg 42, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 19
	sdf 210, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 20
@once-dlog = 20
	sdf -1, -1
	msg 43, -1, 1
	pic 165, 3
	ex1 22, 22, -1
	ex2 -1, -1, -1
	goto 21
@disp-msg = 21
	sdf -1, -1
	msg 49, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 22
	sdf -1, -1
	msg 50, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 23
	sdf -1, -1
	msg 51, -1, 1
	pic 165, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
