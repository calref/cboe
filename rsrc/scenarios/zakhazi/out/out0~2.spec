@once = 0
	sdf 202, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once-dlog = 1
	sdf -1, -1
	msg 0, -1, 1
	pic 102, 1
	ex1 49, 2, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 4, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 3
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 4
	sdf 202, 0
	msg 7, -1, -1
	pic 102, 1
	ex1 81, 1000, -1
	ex2 -1, 5, -1
	goto -1
@once-give-item = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 248, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-trait = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 7, -1
	ex2 1, 2, -1
	goto 8
@disp-msg = 7
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@damage = 8
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 8, 8, -1
	ex2 0, 0, -1
	goto -1
@change-time = 9
	sdf -1, -1
	msg 15, 16, -1
	pic 0, 4
	ex1 2000, -1, -1
	ex2 -1, -1, -1
	goto 10
@change-ter = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 18, -1
	ex2 84, -1, -1
	goto -1
@once-set-sdf = 11
	sdf 202, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 9
