@stair = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 20, 42, -1
	ex2 11, -1, -1
	goto -1
@once-dlog = 1
	sdf 12, 0
	msg 6, -1, 1
	pic 72, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 2
	sdf 12, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 3
@rect-place-field = 3
	sdf 100, 5
	msg 12, -1, -1
	pic 0, 4
	ex1 4, 4, -1
	ex2 6, 9, -1
	goto -1
@lever-generic = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 5, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 5
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 41, 16, -1
	ex2 187, 174, -1
	goto -1
@lever-generic = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 7, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 16, -1
	ex2 170, 174, -1
	goto -1
@once-give-dlog = 8
	sdf 12, 2
	msg 14, -1, 2
	pic 138, 1
	ex1 -1, -1, -1
	ex2 -1, 9, -1
	goto -1
@disp-msg = 9
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 42, 10, -1
	ex2 155, -1, -1
	goto -1
@if-context = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 12
@move-party = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 39, 9, -1
	ex2 0, -1, -1
	goto -1
@if-context = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 14
@move-party = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 9, -1
	ex2 0, -1, -1
	goto -1
@change-ter = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 10, -1
	ex2 155, -1, -1
	goto -1
@change-ter = 16
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 37, 12, -1
	ex2 155, -1, -1
	goto -1
@block-move = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@change-ter = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 26, 4, -1
	ex2 97, -1, -1
	goto -1
@change-ter = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 26, 4, -1
	ex2 0, -1, -1
	goto -1
@once-disp-msg = 20
	sdf 12, 3
	msg 21, 22, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 21
	sdf -1, -1
	msg 23, 24, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@stair-generic = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 23, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 23
	sdf -1, -1
	msg 25, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 24
@if-ter = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 33, -1
	ex2 71, 25, -1
	goto 27
@change-ter = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 33, -1
	ex2 72, -1, -1
	goto 26
@change-ter = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 43, -1
	ex2 162, -1, -1
	goto -1
@change-ter = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 33, -1
	ex2 71, -1, -1
	goto 28
@change-ter = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 43, -1
	ex2 163, -1, -1
	goto -1
@once-town-encounter = 29
	sdf 12, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 30
	sdf 12, 5
	msg -1, -1, -1
	pic 27, 4
	ex1 3, 1, -1
	ex2 80, -1, -1
	goto -1
@once-trap = 31
	sdf 12, 6
	msg -1, -1, -1
	pic 27, 4
	ex1 4, 1, -1
	ex2 80, -1, -1
	goto -1
@if-boat = 99
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 29
