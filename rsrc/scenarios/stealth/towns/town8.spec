@once-disp-msg = 0
	sdf 8, 1
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 8, 2
	msg 2, 3, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@damage = 2
	sdf -1, -1
	msg 4, -1, -1
	pic 0, 4
	ex1 3, 6, -1
	ex2 0, 5, -1
	goto -1
@once-dlog = 3
	sdf 8, 3
	msg 5, -1, 1
	pic 8, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 11, -1, 1
	pic 8, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 5
	sdf 8, 5
	msg 17, -1, 1
	pic 8, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 6
	sdf 8, 6
	msg 23, -1, 1
	pic 8, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf 8, 7
	msg 29, -1, 1
	pic 75, 1
	ex1 28, 8, -1
	ex2 -1, -1, -1
	goto -1
@hp = 8
	sdf -1, -1
	msg 35, 36, -1
	pic 0, 4
	ex1 200, 0, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-trap = 10
	sdf 8, 8
	msg -1, -1, -1
	pic 27, 4
	ex1 2, 2, -1
	ex2 39, -1, -1
	goto -1
@once-set-sdf = 11
	sdf 8, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 12
@once-dlog = 12
	sdf -1, -1
	msg 37, -1, 0
	pic 105, 3
	ex1 22, 13, -1
	ex2 38, 14, -1
	goto -1
@once-town-encounter = 13
	sdf -1, -1
	msg 43, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf -1, -1
	msg 44, -1, 0
	pic 105, 3
	ex1 3, 13, -1
	ex2 2, 15, -1
	goto -1
@once-dlog = 15
	sdf -1, -1
	msg 50, -1, 0
	pic 105, 3
	ex1 3, 13, -1
	ex2 2, 16, -1
	goto -1
@once-dlog = 16
	sdf -1, -1
	msg 56, -1, 1
	pic 105, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 17
@set-sdf = 17
	sdf 8, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 18
@once-give-spec-item = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 0, -1
	ex2 0, -1, -1
	goto 19
@change-ter = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 7, -1
	ex2 130, -1, -1
	goto 20
@move-party = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 7, -1
	ex2 0, -1, -1
	goto 27
@once-give-spec-item = 21
	sdf 7, 9
	msg 62, 63, -1
	pic 0, 4
	ex1 3, 0, -1
	ex2 -1, -1, -1
	goto 28
@set-sdf = 22
	sdf -1, -1
	msg 8, 0, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 23
	sdf -1, -1
	msg 64, 65, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, -1, -1
	ex2 -1, -1, -1
	goto 25
@block-move = 25
	sdf -1, -1
	msg 66, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 26
	sdf -1, -1
	msg 67, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 500, 1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 28
	sdf 8, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-context = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 11
