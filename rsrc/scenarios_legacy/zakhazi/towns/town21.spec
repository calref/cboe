@once-dlog = 0
	sdf 21, 0
	msg 0, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf 21, 1
	msg 6, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 2
@change-ter = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 41, -1
	ex2 0, -1, -1
	goto 3
@set-sdf = 3
	sdf 100, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 4
@once-give-spec-item = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 5
	sdf 21, 2
	msg 12, 13, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 6
	sdf 21, 3
	msg 14, 15, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf -1, -1
	msg 16, -1, 0
	pic 31, 4
	ex1 9, 8, -1
	ex2 38, -1, -1
	goto -1
@once-dlog = 8
	sdf -1, -1
	msg 22, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 9
@gold = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3000, 0, -1
	ex2 -1, -1, -1
	goto 10
@xp = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 50, 0, -1
	ex2 -1, -1, -1
	goto 11
@call-global = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 25
@spell-mage = 12
	sdf -1, -1
	msg 28, -1, -1
	pic 0, 4
	ex1 58, 0, -1
	ex2 -1, -1, -1
	goto -1
@spell-priest = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 59, 0, -1
	ex2 -1, -1, -1
	goto -1
