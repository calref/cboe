@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-give-dlog = 1
	sdf -1, -1
	msg 0, -1, -1
	pic 2, 4
	ex1 388, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 207, 1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 7, -1, 1
	pic 31, 4
	ex1 24, 4, -1
	ex2 -1, -1, -1
	goto -1
@out-move-party = 4
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 15, 22, -1
	ex2 -1, -1, -1
	goto 5
@change-time = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 800, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 6
	sdf -1, -1
	msg 7, -1, 1
	pic 31, 4
	ex1 24, 7, -1
	ex2 -1, -1, -1
	goto -1
@out-move-party = 7
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 38, 22, -1
	ex2 -1, -1, -1
	goto 8
@change-time = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 800, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf 207, 2
	msg 14, -1, 1
	pic 240, 1
	ex1 8, 10, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 10
	sdf -1, -1
	msg 20, -1, 1
	pic 105, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 11
@once-out-encounter = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 12
	sdf -1, -1
	msg 26, 27, -1
	pic 0, 4
	ex1 209, 500, -1
	ex2 150, -1, -1
	goto 13
@once-give-spec-item = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 14
	sdf 207, 3
	msg 28, 29, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 15
	sdf 207, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 16
@once-dlog = 16
	sdf -1, -1
	msg 30, -1, 1
	pic 60, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 17
@if-response = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 6, 4
	ex1 20, 19, -1
	ex2 -1, -1, -1
	goto 18
@once-out-encounter = 18
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 19
	sdf -1, -1
	msg 37, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
