@if-spec-item = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto 2
@once-dlog = 2
	sdf -1, -1
	msg 0, -1, 0
	pic 62, 3
	ex1 2, 3, -1
	ex2 3, 4, -1
	goto -1
@disp-msg = 3
	sdf -1, -1
	msg 6, 7, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 8, -1, 0
	pic 62, 3
	ex1 2, 5, -1
	ex2 3, -1, -1
	goto -1
@once-give-spec-item = 5
	sdf -1, -1
	msg 14, 15, -1
	pic 0, 4
	ex1 11, 0, -1
	ex2 -1, -1, -1
	goto 6
@set-sdf = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 7
	sdf 9, 2
	msg 16, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 8
	sdf 9, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 9, -1
	ex2 -1, -1, -1
	goto 16
@if-spec-item = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 10, -1
	ex2 -1, -1, -1
	goto 15
@once-dlog = 10
	sdf -1, -1
	msg 17, -1, 0
	pic 185, 1
	ex1 2, 11, -1
	ex2 3, 14, -1
	goto -1
@once-give-spec-item = 11
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 12, 1, -1
	ex2 -1, -1, -1
	goto 12
@hp = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 0, -1
	ex2 -1, -1, -1
	goto 13
@set-sdf = 13
	sdf 9, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@status = 14
	sdf -1, -1
	msg 24, -1, -1
	pic 0, 4
	ex1 4, 1, 2
	ex2 -1, -1, -1
	goto -1
@disp-msg = 15
	sdf -1, -1
	msg 25, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 16
	sdf -1, -1
	msg 26, -1, 12
	pic 185, 1
	ex1 -1, -1, -1
	ex2 -1, 17, -1
	goto -1
@set-sdf = 17
	sdf 9, 4
	msg 32, 33, -1
	pic 0, 4
	ex1 1, 0, -1
	ex2 -1, -1, -1
	goto 18
@status = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 1, 2
	ex2 -1, -1, -1
	goto -1
@once-dlog = 19
	sdf -1, -1
	msg 34, -1, 1
	pic 75, 1
	ex1 28, 20, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 20
	sdf 9, 0
	msg 40, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 21
@start-timer-town = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 80, 22, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 22
	sdf 9, 0
	msg 41, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 23
	sdf 9, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 25, -1
	ex2 -1, -1, -1
	goto 24
@block-move = 24
	sdf -1, -1
	msg 44, 45, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 25
	sdf -1, -1
	msg 42, 43, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 31, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 27
	sdf -1, -1
	msg 46, 47, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 28
@status = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 1, 2
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 30, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 30
	sdf 9, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 31
	sdf 9, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 27
@if-spec-item = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 33, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 33
	sdf 9, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 34
	sdf 8, 6
	msg 48, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-trait = 35
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 36, -1
	ex2 1, 2, -1
	goto -1
@once-give-dlog = 36
	sdf 9, 7
	msg 49, -1, -1
	pic 73, 1
	ex1 391, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 38, -1
	ex2 -1, -1, -1
	goto -1
@status = 38
	sdf -1, -1
	msg 55, 56, -1
	pic 0, 4
	ex1 3, 1, 2
	ex2 -1, -1, -1
	goto -1
