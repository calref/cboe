@once-dlog = 0
	sdf 10, 0
	msg 0, -1, 1
	pic 8, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf -1, -1
	msg 6, -1, 1
	pic 22, 4
	ex1 8, 19, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto 3
@once-dlog = 3
	sdf -1, -1
	msg 12, -1, 1
	pic 22, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf 10, 1
	msg 18, -1, 1
	pic 13, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 5
	sdf 10, 2
	msg 24, -1, 1
	pic 13, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 6
@once-town-encounter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf -1, -1
	msg 30, -1, 1
	pic 22, 4
	ex1 8, 8, -1
	ex2 -1, -1, -1
	goto -1
@death = 8
	sdf -1, -1
	msg 36, 37, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 9
	sdf -1, -1
	msg 38, -1, -1
	pic 0, 4
	ex1 28, 11, -1
	ex2 0, -1, -1
	goto -1
@change-ter = 10
	sdf -1, -1
	msg 39, -1, -1
	pic 0, 4
	ex1 30, 11, -1
	ex2 0, -1, -1
	goto -1
@once-give-dlog = 11
	sdf 10, 3
	msg 40, -1, 10
	pic 73, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 12
	sdf 10, 4
	msg -1, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@once-dlog = 13
	sdf -1, -1
	msg 46, -1, 1
	pic 159, 1
	ex1 40, 14, -1
	ex2 37, 15, -1
	goto -1
@damage = 14
	sdf -1, -1
	msg 52, -1, -1
	pic 0, 4
	ex1 10, 10, -1
	ex2 20, 1, -1
	goto -1
@death = 15
	sdf -1, -1
	msg 53, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-context = 16
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 108, 17
	ex2 -1, -1, -1
	goto -1
@death = 17
	sdf -1, -1
	msg 54, 55, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@stair = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 46, 42, -1
	ex2 9, 1, -1
	goto -1
@disp-msg = 19
	sdf -1, -1
	msg 56, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
