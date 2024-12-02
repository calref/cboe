@if-sdf = 0
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 2
@town-attitude = 1
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 2
@if-day = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 3, -1
	ex2 -1, -1, -1
	goto -1
@rect-place-field = 3
	sdf 100, 18
	msg -1, -1, -1
	pic 0, 4
	ex1 27, 40, -1
	ex2 27, 40, -1
	goto -1
@once-disp-msg = 4
	sdf 3, 1
	msg 1, 2, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 5
	sdf 3, 2
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 22, -1
	ex2 128, 7, -1
	goto -1
@if-spec-item = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 8, -1
	ex2 -1, -1, -1
	goto 9
@change-ter = 8
	sdf -1, -1
	msg 4, -1, -1
	pic 0, 4
	ex1 7, 22, -1
	ex2 125, -1, -1
	goto -1
@disp-msg = 9
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 10
	sdf 3, 3
	msg -1, -1, -1
	pic 27, 4
	ex1 3, 2, -1
	ex2 30, -1, -1
	goto -1
@once-give-spec-item = 11
	sdf 3, 4
	msg 6, -1, -1
	pic 0, 4
	ex1 7, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 12
	sdf 3, 6
	msg 7, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-day = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 14, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 14
	sdf 3, 6
	msg 8, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 15
	sdf 3, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 16, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 16
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 17
	sdf 100, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 20, -1
	ex2 -1, -1, -1
	goto 18
@town-visible = 18
	sdf -1, -1
	msg 10, 11, -1
	pic 0, 4
	ex1 7, 1, -1
	ex2 -1, -1, -1
	goto 19
@once-give-spec-item = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 0, -1
	ex2 -1, -1, -1
	goto 24
@if-sdf = 20
	sdf 1, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 22, -1
	ex2 -1, -1, -1
	goto 21
@disp-msg = 21
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 22
	sdf 3, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 23
@set-sdf = 23
	sdf 100, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 27
@set-sdf = 24
	sdf 100, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@buy-item-class = 25
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 50, -1, -1
	goto -1
@disp-msg = 26
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 0, -1
	ex2 -1, -1, -1
	goto -1
