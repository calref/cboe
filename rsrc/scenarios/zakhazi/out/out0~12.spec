@once-out-encounter = 0
	sdf 212, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 212, 1
	msg 1, 2, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 2
	sdf 212, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 3, -1
	ex2 -1, -1, -1
	goto 4
@disp-msg = 3
	sdf -1, -1
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 4
	sdf 212, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 5, -1
	ex2 -1, -1, -1
	goto 8
@once-dlog = 5
	sdf -1, -1
	msg 4, -1, 1
	pic 244, 1
	ex1 8, 7, -1
	ex2 -1, -1, -1
	goto 6
@block-move = 6
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-out-encounter = 7
	sdf 212, 3
	msg 11, 12, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf -1, -1
	msg 13, -1, 1
	pic 244, 1
	ex1 8, 9, -1
	ex2 -1, -1, -1
	goto 6
@once-out-encounter = 9
	sdf 212, 2
	msg 19, 20, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 10
@block-move = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-give-item = 11
	sdf -1, -1
	msg 21, 22, -1
	pic 0, 4
	ex1 323, -1, -1
	ex2 200, -1, -1
	goto -1
@if-sdf = 12
	sdf 212, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 14, -1
	ex2 -1, -1, -1
	goto 13
@disp-msg = 13
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 14
	sdf 212, 5
	msg 24, -1, -1
	pic 0, 4
	ex1 6, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 15
	sdf 212, 6
	msg 25, -1, -1
	pic 2, 4
	ex1 338, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 16
	sdf 212, 7
	msg 31, -1, -1
	pic 0, 4
	ex1 363, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 17
	sdf 212, 8
	msg 32, -1, -1
	pic 0, 4
	ex1 364, -1, -1
	ex2 -1, -1, -1
	goto -1
