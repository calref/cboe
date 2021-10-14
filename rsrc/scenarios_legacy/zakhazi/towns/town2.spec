@if-sdf = 0
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@town-attitude = 1
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
@disp-msg = 2
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 3
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 5, -1
	ex2 -1, -1, -1
	goto 4
@once-dlog = 5
	sdf -1, -1
	msg 2, -1, 1
	pic 45, 3
	ex1 22, 6, -1
	ex2 -1, -1, -1
	goto 8
@town-attitude = 6
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto 7
@set-sdf = 7
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 8
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@if-sdf-eq = 9
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 10, -1
	ex2 -1, -1, -1
	goto -1
@town-attitude = 10
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto 11
@set-sdf = 11
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 12
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 13, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 13
	sdf 2, 1
	msg 11, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 14
	sdf 2, 2
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 15
	sdf 211, 0
	msg 13, 14, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 16
	sdf 2, 3
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 25, -1, -1
	goto -1
@change-ter = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 23, -1
	ex2 10, -1, -1
	goto -1
@once-set-sdf = 18
	sdf 2, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 19
@move-party = 19
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 3, 31, -1
	ex2 0, -1, -1
	goto -1
@if-sdf-eq = 20
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 21, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 21
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 22
@town-attitude = 22
	sdf -1, -1
	msg 16, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
@once-disp-msg = 23
	sdf 2, 5
	msg 17, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 24
	sdf -1, -1
	msg 18, -1, 1
	pic 158, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 27, -1
	ex2 11, 0, -1
	goto 26
@disp-msg = 26
	sdf -1, -1
	msg 24, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-mage = 27
	sdf -1, -1
	msg 25, -1, -1
	pic 0, 4
	ex1 57, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 28
	sdf 211, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 29, -1
	goto -1
@once-dlog = 29
	sdf -1, -1
	msg 26, -1, 0
	pic 45, 3
	ex1 3, 30, -1
	ex2 2, 31, -1
	goto -1
@disp-msg = 30
	sdf -1, -1
	msg 32, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-gold = 31
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1200, 33, -1
	ex2 1, -1, -1
	goto 32
@disp-msg = 32
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 33
	sdf 211, 0
	msg 34, 35, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 34
	sdf 211, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 35, -1
	goto -1
@if-spec-item = 35
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 37, -1
	ex2 -1, -1, -1
	goto 36
@disp-msg = 36
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 37
	sdf -1, -1
	msg 37, -1, 1
	pic 45, 3
	ex1 39, 39, -1
	ex2 -1, -1, -1
	goto 38
@disp-msg = 38
	sdf -1, -1
	msg 43, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 39
	sdf 211, 0
	msg 44, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 40
@once-give-spec-item = 40
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 1, -1
	ex2 -1, -1, -1
	goto -1
