@once-dlog = 0
	sdf 18, 0
	msg 0, -1, 1
	pic 12, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 1
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 2
	sdf -1, -1
	msg 7, 8, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 3
	sdf 18, 1
	msg 9, -1, 1
	pic 1000, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 29
@if-statistic = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 6, -1
	ex2 11, 0, -1
	goto 5
@disp-msg = 5
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-priest = 6
	sdf -1, -1
	msg 16, 17, -1
	pic 0, 4
	ex1 61, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 8, -1
	ex2 11, 0, -1
	goto 5
@spell-priest = 8
	sdf -1, -1
	msg 18, 19, -1
	pic 0, 4
	ex1 54, 0, -1
	ex2 -1, -1, -1
	goto -1
@hp = 9
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 50, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 12, -1
	ex2 -1, -1, -1
	goto 11
@disp-msg = 11
	sdf -1, -1
	msg 21, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 12
	sdf 18, 3
	msg 22, -1, 17
	pic 138, 1
	ex1 386, 500, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 13
	sdf -1, -1
	msg 28, -1, -1
	pic 0, 4
	ex1 17, 0, -1
	ex2 -1, -1, -1
	goto 14
@set-sdf = 14
	sdf 18, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 15
	sdf 18, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 16, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 16
	sdf -1, -1
	msg 29, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 17
@death = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 128, 19, -1
	goto -1
@if-spec-item = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 17, 21, -1
	ex2 -1, -1, -1
	goto 20
@disp-msg = 20
	sdf -1, -1
	msg 30, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 21
	sdf -1, -1
	msg 31, -1, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 125, -1, -1
	goto -1
@if-ter = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 128, 23, -1
	goto -1
@disp-msg = 23
	sdf -1, -1
	msg 32, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 24
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 25, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 25
	sdf -1, -1
	msg 33, -1, 1
	pic 1000, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 26
@disp-msg = 26
	sdf -1, -1
	msg 39, 40, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 28
@button-generic = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 26, 29, -1
	ex2 19, 1, -1
	goto -1
@town-visible = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 1, -1
	ex2 -1, -1, -1
	goto -1
@town-visible = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 30
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 31, -1
	ex2 -1, -1, -1
	goto 3
@once-dlog = 31
	sdf 18, 5
	msg 41, -1, 1
	pic 1000, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
