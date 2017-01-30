@once-dlog = 0
	sdf 10, 1
	msg 0, -1, 1
	pic 76, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf 10, 2
	msg 6, -1, 1
	pic 76, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 2
@set-sdf = 2
	sdf 199, 1
	msg 12, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 10, 3
	msg 13, 14, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf 10, 4
	msg 15, -1, 1
	pic 74, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 21
@once-dlog = 5
	sdf -1, -1
	msg 21, -1, 1
	pic 125, 1
	ex1 51, 7, -1
	ex2 -1, -1, -1
	goto 6
@block-move = 6
	sdf -1, -1
	msg 27, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@if-sdf = 7
	sdf 10, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 9, -1
	ex2 -1, -1, -1
	goto 8
@once-dlog = 8
	sdf -1, -1
	msg 28, -1, 1
	pic 125, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 34, -1, 1
	pic 125, 1
	ex1 36, 12, -1
	ex2 -1, -1, -1
	goto 10
@block-move = 10
	sdf -1, -1
	msg 40, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto 11
@nop = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 14, -1
	ex2 -1, -1, -1
	goto 13
@block-move = 13
	sdf -1, -1
	msg 41, 42, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 14
	sdf -1, -1
	msg 43, -1, 1
	pic 125, 1
	ex1 45, 16, -1
	ex2 -1, -1, -1
	goto 15
@block-move = 15
	sdf -1, -1
	msg 49, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 16
	sdf -1, -1
	msg 50, -1, 1
	pic 125, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 17
@set-sdf = 17
	sdf 1, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 18
@rect-place-field = 18
	sdf 100, 15
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 24, -1
	ex2 18, 24, -1
	goto 19
@rect-place-field = 19
	sdf 100, 15
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 27, -1
	ex2 24, 27, -1
	goto 20
@rect-place-field = 20
	sdf 100, 15
	msg -1, -1, -1
	pic 0, 4
	ex1 29, 22, -1
	ex2 29, 22, -1
	goto -1
@once-town-encounter = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 22
	sdf 10, 5
	msg 56, -1, 1
	pic 138, 1
	ex1 49, 23, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 23
	sdf -1, -1
	msg 62, -1, -1
	pic 0, 4
	ex1 8, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 24
	sdf 10, 6
	msg 63, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 25
	sdf -1, -1
	msg 64, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@status = 26
	sdf -1, -1
	msg 65, -1, -1
	pic 0, 4
	ex1 2, 1, 7
	ex2 -1, -1, -1
	goto -1
@if-sdf = 27
	sdf 1, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 30, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 28
	sdf -1, -1
	msg 66, -1, 1
	pic 87, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 29
@stair = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 34, 34, -1
	ex2 1, 1, -1
	goto -1
@if-context = 30
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 28
